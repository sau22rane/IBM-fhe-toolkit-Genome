#include "mPS.h"

using namespace std;


void MPS::loadh5(const H5Parser& h5p, std::vector<string> names,
		std::vector<int> dims, int numFilledSlots){
	fpl1.setName(names.at(0));
	fpl1.initSize(dims.at(1),dims.at(0),numFilledSlots);
	fpl1.loadh5(h5p);
}