#include <iostream>
using namespace std;

#include <helib/helib.h>
#include <helib/binaryArith.h>
#include <helib/intraSlot.h>
#include <fstream>
#include "HelibCkksContext.h"
#include "HelibConfig.h"
#include "FileUtils.h"
#include "CipherMatrixEncoder.h" 
#include "Layer.h"
#include "FcLayer.h"
#include "SimpleTrainingSet.h"
#include "DoubleMatrix.h"
#include "h5Parser.h"
#include "SimpleFcLayer.h"
#include "SimpleTimer.h"
#include "CipherMatrix.h"

#include <fstream> 
// using namespace boost::numeric::ublas;



const string outDir = "./Genome_output";
const string clientContext = outDir + "/client_context.bin";
const string serverContext = outDir + "/server_context.bin";
const string encryptedModelFile = outDir + "/encrypted_model.bin";
const string encryptedSamplesFile = outDir + "/encrypted_batch_samples_1.bin";
  const string encryptedPredictionsFile = outDir + "/encrypted_batch_predictions_1.bin";

const string plainModelFile  = "./data/weights.h5";
const string plainSamplesFile  = "./data/data.txt";
const string plainLabelsFile  = "./data/test_y.h5";

int numSlots = 512;
int batchSize = numSlots;

int numBatches = -1; // to be overwritten when loading the data set
double classificationThreshold = 0.5; // used to separate positive from negative samples

void createContexts() {
  
  cout << "Initalizing HElib . . ." << endl;
  HelibConfig conf;
  conf.m=numSlots*2*2;
  if (numSlots==16384 || numSlots==32768) {   
    conf.r=50;
    conf.L=700;
  } else if (numSlots==512) {
    conf.r=52;
    conf.L=1024;
  } else
    throw runtime_error("configuration not tested for given number of slots");

  HelibCkksContext he;
  he.init(conf);
  he.printSignature(cout);

  cout << "Clearing " << outDir << endl;
  FileUtils::createCleanDir(outDir);
  cout << "Saving client side context to " << clientContext << endl;
  bool withSecretKey = true;
  he.saveToFile(clientContext, withSecretKey); // save client context

  cout << "Saving server side context to " << serverContext << endl;
  withSecretKey = false;
  he.saveToFile(serverContext, withSecretKey); // save server context

}

class GenomePlainLayer{
  DoubleMatrix *weights;
  DoubleMatrix *bias;
  string name;
  int index;

  public:
    GenomePlainLayer(const H5Parser& h5p, string name, int a, int b);
    ~GenomePlainLayer() {}
    inline const DoubleMatrix& getWeights() const {
      return *weights;
    }
    inline const DoubleMatrix& getBias() const {
      return *bias;
    }
    void initSize(int rows, int cols);
    void setName(string name){this->name = name;};
    string getName(){return this->name;}
    DoubleMatrix forward(const DoubleMatrix& inVec);
    void loadh5(const H5Parser& h5File);
    
    static DoubleMatrix loadFcMatrixFromH5File(const H5Parser& h5P, const string& path) {
      vector<vector<double> > FcMatrix = h5P.parseFC(path);
      DoubleMatrix res(FcMatrix);
      res.transpose();
      return res;
    }


};

class EncryptedLayer{
  HeContext& he;

	CipherMatrix weights;

	CipherMatrix bias;

  public:
    EncryptedLayer(HeContext& he);
    ~EncryptedLayer();
    std::streamoff save(std::ostream& stream) const;
    std::streamoff load(std::istream& stream);
    void initFromLayer(const GenomePlainLayer& fpl, int baseChainIndex = -1);
    CipherMatrix forward(const CipherMatrix& inVec) const;
    void predict(const CipherMatrix& input, CipherMatrix& output) const;
};

class PlainData{
  DoubleMatrix *data;
  HeContext& he;
  CipherMatrix enData;
  public:
    PlainData(HeContext& he);
    ~PlainData(){};
    void loadData(string name, int n);
    void encrypt(int baseChainIndex = -1);
    std::streamoff save(std::ostream& stream) const;
};


class Client{
  
  HelibCkksContext he;
  SimpleTrainingSet ts;
	DoubleMatrixArray *Predictions;

	int currentBatch;

  public: 
    Client() : ts(batchSize), currentBatch(0) {
	  }

