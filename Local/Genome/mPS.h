#ifndef SRC_ML_HE_EXAMPLES_mPS_H
#define SRC_ML_HE_EXAMPLES_mPS_H

#include "Layer.h"
#include "FcLayer.h"
#include "DoubleMatrixArray.h"
#include "h5Parser.h"
#include <iostream>

class MPS{
    FcPlainLayer fpl1;
    public:
    MPS(){};
  void loadh5(const H5Parser& h5p, std::vector<std::string> names, std::vector<int> dims, int numFilledSlots);

};

#endif 
	