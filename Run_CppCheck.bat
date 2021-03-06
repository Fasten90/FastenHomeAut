@echo off
echo Run CppCheck

:: cppcheck shall be added to the PATH

:: TODO: Avoid the duplicated parts (Includes)

mkdir Out


:: C99
echo CppCheck - C99

cppcheck Src Inc Drivers ^
-IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Application -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest ^
-IDrivers/x86/Inc ^
-IDrivers/STM32F0xx_HAL_Driver/Inc -IDrivers/STM32F0xx_HAL_Driver/Inc/Legacy -IDrivers/CMSIS/Device/ST/STM32F0xx/Include ^
-IDrivers/STM32F4xx_HAL_Driver/Inc -IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy -IDrivers/CMSIS/Device/ST/STM32F4xx/Include ^
-IDrivers/CMSIS/Include ^
-iDrivers/CMSIS -iDrivers/STM32F0xx_HAL_Driver -iDrivers/STM32F4xx_HAL_Driver ^
--enable=all --inconclusive --suppressions-list=cppcheck_suppressions.txt --template="[{file}:{line}]:\t({severity})\t{message}" --inline-suppr --std=c99 --force --check-config 2> Out/Cppcheck_ErrorsC99.txt


:: C11
echo CppCheck - C11

cppcheck Src Inc Drivers ^
-IInc/Common -IInc/Communication -IInc/HwTester -IInc/Modules -IInc -IInc/Application -IInc/List -IInc/Display -IInc/Common/Helper -IInc/Common/Handler -IInc/SelfTest ^
-IDrivers/x86/Inc ^
-IDrivers/STM32F0xx_HAL_Driver/Inc -IDrivers/STM32F0xx_HAL_Driver/Inc/Legacy -IDrivers/CMSIS/Device/ST/STM32F0xx/Include ^
-IDrivers/STM32F4xx_HAL_Driver/Inc -IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy -IDrivers/CMSIS/Device/ST/STM32F4xx/Include ^
-IDrivers/CMSIS/Include ^
-iDrivers/CMSIS -iDrivers/STM32F0xx_HAL_Driver -iDrivers/STM32F4xx_HAL_Driver ^
--enable=all --inconclusive --suppressions-list=cppcheck_suppressions.txt --template="[{file}:{line}]:\t({severity})\t{message}" --inline-suppr --std=c11 --force --check-config 2> Out/Cppcheck_ErrorsC11.txt


:: Open Error files with Notepad++
start notepad++ "Out/Cppcheck_ErrorsC99.txt"
start notepad++ "Out/Cppcheck_ErrorsC11.txt"
