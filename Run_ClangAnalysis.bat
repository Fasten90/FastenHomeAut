echo Run - Clang - Analysis

:: Analyze - okay
::clang --analyze --analyzer-output html Src/* Src/Common/* Src/Common/Helper/* Src/Common/Handler/* Src/Communication/* Src/HwTester/* Src/Modules/* Src/Application/* Src/List/* Src/Display/* Src/SelfTest/* Drivers/x86/Src/* -IDrivers/x86/Inc -IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Application -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest -DCONFIG_USE_PANEL_PC=1 -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v

mkdir Out
cd Out

rmdir /S /Q ClangStaticAnalysisOutput
mkdir ClangStaticAnalysisOutput
cd ClangStaticAnalysisOutput


:: TODO: Delete "-save-stats" ? Did nothing
clang --analyze -Xanalyzer -analyzer-output=html -v -save-stats ^
../../Src/* ../../Src/Common/* ../../Src/Common/Helper/* ../../Src/Common/Handler/* ^
../../Src/Communication/* ../../Src/HwTester/* ../../Src/Modules/* ../../Src/Application/* ^
../../Src/List/* ../../Src/Display/* ../../Src/SelfTest/* ../../Drivers/x86/Src/* ^
-I../../Drivers/x86/Inc -I../../Inc/Common -I../../Inc/Communication -I../../Inc/HwTester ^
-I../../Inc/Modules -I../../Inc -I../../Inc/Application -I../../Inc/List -I../../Inc/Display ^
-I../../Inc/Common/Helper -I../../Inc/Common/Handler -I../../Inc/SelfTest ^
-DCONFIG_USE_PANEL_PC=1 -std=c99 -target x86_64-w64-mingw64 -Wall

:: -v verbose
:: -V View in webbrowswer when finished
:: -o Out/ClangStaticAnalysisOutput  - works at only one file
:: -stats  - does not works
::  -html-title=FastenHomeAut_ClangAnalysis -V 

:: Ideas:
:: -working-directory <value> --> for avoid ../...


:: List directories
dir


cd ../..
