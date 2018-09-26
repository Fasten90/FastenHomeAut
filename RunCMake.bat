echo Run CMake


:: Go to Out/CMakeBuild
mkdir Out
cd Out
mkdir CMakeBuild
cd CMakeBuild


:: Run CMake
cmake ^
-D"CMAKE_MAKE_PROGRAM:FILEPATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" ^
-D"CMAKE_C_COMPILER:FILEPATH=c:/Programs/Engineer/mingw64/bin/gcc.exe" ^
-D"CMAKE_CXX_COMPILER:FILEPATH=c:/Programs/Engineer/mingw64/bin/g++.exe" ^
CC=gcc -G "MinGW Makefiles" ../..


:: Run make
:: c:\Programs\Engineer\Utils\CompilerTools\make.exe
c:\Programs\Engineer\Utils\CompilerTools\make.exe VERBOSE=1


:: Start compiled exe
FastenHomeAut.exe


:: Go to original directory
cd ..
cd ..
