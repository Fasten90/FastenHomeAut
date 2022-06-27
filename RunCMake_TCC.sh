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

# PIPELINE_WORKSPACE - Define available at Pipeline (e.g. Azure)
if [[ -z "${PIPELINE_WORKSPACE}" ]]; then
  # Local
  echo Run on local
  make
else
  # Pipeline
  echo Run on pipeline
  make > TinyCC_build.log 2>&1
  cat TinyCC_build.log
fi


# Start compiled exe
./FastenHomeAut


# Go to original directory
cd ../..

