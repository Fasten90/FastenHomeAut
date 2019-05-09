echo Run CMake - MinGW64

# Go to Out/CMakeBuild
mkdir Out
cd Out
mkdir CMakeBuildMinGW64
cd CMakeBuildMinGW64


# Run CMake
# cmake CC=gcc ..
cmake ../..


# Run make
make


# Start compiled exe
./FastenHomeAut


:: Go to original directory
cd ../..
