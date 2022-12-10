echo Run CMake - MinGW64

# Go to Out/CMakeBuild
mkdir Out
cd Out
mkdir CMakeBuildMinGW64
cd CMakeBuildMinGW64


# Run CMake
# cmake CC=gcc ..
cmake ../..


if [[ -z "${PIPELINE_WORKSPACE}" ]] && [[ -z "${CI}" ]] ; then
  # Local
  echo Run on local
  make
else
  # Pipeline
  echo Run on pipeline
  make > GCC_build.log 2>&1
  cat GCC_build.log
fi


file="./FastenHomeAut"
if [ -f "$file" ]
then
    echo "$0: File '${file}' has found."
else
    echo "$0: File '${file}' not found."
    echo 'Finished...'
    exit -1
fi


# Start compiled exe
./FastenHomeAut


# Go to original directory
cd ../..