    void init(){
      H5Parser parser(plainModelFile);
      cout << "CLIENT: loading client side context . . ." << endl;
      he.loadFromFile(clientContext); // load context from file
      he.printSignature(cout);

      cout << "CLIENT: loading plain model . . ." << endl;
      GenomePlainLayer layer(parser, "dense_3",8,12);

      cout << "CLIENT: encrypting plain model . . ." << endl;
      // model.initFromNet();
      EncryptedLayer enLayer(he);
      enLayer.initFromLayer(layer);


      cout << "CLIENT: saving encrypted model . . ." << endl;
      // saving the encrypted NN to file to be later used by the server
      ofstream ofs(encryptedModelFile, ios::out | ios::binary);
      enLayer.save(ofs);

      PlainData userData(he);
      userData.loadData(plainSamplesFile, 8);
      userData.encrypt();

      cout << "CLIENT: saving encrypted samples . . ." << endl;
      std::ofstream ofs2(encryptedSamplesFile, ios::out | ios::binary);
      userData.save(ofs2);
      ofs2.close();
    }

    void decryptPredictions(const string& encryptedPredictionsFile){
      const CipherMatrixEncoder encoder(he);
      const CTile prototype(he);

      cout << "CLIENT: loading encrypted predictions . . ." << endl;
      CipherMatrix encryptedPredictions(prototype);
      std::ifstream ifs(encryptedPredictionsFile, ios::in | ios::binary);
      encryptedPredictions.load(ifs);
      ifs.close();

      cout << "CLIENT: decrypting predictions . . ." << endl;
      Predictions = new DoubleMatrixArray(encoder.decryptDecodeDouble(encryptedPredictions));
    }

    
    void assessResults(){

      cout << "CLIENT: assessing results so far . . ." << endl;

      // cout<<Predictions->size()<<endl;
      DoubleMatrix t = Predictions->getMat(0);
      cout<<"\nWeights * User data:\n"<<endl;
      cout<<t<<endl;
      
    }

};



class Server{
  HelibCkksContext he;
  shared_ptr<EncryptedLayer> encryptedLayer;
  public:
    void init(){
      cout << "SERVER: loading server side context . . ." << endl;
      he.loadFromFile(serverContext); // load context from file
      he.printSignature(cout);

      cout << "SERVER: loading encrypted model . . ." << endl;
      // load the encrypted model from file that was saved by the client into an encrypted NN object
      ifstream ifs(encryptedModelFile, ios::in | ios::binary);
      EncryptedLayer temp(he);
      temp.load(ifs);
      ifs.close();

      encryptedLayer = make_shared<EncryptedLayer>(temp);
	}

  void processEncryptedSamples(const string& encryptedSamplesFile, const string& encryptedPredictionsFile){
    const CipherMatrixEncoder encoder(he);
    const CTile prototype(he);

		cout << "SERVER: loading encrypted samples . . ." << endl;
		// load a batch of encrypted samples from file that was saved by the client
    CipherMatrix encryptedSamples(prototype);
	  std::ifstream ifs(encryptedSamplesFile, ios::in | ios::binary);
	  encryptedSamples.load(ifs);
	  ifs.close();

		cout << "SERVER: predicting over encrypted samples . . ." << endl;
    CipherMatrix encryptedPredictions(prototype);
	  encryptedLayer->predict(encryptedSamples, encryptedPredictions);

		cout << "SERVER: saving encrypted predictions . . ." << endl;
		// save predictions to be later loaded by the client
	  std::ofstream ofs(encryptedPredictionsFile, ios::out | ios::binary);
    encryptedPredictions.save(ofs);
    ofs.close();
	}

};



int main(int argc, char* argv[]){
  createContexts();
  cout<<"Success";
  Client client;
  client.init();

  Server server;
  server.init();
  server.processEncryptedSamples(encryptedSamplesFile, encryptedPredictionsFile);

  client.decryptPredictions(encryptedPredictionsFile);
  client.assessResults();

  return 0;
}



GenomePlainLayer::GenomePlainLayer(const H5Parser& h5p, string name, int a, int b) {
  setName(name);
  initSize(b,a);
  loadh5(h5p);
}

void GenomePlainLayer::initSize(int rows, int cols) {
  double init = 0;
  weights = new DoubleMatrix(rows, cols, init);
  bias = new DoubleMatrix(rows, 1, init);
}

DoubleMatrix GenomePlainLayer::forward(const DoubleMatrix& inVec) {
  SimpleTimer st("GenomePlainLayer::forward");

  DoubleMatrix lastInput = inVec;
  if (lastInput.rows()==1 && lastInput.cols()>1)
    lastInput.transpose();
  DoubleMatrix res = weights->getMultiply(lastInput);
  // res += *bias;
  return res;
}

