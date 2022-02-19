@echo off
echo Run CMake - GCC -Coverage


mkdir Out
cd Out
mkdir CMakeBuild_GccCoverage
cd CMakeBuild_GccCoverage


:: Run CMake
:: cmake CC=gcc ..
echo Execute cmake
cmake CC=gcc -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND" -DCMAKE_FASTENHOMEAUT_GCC_COVERAGE="TRUE" ../..


:: Run make
echo Execute make
make -j4


:: Start compiled exe
FastenHomeAut.exe


echo Coverage


:: Clone GccCoverageParser
:: PIPELINE_WORKSPACE - Define available at Pipeline (e.g. Azure)
if defined PIPELINE_WORKSPACE (
  echo PIPELINE_WORKSPACE is defined, download GccCoverageParser
  
  :: Go to original directory
  cd ../..

  mkdir Tools
  cd Tools
 
  git clone https://github.com/Fasten90/GccCoverageParser.git
  
  cd GccCoverageParser
  git checkout fix_syntax_issue
  cd ..
  
  cd ..

  cd Out
  cd CMakeBuild_GccCoverage
)


echo Run GccCoverageParser

cd

python ../../Tools/GccCoverageParser/Run_GccCoverage_ForSourceFiles.py --source-root-dir ../.. --gcno-files-root CMakeFiles/FastenHomeAut.dir --gcov-files-root . --export-file-path GccCoverage.txt


:: Go to original directory
cd ../..

