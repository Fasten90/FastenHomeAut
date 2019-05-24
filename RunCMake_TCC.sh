echo Run CMake - TinyCC - TCC


# Install TCC
sudo apt-get install tcc

# TCC version
tcc -version


mkdir Out
cd Out
mkdir CMakeBuild_Tcc
cd CMakeBuild_Tcc


# Run CMake
cmake CC=tcc ../..


# Run make
echo Execute make
make


# Start compiled exe
./FastenHomeAut


# Go to original directory
cd ../..

