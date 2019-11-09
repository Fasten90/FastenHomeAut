echo Run CMake - GCC -Coverage


mkdir Out
cd Out
mkdir CMakeBuild_GccCoverage
cd CMakeBuild_GccCoverage


# Run CMake
# cmake CC=gcc ..
echo Execute cmake
cmake -DCMAKE_FASTENHOMEAUT_GCC_COVERAGE="TRUE" ../..


# Run make
echo Execute make
make -j4


# Start compiled exe
./FastenHomeAut


echo Coverage

gcov --all-blocks --function-summaries --branch-probabilities --branch-counts --unconditional-branches CMakeFiles/FastenHomeAut.dir/Src/main.c.gcno
gcov --all-blocks --function-summaries --branch-probabilities --branch-counts --unconditional-branches CMakeFiles/FastenHomeAut.dir/Src/Common/Handler/CommandHandler.c.gcno
gcov --all-blocks --function-summaries --branch-probabilities --branch-counts --unconditional-branches CMakeFiles/FastenHomeAut.dir/Src/Common/Helper/StringHelper.c.gcno


# Go to original directory
cd ../..

