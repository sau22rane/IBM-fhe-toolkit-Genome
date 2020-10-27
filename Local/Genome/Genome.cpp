#include <iostream>
using namespace std;

#include <helib/helib.h>
#include <helib/binaryArith.h>
#include <helib/intraSlot.h>
#include <fstream> 

class User{
  private:
    long v1_var, v2_var, v3_var, nslots;

  public:

    std::vector<helib::Ctxt> encrypted_v1;  
    std::vector<helib::Ctxt> encrypted_v2;
    std::vector<helib::Ctxt> encrypted_v3;

    User(){
      // Plaintext prime modulus.
      long p = 2;
      // Cyclotomic polynomial - defines phi(m).
      long m = 4095;
      // Hensel lifting (default = 1).
      long r = 1;
      // Number of bits of the modulus chain.
      long bits = 500;
      // Number of columns of Key-Switching matrix (typically 2 or 3).
      long c = 2;
      // Factorisation of m required for bootstrapping.
      std::vector<long> mvec = {7, 5, 9, 13};
      // Generating set of Zm* group.
      std::vector<long> gens = {2341, 3277, 911};
      // Orders of the previous generators.
      std::vector<long> ords = {6, 4, 6};
      std::cout << std::endl << "====================================================================================" << std::endl;
      std::cout << "Initialising User object..." << std::endl << std::endl;
      std::cout<<"Initialising context object for key generation..." << std::endl;
      // Initialize the context.
      helib::Context context(m, p, r, gens, ords);
      
      // Modify the context, adding primes to the modulus chain.
      std::cout << "Building modulus chain..." << std::endl;
      buildModChain(context, bits, c);

      // Make bootstrappable.
      context.makeBootstrappable(helib::convert<NTL::Vec<long>, std::vector<long>>(mvec));
      std::cout << "------------------------------------------------" << std::endl;
      std::cout << "Printing Context information..." << std::endl << std::endl << "  ";
      
      // Print the context.
      context.zMStar.printout();
      std::cout << "------------------------------------------------" << std::endl;
      // Secret key management.
      std::cout << "Creating secret key..." << std::endl;
      // Create a secret key associated with the context.
      helib::SecKey secret_key(context);
      // Generate the secret key.
      secret_key.GenSecKey();

      // Generate bootstrapping data.
      secret_key.genRecryptData();

      // Public key management.
      // Set the secret key (upcast: SecKey is a subclass of PubKey).
      std::cout << "Creating public key..." << std::endl;
      const helib::PubKey& public_key = secret_key;
      std::cout << "Saving public key to Key.txt file..." << std::endl;

      ofstream Key;
      Key.open("Key.txt");
      Key << secret_key;
      Key.close();
      std::cout << "Public key saved to Key.txt file..." << std::endl;

      // Get the EncryptedArray of the context.
      const helib::EncryptedArray& ea = *(context.ea);

      std::vector<helib::zzX> unpackSlotEncoding;
      buildUnpackSlotEncoding(unpackSlotEncoding, ea);

      // Get the number of slot (phi(m)).
      this->nslots = ea.size();
      std::cout << "Number of slots: " << nslots;
      std::cout << std::endl << "====================================================================================" << std::endl;
    }

    void LoadData(){
      std::cout << std::endl << "====================================================================================" << std::endl;

      std::cout << "Loading User data from data.txt file..." << std::endl;
      ifstream File;
      File.open("data.txt");
      File >> this->v1_var;
      File >> this->v2_var;
      File >> this->v3_var;
      File.close();
      std::cout << "User data loaded successfully..." << std::endl << std::endl;

      std::cout << "Pre-encryption User data:" << std::endl;
      std::cout << "Var1 = " << this->v1_var << std::endl;
      std::cout << "Var2 = " << this->v2_var << std::endl;
      std::cout << "Var3 = " << this->v3_var << std::endl;
    }

    void EncryptData(const helib::EncryptedArray& ea, const helib::PubKey& public_key, long bitSize){
      std::cout << "Encrypting User data........." << std::endl;
      for (long i = 0; i < bitSize; ++i) {
        std::vector<long> v1_vec(ea.size());
        std::vector<long> v2_vec(ea.size());
        std::vector<long> v3_vec(ea.size());
        // Extract the i'th bit of a,b,c.
        for (auto& slot : v1_vec)
          slot = (this->v1_var >> i) & 1;
        for (auto& slot : v2_vec)
          slot = (this->v2_var >> i) & 1;
        for (auto& slot : v3_vec)
          slot = (this->v3_var >> i) & 1;
        ea.encrypt(this->encrypted_v1[i], public_key, v1_vec);
        ea.encrypt(this->encrypted_v2[i], public_key, v2_vec);
        ea.encrypt(this->encrypted_v3[i], public_key, v3_vec);
      }
    std::cout << "User data encryption completed.....!!";
    std::cout << std::endl << "====================================================================================" << std::endl;
    }

