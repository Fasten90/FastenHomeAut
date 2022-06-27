@echo off
echo Run CMake - TCC - TinyCC


:: Go to Out/CMakeBuild_Tcc
mkdir Out
cd Out
rmdir /s /q CMakeBuild_Tcc
mkdir CMakeBuild_Tcc
cd CMakeBuild_Tcc


:: PreTest:
::make
::tcc


:: Run CMake
cmake ^
-D"CMAKE_MAKE_PROGRAM:FILEPATH=make.exe" ^
-D"CMAKE_C_COMPILER:FILEPATH=tcc.exe" ^
CC=gcc -G "MinGW Makefiles" ../..


:: Run make
make VERBOSE=1


:: Start compiled exe
FastenHomeAut.exe


:: Go to original directory
cd ../..
