echo Run CMake - GCC -Coverage


mkdir Out
cd Out
mkdir CMakeBuild_GccCoverage
cd CMakeBuild_GccCoverage


# Run CMake
# cmake CC=gcc ..
echo Execute cmake
cmake -DCMAKE_FASTENHOMEAUT_GCC_COERAGE="TRUE" ../..


# Run make
echo Execute make
make -j4


# Start compiled exe
./FastenHomeAut



:: Go to original directory
cd ../..