    void GetResult(std::vector<long>& decrypted_result, helib::CtPtrs_vectorCt& result_wrapper, helib::SecKey& secret_key, const helib::EncryptedArray& ea){
      std::cout << std::endl << "====================================================================================" << std::endl;
      std::cout << "Decrypting Result returned by the server..." << std::endl << std::endl;
      std::cout << "Loading Private Key for decryption..." << std::endl;
      // long p = 2, m = 4095, r = 1, bits = 500, c = 2;
      // std::vector<long> mvec = {7, 5, 9, 13}, gens = {2341, 3277, 911}, ords = {6, 4, 6};
      // helib::Context context(m, p, r, gens, ords);
      // buildModChain(context, bits, c);
      // context.makeBootstrappable(helib::convert<NTL::Vec<long>, std::vector<long>>(mvec));
      // helib::SecKey secret_key(context);
      // ifstream Key;
      // Key.open("Key.txt");
      // Key >> secret_key;
      // Key.close();
      std::cout << "Private key loded successfully..." << std::endl;  
      helib::decryptBinaryNums(decrypted_result, result_wrapper, secret_key, ea);
      std::cout << "Decrypted result = " << decrypted_result.back() << std::endl;
      
      std::cout << "====================================================================================" << std::endl;
    }

};

class Server{
  private:
    long c1_coef, c2_coef, c3_coef, c4_const;
  public:
    std::vector<helib::Ctxt> encrypted_c1, encrypted_c2, encrypted_c3, encrypted_c4;
    std::vector<helib::zzX> unpackSlotEncoding;

    Server(){

    };
    
    void LoadData(){
      std::cout << std::endl << "====================================================================================" << std::endl;
      std::cout << "Loading weights from weight.txt file......." << std::endl;
      ifstream File;
      File.open("weights.txt");
      File >> this->c1_coef;
      File >> this->c2_coef;
      File >> this->c3_coef;
      File >> this->c4_const;
      File.close();
      std::cout << "Weights loaded successfully..." << std::endl << std::endl;

      std::cout << "Pre-encryption Server data:" << std::endl;
      std::cout << "Coef1 = " << this->c1_coef << std::endl;
      std::cout << "Coef2 = " << this->c2_coef << std::endl;
      std::cout << "Coef3 = " << this->c3_coef << std::endl;
      std::cout << "Const = " << this->c4_const << std::endl;
    }

    void EncryptData(const helib::EncryptedArray& ea, const helib::PubKey& public_key, long bitSize){
      std::cout << "Encrypting Server data........." << std::endl;
      for (long i = 0; i < bitSize; ++i) {
        std::vector<long> c1_vec(ea.size());
        std::vector<long> c2_vec(ea.size());
        std::vector<long> c3_vec(ea.size());
        std::vector<long> c4_vec(ea.size());
        // Extract the i'th bit of a,b,c.
        for (auto& slot : c1_vec)
          slot = (this->c1_coef >> i) & 1;
        for (auto& slot : c2_vec)
          slot = (this->c2_coef >> i) & 1;
        for (auto& slot : c3_vec)
          slot = (this->c3_coef >> i) & 1;
        for (auto& slot : c4_vec)
          slot = (this->c4_const >> i) & 1;
        ea.encrypt(this->encrypted_c1[i], public_key, c1_vec);
        ea.encrypt(this->encrypted_c2[i], public_key, c2_vec);
        ea.encrypt(this->encrypted_c3[i], public_key, c3_vec);
        ea.encrypt(this->encrypted_c4[i], public_key, c4_vec);
      }
      std::cout << "Server data encryption completed.....!!";
      std::cout << std::endl << "====================================================================================" << std::endl;
    }

