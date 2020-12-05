project(Genome)
ENABLE_LANGUAGE(CXX)

# We need a recent cmake
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# We rely on HELib...
find_package(helib 1.0.0 REQUIRED)

# We rely on HDF5 for data access...
find_package(HDF5 REQUIRED COMPONENTS CXX)
include_directories(${HDF5_INCLUDE_DIR})

# We rely on Boost C++
find_package(Boost REQUIRED COMPONENTS filesystem)

# Build the credit_card_fraud object from credit_card_fraud.cpp entry point
add_executable(Genome Genome.cpp)

INCLUDE_DIRECTORIES(/usr/local/include/simple_ml_helib)

# Link credit_card_fraud, hdf5, helib, boost, and simple_ml_helib
target_link_libraries(Genome -lsimple_ml_helib ${HDF5_LIBRARIES} helib ${Boost_LIBRARIES})
