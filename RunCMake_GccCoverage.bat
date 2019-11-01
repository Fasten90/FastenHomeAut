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


:: Clone SCC
:: PIPELINE_WORKSPACE - Define available at Pipeline (e.g. Azure)
if defined PIPELINE_WORKSPACE (
  echo PIPELINE_WORKSPACE is defined, download GccCoverageParser
  
  :: Go to original directory
  cd ../..

  mkdir Tools
  cd Tools
 
  git clone https://github.com/Fasten90/GccCoverageParser.git
  
  cd Out
  cd CMakeBuild_GccCoverage
)



:: TODO: Another files?
:: gcov ../../Src/main.c
:: Works:
::gcov CMakeFiles/FastenHomeAut.dir/Src/main.c.gcno
::gcov --function-summaries CMakeFiles/FastenHomeAut.dir/Src/main.c.gcno

:: gcov -abcfu lib.c
:: --all-blocks                Show information for every basic block
::echo Test commands
::gcov --all-blocks --function-summaries --branch-probabilities --branch-counts --unconditional-branches CMakeFiles/FastenHomeAut.dir/Src/main.c.gcno
::gcov --all-blocks --function-summaries --branch-probabilities --branch-counts --unconditional-branches CMakeFiles/FastenHomeAut.dir/Src/Common/Handler/CommandHandler.c.gcno
::gcov --all-blocks --function-summaries --branch-probabilities --branch-counts --unconditional-branches CMakeFiles/FastenHomeAut.dir/Src/Common/Helper/StringHelper.c.gcno

echo Run GccCoverageParser

python ../../Tools/GccCoverageParser/Run_GccCoverage_ForSourceFiles.py



::gcc -fprofile-arcs -ftest-coverage main.c

::gcc -fprofile-arcs -ftest-coverage ../../Src/main.c

::gcc -fprofile-arcs -ftest-coverage CMakeFiles/FastenHomeAut.dir/Src/main.c

::gcc -fprofile-arcs -ftest-coverage CMakeFiles/FastenHomeAut.dir/Src/main.c.gcno



:: Go to original directory
cd ../..