    void Predict(User& user, long bitSize, long outSize, std::vector<helib::Ctxt>& encrypted_result, helib::CtPtrs_vectorCt& result_wrapper){
      std::cout << std::endl << "====================================================================================" << std::endl;
      std::cout << "Starting Homomorphic Computation......" << std::endl;
      std::vector<long> decrypted_result;

      // first term
      std::vector<helib::Ctxt> encrypted_product1;
      helib::CtPtrs_vectorCt product_wrapper1(encrypted_product1);
      helib::multTwoNumbers(
          product_wrapper1,
          helib::CtPtrs_vectorCt(this->encrypted_c1),
          helib::CtPtrs_vectorCt(user.encrypted_v1),
          /*rhsTwosComplement=*/false, // This means the rhs is unsigned rather than
                                      // 2's complement.
          outSize, // Outsize is the limit on the number of bits in the output.
          &unpackSlotEncoding); // Information needed for bootstrapping.
      std::cout << "Procuct 1 computed..." << std::endl;

      // second term

      decrypted_result.clear();
      std::vector<helib::Ctxt> encrypted_product2;
      helib::CtPtrs_vectorCt product_wrapper2(encrypted_product2);
      helib::multTwoNumbers(
          product_wrapper2,
          helib::CtPtrs_vectorCt(this->encrypted_c2),
          helib::CtPtrs_vectorCt(user.encrypted_v2),
          /*rhsTwosComplement=*/false, // This means the rhs is unsigned rather than
                                      // 2's complement.
          outSize, // Outsize is the limit on the number of bits in the output.
          &unpackSlotEncoding); // Information needed for bootstrapping.
  
      std::cout << "Procuct 2 computed..." << std::endl;
  
      //third term

      decrypted_result.clear();
      std::vector<helib::Ctxt> encrypted_product3;
      helib::CtPtrs_vectorCt product_wrapper3(encrypted_product3);
      helib::multTwoNumbers(
          product_wrapper3,
          helib::CtPtrs_vectorCt(this->encrypted_c3),
          helib::CtPtrs_vectorCt(user.encrypted_v3),
          /*rhsTwosComplement=*/false, // This means the rhs is unsigned rather than
                                      // 2's complement.
          outSize, // Outsize is the limit on the number of bits in the output.
          &unpackSlotEncoding); // Information needed for bootstrapping.

      std::cout << "Procuct 3 computed..." << std::endl;
      // helib::CtPtrs_vectorCt(encrypted_c3),

      // Now perform the encrypted sum and put it in encrypted_result.
      
      
      std::vector<std::vector<helib::Ctxt>> summands1 = {encrypted_product1,
                                                        encrypted_product2,
                                                        encrypted_product3,
                                                        this->encrypted_c4};
      helib::CtPtrMat_vectorCt summands_wrapper1(summands1);
      helib::addManyNumbers(
          result_wrapper,
          summands_wrapper1,
          0,                    // sizeLimit=0 means use as many bits as needed.
          &unpackSlotEncoding); // Information needed for bootstrapping.
      
          std::cout << "Summation computed..." << std::endl << std::endl;
          std::cout <<"Returning encrypted result..." << std::endl;
          std::cout << "====================================================================================" << std::endl;

    }

};

int main(int argc, char* argv[])
{
  long p = 2, m = 4095, r = 1, bits = 500, c = 2;
  std::vector<long> mvec = {7, 5, 9, 13}, gens = {2341, 3277, 911}, ords = {6, 4, 6};
  helib::Context context(m, p, r, gens, ords);
  buildModChain(context, bits, c);
  context.makeBootstrappable(helib::convert<NTL::Vec<long>, std::vector<long>>(mvec));
  helib::SecKey secret_key(context);

  // Initialising User object
  User user = User();
  ifstream Key;
  Key.open("Key.txt");
  Key >> secret_key;
  Key.close();
  const helib::PubKey& public_key = secret_key;
  const helib::EncryptedArray& ea = *(context.ea);


  long bitSize = 20;
  long outSize = 2 * bitSize;

  helib::Ctxt scratch(public_key);

  user.encrypted_v1 = std::vector<helib::Ctxt>(bitSize, scratch);
  user.encrypted_v2 = std::vector<helib::Ctxt>(bitSize, scratch);
  user.encrypted_v3 = std::vector<helib::Ctxt>(bitSize, scratch);
  user.LoadData();
  user.EncryptData(ea, public_key, bitSize);



  Server server = Server();
  buildUnpackSlotEncoding(server.unpackSlotEncoding, ea);
  server.LoadData();
  // Use a scratch ciphertext to populate vectors.
  server.encrypted_c1 = std::vector<helib::Ctxt>(bitSize, scratch);
  server.encrypted_c2 = std::vector<helib::Ctxt>(bitSize, scratch);
  server.encrypted_c3 = std::vector<helib::Ctxt>(bitSize, scratch);
  server.encrypted_c4 = std::vector<helib::Ctxt>(bitSize, scratch);
  
  // Encrypt the data in binary representation.
  server.EncryptData(ea, public_key, bitSize);

  std::vector<helib::Ctxt> encrypted_result;
  helib::CtPtrs_vectorCt result_wrapper(encrypted_result);
  server.Predict(user, bitSize, outSize, encrypted_result, result_wrapper);

  std::vector<long> decrypted_result;
  
  user.GetResult(decrypted_result, result_wrapper, secret_key, ea);

  std::cout << "Task Completed successfully...!" << std::endl;
  std::cout << "Exiting...!" << std::endl << std::endl;

  return 0;
}
