echo Run CMake - MinGW64

# Go to Out/CMakeBuild_Clang
mkdir Out
cd Out
mkdir CMakeBuild_Clang
cd CMakeBuild_Clang


export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

# Run CMake
# cmake CC=gcc ..
cmake ../..


# Run make
make


# Start compiled exe
./FastenHomeAut


:: Go to original directory
cd ../..
