// temporary code for testing build process

#include<iostream>
#include <helib/helib.h>
using namespace helib;
using namespace std;

int main()
{
    cout<<"Hello HElib"<<endl; 
  // Plaintext prime modulus
  unsigned long p = 4999;
  // Cyclotomic polynomial - defines phi(m)
  unsigned long m = 32109;
  // Hensel lifting (default = 1)
  unsigned long r = 1;
  // Number of bits of the modulus chain
  unsigned long bits = 500;
  // Number of columns of Key-Switching matrix (default = 2 or 3)
  unsigned long c = 2;

  cout << "Initialising context object..." << endl;
  // Initialize context
  Context context(m, p, r);
  // Modify the context, adding primes to the modulus chain
  cout << "Building modulus chain..." << endl;
  buildModChain(context, bits, c);

  // Secret key management
  cout << "Creating secret key..." << endl;
  // Create a secret key associated with the context
  SecKey secret_key(context);
  // Generate the secret key
  secret_key.GenSecKey();
  cout << "Generating key-switching matrices..." << endl;
  // Compute key-switching matrices that we need
  addSome1DMatrices(secret_key);

  // Public key management
  // Set the secret key (upcast: SecKey is a subclass of PubKey)
  const PubKey& public_key = secret_key;


  return 0;
}