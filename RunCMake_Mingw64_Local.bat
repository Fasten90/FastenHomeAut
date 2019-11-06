echo Run CMake - MinGW64


:: Go to Out/CMakeBuild
mkdir Out
cd Out
mkdir CMakeBuildMinGW64
cd CMakeBuildMinGW64


:: Run CMake
cmake ^
-D"CMAKE_MAKE_PROGRAM:FILEPATHmake.exe" ^
-D"CMAKE_C_COMPILER:FILEPATH=gcc.exe" ^
-D"CMAKE_CXX_COMPILER:FILEPATH=g++.exe" ^
CC=gcc -G "MinGW Makefiles" ../..


:: Run make
make VERBOSE=1


:: Start compiled exe
FastenHomeAut.exe


:: Go to original directory
cd ..
cd ..
