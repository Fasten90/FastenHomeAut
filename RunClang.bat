:: Compile - per file - okay
::clang Src/* Src/Common/* Src/Common/Helper/* Src/Common/Handler/* Src/Communication/* Src/HwTester/* Src/Modules/* Src/Application/* Src/List/* Src/Display/* Src/SelfTest/* Drivers/Windows/Src/* -IDrivers/Windows/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Application -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v


:: Compile - out.exe - Linker error
::clang Src/* Src/Common/* Src/Common/Helper/* Src/Common/Handler/* Src/Communication/* Src/HwTester/* Src/Modules/* Src/Application/* Src/List/* Src/Display/* Src/SelfTest/* Drivers/Windows/Src/* -IDrivers/Windows/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Application -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -v -o Out/ClangCompiled.exe


:: Analyze - okay
clang --analyze --analyzer-output html Src/* Src/Common/* Src/Common/Helper/* Src/Common/Handler/* Src/Communication/* Src/HwTester/* Src/Modules/* Src/Application/* Src/List/* Src/Display/* Src/SelfTest/* Drivers/Windows/Src/* -IDrivers/Windows/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Application -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v


:: Analyze - error with "only one file"
::clang --analyze --analyzer-output html /VG/Projects/AtollicWorkspace/FastenHomeAut/Src/* /VG/Projects/AtollicWorkspace/FastenHomeAut/Src/Common/* /VG/Projects/AtollicWorkspace/FastenHomeAut/Src/Common/Helper/* /VG/Projects/AtollicWorkspace/FastenHomeAut/Src/Common/Handler/* /VG/Projects/AtollicWorkspace/FastenHomeAut/Src/Communication/* /VG/Projects/AtollicWorkspace/FastenHomeAut/Src/HwTester/* /VG/Projects/AtollicWorkspace/FastenHomeAut/Src/Modules/* /VG/Projects/AtollicWorkspace/FastenHomeAut/Src/Application/* /VG/Projects/AtollicWorkspace/FastenHomeAut/Src/List/* /VG/Projects/AtollicWorkspace/FastenHomeAut/Src/Display/* /VG/Projects/AtollicWorkspace/FastenHomeAut/Src/SelfTest/*  /VG/Projects/AtollicWorkspace/FastenHomeAut/Drivers/Windows/Src/* -IDrivers/Windows/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Application -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v -o Out
:: -o Out - only in one file?


:: Check - unable to handle compilation
::clang-check --analyze Src/* Src/Common/* Src/Common/Helper/* Src/Common/Handler/* Src/Communication/* Src/HwTester/* Src/Modules/* Src/Application/* Src/List/* Src/Display/* Src/SelfTest/* Drivers/Windows/Src/*

::clang-check Src/main.c -p Out/ClangOut


:: Clang parameters
:: -v ~verbose
:: -S Only preprocess+compile
:: ?? -o Out/ClangCompiled.exe


:: TODO:
:: c: C99
:: cpp: clang++ -std=c++14
