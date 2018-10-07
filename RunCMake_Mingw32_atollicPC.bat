echo Run CMake - MinGW32


:: Go to Out/CMakeBuild
mkdir Out
cd Out
mkdir CMakeBuildMinGW32
cd CMakeBuildMinGW32


:: Run CMake
cmake ^
-D"CMAKE_MAKE_PROGRAM:FILEPATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" ^
-D"CMAKE_C_COMPILER:FILEPATH=c:/Programs/Engineer/TrueSTUDIO for STM32 9.0.1/PCTools/bin/gcc.exe" ^
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
