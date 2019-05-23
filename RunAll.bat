::RunAllCompilers

CALL RunCMake_MinGW_GCC.bat
::pause


CALL RunCMake_gcc-arm-atollic-eabi.bat


CALL Run_ClangAnalysis.bat


CALL Run_ClangFormatter.bat


CALL RunCMake_GccCoverage.bat


CALL Run_CppCheck.bat


CALL RunCmake_TCC.bat

