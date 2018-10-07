echo Run Cppcheck

cppcheck Src Inc Drivers -iDrivers/CMSIS -iDrivers/STM32F0xx_HAL_Driver -iDrivers/STM32F4xx_HAL_Driver --enable=all --inconclusive --suppressions-list=cppcheck_suppressions.txt --template="[{file}:{line}]:\t({severity})\t{message}" --inline-suppr --std=c99 --force 2> Out/CppcheckErrorsC99.txt

cppcheck Src Inc Drivers -iDrivers/CMSIS -iDrivers/STM32F0xx_HAL_Driver -iDrivers/STM32F4xx_HAL_Driver --enable=all --inconclusive --suppressions-list=cppcheck_suppressions.txt --template="[{file}:{line}]:\t({severity})\t{message}" --inline-suppr --std=c11 --force 2> Out/CppcheckErrorsC11.txt

:: Open Error file with Notepad++
start notepad++ "Out/CppcheckErrorsC99.txt"

:: Open Error file with Notepad++
start notepad++ "Out/CppcheckErrorsC11.txt"
