echo Run CMake - TCC


:: Go to Out/CMakeBuildTcc
mkdir Out
cd Out
mkdir CMakeBuildTcc
cd CMakeBuildTcc


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
cd ..
cd ..
