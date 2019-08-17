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


echo Coverage

# TODO: Another files + PATH fixes
gcov --all-blocks --function-summaries --branch-probabilities --branch-counts --unconditional-branches main.c


# Go to original directory
cd ../..

