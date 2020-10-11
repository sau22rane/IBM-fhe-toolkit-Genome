#include <iostream>
using namespace std;

#include <helib/helib.h>
#include <helib/binaryArith.h>
#include <helib/intraSlot.h>

int main(int argc, char* argv[])
{
  /*  Example of binary arithmetic using the BGV scheme  */

  // First set up parameters.
  //
  // NOTE: The parameters used in this example code are for demonstration only.
  // They were chosen to provide the best performance of execution while
  // providing the context to demonstrate how to use the "Binary Arithmetic
  // APIs". The parameters do not provide the security level that might be
  // required by real use/application scenarios.

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

  std::cout << "Initialising context object..." << std::endl;
  // Initialize the context.
  helib::Context context(m, p, r, gens, ords);
  
  // Modify the context, adding primes to the modulus chain.
  std::cout << "Building modulus chain..." << std::endl;
  buildModChain(context, bits, c);

  // Make bootstrappable.
  context.makeBootstrappable(
      helib::convert<NTL::Vec<long>, std::vector<long>>(mvec));
  





  cout<<"Success";


  return 0;
}