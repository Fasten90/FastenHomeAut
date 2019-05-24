echo Run CMake - Clang

# Installation tools
#apt-get update
#apt-get install -y clang-6.0 lldb-6.0 lld-6.0

# Go to Out/CMakeBuild_Clang
mkdir Out
cd Out
mkdir CMakeBuild_Clang
cd CMakeBuild_Clang


export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

# Run CMake
# cmake CC=gcc ..
echo Execute cmake
cmake ../..


# Run make
echo Execute make
make


# Start compiled exe
./FastenHomeAut


:: Go to original directory
cd ../..
