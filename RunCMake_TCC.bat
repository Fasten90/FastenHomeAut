echo Run CMake - TCC - TinyCC


:: Go to Out/CMakeBuild_Tcc
mkdir Out
cd Out
mkdir CMakeBuild_Tcc
cd CMakeBuild_Tcc


:: Run CMake
cmake ^
-D"CMAKE_MAKE_PROGRAM:FILEPATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" ^
-D"CMAKE_C_COMPILER:FILEPATH=c:/Programs/Engineer/tcc/tcc.exe" ^
CC=gcc -G "MinGW Makefiles" ../..


:: Run make
:: c:\Programs\Engineer\Utils\CompilerTools\make.exe
c:\Programs\Engineer\Utils\CompilerTools\make.exe VERBOSE=1


:: Start compiled exe
FastenHomeAut.exe


:: Go to original directory
cd ../..
