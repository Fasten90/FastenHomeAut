echo Run CMake - MinGW64


:: Go to Out/CMakeBuild
mkdir Out
cd Out
mkdir CMakeBuildMinGW64
cd CMakeBuildMinGW64


:: Run CMake
::cmake CC=gcc ..
cmake ../..


:: Run make
:: c:\Programs\Engineer\Utils\CompilerTools\make.exe
c:\Programs\Engineer\Utils\CompilerTools\make.exe VERBOSE=1


:: Start compiled exe
:: FastenHomeAut.exe


:: Go to original directory
cd ..
cd ..
