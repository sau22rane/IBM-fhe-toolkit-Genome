#include <iostream>
using namespace std;

#include <helib/helib.h>
#include <helib/binaryArith.h>
#include <helib/intraSlot.h>
#include <fstream> 

int main(int argc, char* argv[])
{
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
  std::cout<<"Initialising context object..." << std::endl;
  // Initialize the context.
  helib::Context context(m, p, r, gens, ords);
  
  // Modify the context, adding primes to the modulus chain.
  std::cout << "Building modulus chain..." << std::endl;
  buildModChain(context, bits, c);

  // Make bootstrappable.
  context.makeBootstrappable(
      helib::convert<NTL::Vec<long>, std::vector<long>>(mvec));
  


  // Print the context.
  context.zMStar.printout();
  std::cout << "====================================================================================" << std::endl;

  // Secret key management.
  std::cout << std::endl << "====================================================================================" << std::endl;
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

  // Get the EncryptedArray of the context.
  const helib::EncryptedArray& ea = *(context.ea);



  std::vector<helib::zzX> unpackSlotEncoding;
  buildUnpackSlotEncoding(unpackSlotEncoding, ea);

  // Get the number of slot (phi(m)).
  long nslots = ea.size();
  std::cout << "Number of slots: " << nslots;
  std::cout << std::endl << "====================================================================================" << std::endl;


  long bitSize = 20;
  long outSize = 2 * bitSize;

  /* 4.625124
    0.054446
    0.107001
    0.000336 */
  long c1_coef, c2_coef, c3_coef, c4_const;

  long v1_var, v2_var, v3_var;
  helib::Ctxt scratch(public_key);



  std::cout << std::endl << "====================================================================================" << std::endl;

  std::cout << "Loading User data from data.txt file......" << std::endl;
  ifstream File;
  File.open("data.txt");
  File >> v1_var;
  File >> v2_var;
  File >> v3_var;
  File.close();

  std::cout << "Pre-encryption User data:" << std::endl;
  std::cout << "Var1 = " << v1_var << std::endl;
  std::cout << "Var2 = " << v2_var << std::endl;
  std::cout << "Var3 = " << v3_var << std::endl;
  std::cout << "Encrypting User data........." << std::endl;
  std::vector<helib::Ctxt> encrypted_v1(bitSize, scratch);
  std::vector<helib::Ctxt> encrypted_v2(bitSize, scratch);
  std::vector<helib::Ctxt> encrypted_v3(bitSize, scratch);

  
  // Encrypt the data in binary representation.
  for (long i = 0; i < bitSize; ++i) {
      std::vector<long> v1_vec(ea.size());
      std::vector<long> v2_vec(ea.size());
      std::vector<long> v3_vec(ea.size());
      // Extract the i'th bit of a,b,c.
      for (auto& slot : v1_vec)
        slot = (v1_var >> i) & 1;
      for (auto& slot : v2_vec)
        slot = (v2_var >> i) & 1;
      for (auto& slot : v3_vec)
        slot = (v3_var >> i) & 1;
      ea.encrypt(encrypted_v1[i], public_key, v1_vec);
      ea.encrypt(encrypted_v2[i], public_key, v2_vec);
      ea.encrypt(encrypted_v3[i], public_key, v3_vec);
  }
  std::cout << "User data encryption completed.....!!";
  std::cout << std::endl << "====================================================================================" << std::endl;



  std::cout << std::endl << "====================================================================================" << std::endl;

  std::cout << "Loading weights from weight.txt file......." << std::endl;
  File.open("weights.txt");
  File >> c1_coef;
  File >> c2_coef;
  File >> c3_coef;
  File >> c4_const;
  File.close();

  std::cout << "Pre-encryption Server data:" << std::endl;
  std::cout << "Coef1 = " << c1_coef << std::endl;
  std::cout << "Coef2 = " << c2_coef << std::endl;
  std::cout << "Coef3 = " << c3_coef << std::endl;
  std::cout << "Const = " << c4_const << std::endl;
  std::cout << "Encrypting Server data........." << std::endl;

  // Use a scratch ciphertext to populate vectors.
  std::vector<helib::Ctxt> encrypted_c1(bitSize, scratch);
  std::vector<helib::Ctxt> encrypted_c2(bitSize, scratch);
  std::vector<helib::Ctxt> encrypted_c3(bitSize, scratch);
  std::vector<helib::Ctxt> encrypted_c4(bitSize, scratch);


  // Encrypt the data in binary representation.
  for (long i = 0; i < bitSize; ++i) {
      std::vector<long> c1_vec(ea.size());
      std::vector<long> c2_vec(ea.size());
      std::vector<long> c3_vec(ea.size());
      std::vector<long> c4_vec(ea.size());
      // Extract the i'th bit of a,b,c.
      for (auto& slot : c1_vec)
        slot = (c1_coef >> i) & 1;
      for (auto& slot : c2_vec)
        slot = (c2_coef >> i) & 1;
      for (auto& slot : c3_vec)
        slot = (c3_coef >> i) & 1;
      for (auto& slot : c4_vec)
        slot = (c4_const >> i) & 1;
      ea.encrypt(encrypted_c1[i], public_key, c1_vec);
      ea.encrypt(encrypted_c2[i], public_key, c2_vec);
      ea.encrypt(encrypted_c3[i], public_key, c3_vec);
      ea.encrypt(encrypted_c4[i], public_key, c4_vec);
  }
  std::cout << "Server data encryption completed.....!!";
  std::cout << std::endl << "====================================================================================" << std::endl;

  


  std::cout << std::endl << "====================================================================================" << std::endl;
  std::cout << "Starting Homomorphic Computation......" << std::endl;
  std::vector<long> decrypted_result;

  // first term
  std::vector<helib::Ctxt> encrypted_product1;
  helib::CtPtrs_vectorCt product_wrapper1(encrypted_product1);
  helib::multTwoNumbers(
      product_wrapper1,
      helib::CtPtrs_vectorCt(encrypted_c1),
      helib::CtPtrs_vectorCt(encrypted_v1),
      /*rhsTwosComplement=*/false, // This means the rhs is unsigned rather than
                                   // 2's complement.
      outSize, // Outsize is the limit on the number of bits in the output.
      &unpackSlotEncoding); // Information needed for bootstrapping.
  helib::decryptBinaryNums(decrypted_result, product_wrapper1, secret_key, ea);
  std::cout << "Product1 = " << decrypted_result.back() << std::endl;

  // second term

  decrypted_result.clear();
  std::vector<helib::Ctxt> encrypted_product2;
  helib::CtPtrs_vectorCt product_wrapper2(encrypted_product2);
  helib::multTwoNumbers(
      product_wrapper2,
      helib::CtPtrs_vectorCt(encrypted_c2),
      helib::CtPtrs_vectorCt(encrypted_v2),
      /*rhsTwosComplement=*/false, // This means the rhs is unsigned rather than
                                   // 2's complement.
      outSize, // Outsize is the limit on the number of bits in the output.
      &unpackSlotEncoding); // Information needed for bootstrapping.
  helib::decryptBinaryNums(decrypted_result, product_wrapper2, secret_key, ea);
  std::cout << "Product2 = " << decrypted_result.back() << std::endl;
  
  //third term

  decrypted_result.clear();
  std::vector<helib::Ctxt> encrypted_product3;
  helib::CtPtrs_vectorCt product_wrapper3(encrypted_product3);
  helib::multTwoNumbers(
      product_wrapper3,
      helib::CtPtrs_vectorCt(encrypted_c3),
      helib::CtPtrs_vectorCt(encrypted_v3),
      /*rhsTwosComplement=*/false, // This means the rhs is unsigned rather than
                                   // 2's complement.
      outSize, // Outsize is the limit on the number of bits in the output.
      &unpackSlotEncoding); // Information needed for bootstrapping.
  helib::decryptBinaryNums(decrypted_result, product_wrapper3, secret_key, ea);
  std::cout << "Product3 = " << decrypted_result.back() << std::endl;
  
  // helib::CtPtrs_vectorCt(encrypted_c3),

  // Now perform the encrypted sum and put it in encrypted_result.
  std::vector<helib::Ctxt> encrypted_result;
  helib::CtPtrs_vectorCt result_wrapper(encrypted_result);
  
  
  decrypted_result.clear();
  std::vector<std::vector<helib::Ctxt>> summands1 = {encrypted_product1,
                                                    encrypted_product2,
                                                    encrypted_product3,
                                                    encrypted_c4};
  helib::CtPtrMat_vectorCt summands_wrapper1(summands1);
  helib::addManyNumbers(
      result_wrapper,
      summands_wrapper1,
      0,                    // sizeLimit=0 means use as many bits as needed.
      &unpackSlotEncoding); // Information needed for bootstrapping.
  helib::decryptBinaryNums(decrypted_result, result_wrapper, secret_key, ea);
  std::cout << "Predicted = " << decrypted_result.back() << std::endl;

  decrypted_result.clear();
 
  encrypted_result.resize(4lu, scratch);
  decrypted_result.clear();
  encrypted_c1.pop_back(); // drop the MSB since we only support up to 15 bits.
  helib::fifteenOrLess4Four(result_wrapper,
                            helib::CtPtrs_vectorCt(encrypted_c1));

  // Decrypt and print the result.
  helib::decryptBinaryNums(decrypted_result, result_wrapper, secret_key, ea);
  std::cout << "popcnt(a) = " << decrypted_result.back() << std::endl;

  cout<<"Success";

  return 0;
}
