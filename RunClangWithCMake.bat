echo Run Clang CMake - MinGW64


:: Go to Out/ClangCMakeBuild
mkdir Out
cd Out
mkdir ClangCMakeBuild
cd ClangCMakeBuild


:: Run CMake
:: Clang need "Unix Makefiles" generator
cmake ^
-D"CMAKE_MAKE_PROGRAM:FILEPATH=c:/Programs/Engineer/Utils/CompilerTools/make.exe" ^
-D"CMAKE_C_COMPILER:FILEPATH=C:/Programs/Engineer/LLVM/bin/clang.exe" ^
CC=clang-cl -G "Unix Makefiles" ../..

:: Deleted, because not necessary: -D"CMAKE_CXX_COMPILER:FILEPATH=c:/Programs/Engineer/mingw64/bin/g++.exe" ^


:: Run make
:: c:\Programs\Engineer\Utils\CompilerTools\make.exe
c:\Programs\Engineer\Utils\CompilerTools\make.exe


:: Start compiled exe
FastenHomeAut.exe


:: Go to original directory
cd ..
cd ..
