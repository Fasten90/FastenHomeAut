echo Run Clang - Formatter

:: Installation tools
::apt-get update
::apt-get install -y clang-6.0 lldb-6.0 lld-6.0

:: Go to Out/CMakeBuild_Clang
mkdir Out
cd Out
mkdir ClangFormatter
cd ClangFormatter


:: Run Clang formatter
:: clang -fsyntax-only ~/t.c
:: clang-format
:: -output-replacements-xml  - Output replacements as XML.
clang -fsyntax-only ^
    ../../Src/* ../../Src/Common/* ../../Src/Common/Helper/* ../../Src/Common/Handler/* ^
    ../../Src/Communication/* ../../Src/HwTester/* ../../Src/Modules/* ../../Src/Application/* ^
    ../../Src/List/* ../../Src/Display/* ../../Src/SelfTest/* ../../Drivers/x86/Src/* ^
    -I../../Drivers/x86/Inc -I../../Inc/Common -I../../Inc/Communication -I../../Inc/HwTester ^
    -I../../Inc/Modules -I../../Inc -I../../Inc/Application -I../../Inc/List -I../../Inc/Display ^
    -I../../Inc/Common/Helper -I../../Inc/Common/Handler -I../../Inc/SelfTest ^
    -DCONFIG_USE_PANEL_PC=1 -std=c11 -target x86_64-pc-windows-msvc -Wall
:: Linux: x86_64-pc-linux-gnu

:: List directories
dir


:: Go to original directory
cd ../..
