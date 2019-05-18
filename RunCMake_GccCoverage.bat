@echo off
echo Run CMake - GCC -Coverage


mkdir Out
cd Out
mkdir CMakeBuild_GccCoverage
cd CMakeBuild_GccCoverage


:: Run CMake
:: cmake CC=gcc ..
echo Execute cmake
cmake CC=gcc -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND" -DCMAKE_FASTENHOMEAUT_GCC_COERAGE="TRUE" ../..


:: Run make
echo Execute make
make -j4


:: Start compiled exe
FastenHomeAut.exe


echo Coverage

:: gcov ../../Src/main.c
:: Works:
::gcov CMakeFiles/FastenHomeAut.dir/Src/main.c.gcno
::gcov --function-summaries CMakeFiles/FastenHomeAut.dir/Src/main.c.gcno

gcov --function-summaries --branch-probabilities --branch-counts --unconditional-branches CMakeFiles/FastenHomeAut.dir/Src/main.c.gcno


::gcc -fprofile-arcs -ftest-coverage main.c

::gcc -fprofile-arcs -ftest-coverage ../../Src/main.c

::gcc -fprofile-arcs -ftest-coverage CMakeFiles/FastenHomeAut.dir/Src/main.c

::gcc -fprofile-arcs -ftest-coverage CMakeFiles/FastenHomeAut.dir/Src/main.c.gcno




:: Go to original directory
cd ../..