void GenomePlainLayer::loadh5(const H5Parser& h5P) {
  string path=getName()+"/"+getName();
  // cout<<weights->size()<<endl;
  // cout<<bias->size()<<endl;
  weights->copy(loadFcMatrixFromH5File(h5P, path));
  bias->unFlatten(h5P.parseBias(path));
  cout<<"\nWeights:\n"<<*weights<<endl<<endl<<"Bias:\n"<<*bias<<endl;
}

boost::numeric::ublas::tensor<double> getTensor(DoubleMatrix temp){
	boost::numeric::ublas::tensor<double> tensor{(long unsigned int) temp.rows(), (long unsigned int) temp.cols(),1};
	for(int k=0; k<tensor.size(2); ++k){
		for(int i=0; i<tensor.size(0); ++i){
			for(int j=0; j<tensor.size(1); ++j)
				tensor.at(i,j,k) = temp.get(i,j);
		}
	}
	return tensor;
}





EncryptedLayer::EncryptedLayer(HeContext& he) :
		he(he), weights(CTile(he)), bias(CTile(he)) {
}

EncryptedLayer::~EncryptedLayer(){

}

streamoff EncryptedLayer::save(ostream& stream) const {
  SimpleTimer::Guard guard("EncryptedLayer::save");

  const streampos streamStartPos = stream.tellp();

  weights.save(stream);
  bias.save(stream);

  const streampos streamEndPos = stream.tellp();

  return streamEndPos - streamStartPos;
}

streamoff EncryptedLayer::load(istream& stream){
  SimpleTimer::Guard guard("EncryptedLayer::load");

  const streampos streamStartPos = stream.tellg();

  weights.load(stream);
  bias.load(stream);

  const streampos streamEndPos = stream.tellg();

  return streamEndPos - streamStartPos;
}

void EncryptedLayer::initFromLayer(const GenomePlainLayer& fpl, int baseChainIndex){
	SimpleTimer::Guard guard("EncryptedLayer::initFromLayer");
  if(!he.automaticallyManagesChainIndices() && baseChainIndex == -1)
		baseChainIndex = he.getTopChainIndex();

	if(!he.automaticallyManagesChainIndices()){
		if(baseChainIndex < -1 || baseChainIndex > he.getTopChainIndex())
			throw invalid_argument("Illegal chain index value");
		if(baseChainIndex == -1)
			baseChainIndex = he.getTopChainIndex();
	}

  boost::numeric::ublas::tensor<double> weightsVals = getTensor(fpl.getWeights());
  boost::numeric::ublas::tensor<double> biasVals = getTensor(fpl.getBias());

  const CipherMatrixEncoder encoder(he);
  encoder.encodeEncrypt(weights, weightsVals, baseChainIndex);
  encoder.encodeEncrypt(bias, biasVals, baseChainIndex - 1);
}

CipherMatrix EncryptedLayer::forward(const CipherMatrix& inVec) const {
	SimpleTimer::Guard guard("EncryptedLayer::forward");

	CipherMatrix res = weights.getMatrixMultiply(inVec);
	// res.add(bias);

	return res;
}

void EncryptedLayer::predict(const CipherMatrix& input, CipherMatrix& output) const{
  output = forward(input);
}






PlainData::PlainData(HeContext& he) :
		he(he), enData(CTile(he)){
}

void PlainData::loadData(string name, int n){
  std::cout << "Loading User data from data.txt file..." << std::endl;
  ifstream File;
  File.open(name);
  double init = 0;
  this->data = new DoubleMatrix(n, 1, init);
  double temp = 0;
  for(int i = 0;i<n;i++){
    File >> temp;
    data->set(i, 0, temp); 
  }

  cout<<"\nUser data:\n"<<*data;

}

void PlainData::encrypt(int baseChainIndex){
  SimpleTimer::Guard guard("PlainData::encrypt");
  if(!he.automaticallyManagesChainIndices() && baseChainIndex == -1)
		baseChainIndex = he.getTopChainIndex();

	if(!he.automaticallyManagesChainIndices()){
		if(baseChainIndex < -1 || baseChainIndex > he.getTopChainIndex())
			throw invalid_argument("Illegal chain index value");
		if(baseChainIndex == -1)
			baseChainIndex = he.getTopChainIndex();
	}

  const CipherMatrixEncoder encoder(he);
  boost::numeric::ublas::tensor<double> weightsVals = getTensor(*data);
  encoder.encodeEncrypt(enData, weightsVals, baseChainIndex);
}

streamoff PlainData::save(ostream& stream) const {
  SimpleTimer::Guard guard("EncryptedLayer::save");

  const streampos streamStartPos = stream.tellp();

  enData.save(stream);

  const streampos streamEndPos = stream.tellp();

  return streamEndPos - streamStartPos;
}
