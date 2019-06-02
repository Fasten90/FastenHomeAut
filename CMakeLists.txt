#cmake_minimum_required (VERSION 2.6)
cmake_minimum_required (VERSION 3.0)
project (FastenHomeAut)

# reported in Azure pipeline - Windows - MSVC?
cmake_policy(SET CMP0054 NEW) 


# C parameters
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")


# Compiler type
message(STATUS "# Compiler ID: ${CMAKE_C_COMPILER_ID}")


# CI vs Local build
# Includes, compilers
# "CI" used in Bitbucket pipeline
if(DEFINED ENV{CI})
    # CI build
    message(STATUS "# CI environment variable defined")
else()
    # LOCAL build
    message(STATUS "# CI environment variable has not defined")
endif()


# Include list
# ".." is necessary because make called from <project>\build directory
# Now used: "../.." because <project>/Out/<MakeBuildDirectory>

# FastenHomeAut includes
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I../../Inc/Common -I../../Inc/Communication -I../../Inc/HwTester -I../../Inc/Modules -I../../Inc -I../../Inc/Application -I../../Inc/List -I../../Inc/Display -I../../Inc/Common/Helper -I../../Inc/Common/Handler -I../../Inc/SelfTest")

# Target dependent: Library includes, Defines, target
# STM32F0_arm-none-eabi-gcc & STM32F0_arm-atollic-eabi-gcc
if ("${CMAKE_FASTENHOMEAUT_TARGET}" MATCHES "STM32F0_arm-.*")
    # STM32F0
    message(STATUS "# Selected target: STM32F0")
    
    # TODO: Copied 2 line not important, there is in toolchain too
    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_SYSTEM_PROCESSOR arm)

    if ("${CMAKE_FASTENHOMEAUT_TARGET}" STREQUAL "STM32F0_arm-none-eabi-gcc")
        set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
        set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
    elseif ("${CMAKE_FASTENHOMEAUT_TARGET}" STREQUAL "STM32F0_arm-atollic-eabi-gcc")
        if (NOT "${CMAKE_C_COMPILER}" MATCHES ".*arm-atollic-eabi-gcc.*")
            set(CMAKE_C_COMPILER "arm-atollic-eabi-gcc")
            set(CMAKE_CXX_COMPILER "arm-atollic-eabi-g++")
        endif()
    endif()

    # TODO: Copied 5 line not important, there is in toolchain too
    set(CMAKE_EXE_LINKER_FLAGS "--specs=nosys.specs" CACHE INTERNAL "")

    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

    string(CONCAT TARGET_COMPILER_OPTIONS
                                " -mthumb"
                                " -mcpu=cortex-m0"
                                " -std=gnu11"
                                " -O0"
                                " -ffunction-sections"
                                " -fdata-sections"
                                " -fno-strict-aliasing"
                                " -g"
                                " -fstack-usage"
                                " -Wall"
                                " -Wextra"
                                " -pedantic"
                                " -Wmissing-include-dirs"
                                " -Wswitch-default"
                                " -Wswitch-enum"
                                )

    string(CONCAT TARGET_INCLUDES
                        " -I../../Drivers/STM32F0xx_HAL_Driver/Inc"
                        " -I../../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy"
                        " -I../../Drivers/CMSIS/Device/ST/STM32F0xx/Include"
                        " -I../../Drivers/CMSIS/Include"
                        )

    # " -D__weak=__attribute__((weak))"
    # " -D__packed=__attribute__((__packed__))"
    string(CONCAT TARGET_DEFINES
                       " -DUSE_HAL_DRIVER"
                       " -DCONFIG_USE_PANEL_FASTENNODE=1"
                       " -DSTM32F030x8"
                       )
   
    if ("${CMAKE_FASTENHOMEAUT_TARGET}" STREQUAL "STM32F0_arm-none-eabi-gcc")
        string(APPEND TARGET_DEFINES " -DCONFIG_STANDARD_LIBRARY_USE_DEFAULT")
    endif()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_COMPILER_OPTIONS} ${TARGET_INCLUDES} ${TARGET_DEFINES}")
    
    # TODO: Make parameter for // TODO: Make parameter for FastenNodeF0\Debug_STM32F030C8_FLASH.ld
    #' -T..\FastenNodeF0\Debug_STM32F030C8_FLASH.ld'   
    # " -specs=nano.specs" was added, but used at Atollic
    string(CONCAT TARGET_LINKER_OPTIONS
                              " -mthumb"
                              " -mcpu=cortex-m0"
                              " -Wl,--start-group"
                              " -lc"
                              " -lm"
                              " -Wl,--end-group"
                              " -static"
                              " -Wl,-Map=FastenHomeAut.map"
                              " -Wl,--gc-sections"
                              " -Wl,--defsym=malloc_getpagesize_P=0x80"
                              )

    # TODO: Delete this
    if ("${CMAKE_FASTENHOMEAUT_TARGET}" STREQUAL "STM32F0_arm-none-eabi-gcc")
        #string(APPEND TARGET_LINKER_OPTIONS " -nodefaultlibs")
    elseif ("${CMAKE_FASTENHOMEAUT_TARGET}" STREQUAL "STM32F0_arm-atollic-eabi-gcc")
        #string(CONCAT TARGET_LINKER_OPTIONS " -T../../../FastenNodeF0/Debug_STM32F030C8_FLASH.ld")
    endif()

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${TARGET_LINKER_OPTIONS}")
else()
    # Not defined STM32F0 --> So target: x86
    message(STATUS "# Selected target: x86")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I../../Drivers/x86/Inc -DCONFIG_USE_PANEL_PC=1")
endif()


# Compiler type
message(STATUS "Compiler ID: ${CMAKE_C_COMPILER_ID}")

# Set compiler dependent settings
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    # Clang
    message(STATUS "# Compiler: Clang")

    # Clang error suppression
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__clang__=1")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v")
    # Clang: -S --> preprocessor only
    
    if (WIN32)
        message(STATUS "# Clang platform: Win")
        #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -target x86_64-pc-win32 -Wall -v")
        #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -target x86_64-pc-windows-gnu -Wall -v")    
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -target x86_64-pc-windows-msvc -Wall -v")    
    elseif (UNIX)
        message(STATUS "# Clang platform: Linux")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -target x86_64-pc-linux-gnu -Wall -v")    
    else()
        message(STATUS "# Unknown Clang platform")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -target x86_64-pc-linux-gnu -Wall -v")        
    endif ()

    # clang++ -target x86_64-pc-windows-gnu -std=c++14 test.cc -o test.exe
    # Possible targets:
    # x86_64-w64-mingw64
    # x86_64-pc-linux-gnu

elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    # GCC, MinGW32, MinGW64
    message(STATUS "# Compiler: GNU (GCC, MinGW*)")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c11 -D__clang__=1")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "TinyCC")
    # TCC / TinyCC
    message(STATUS "# Compiler TCC")

    # TCC include
    # TODO: Why not necessary?
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I/Programs/Engineer/tcc/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v -o Out/CmakeTccCompiled/")
    # Memory and Bound checks: -b # TODO: Not works yet
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -b")
    
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
    # MSVC - Microsoft Visual Studio
    message(STATUS "# Compiler: MSVC")
    
    # MSVC does not support the '-Wextra' and '-std=c11'
    # /Wall --> turn on all warnings
    
    # /wdnnnn --> turn off this check
    # /wonnnn 	Reports the compiler warning that is specified by nnnn only once.

    # C4668 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
    # /wd4668 --> turn off
    # C4820 (level 4) 	'bytes' bytes padding added after construct 'member_name'
    # /wd4820
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Wall /wd4668 /wd4820 /std:c++14")
else()
    message(STATUS "# !! Cmake Error possibility !! Not known Compiler, please set it!")
endif()


# Set target dependent settings
if (UNIX)
    #Linux need pthread for link
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
endif (UNIX)


# E.g.
#file(GLOB_RECURSE Foo_SOURCES "src/*.cpp")
#add_executable (FooExe ${Foo_SOURCES})


# Sources
#Src/*.c Src/Common/*.c Src/Common/Helper/*.c Src/Common/Handler/*.c Src/Communication/*.c Src/HwTester/*.c Src/Modules/*.c Src/Application/*.c Src/List/*.c Src/Display/*.c Src/SelfTest/*.c Drivers/x86/Src/*.c
file(GLOB_RECURSE FASTEN_SOURCE_FILES "Src/*.c")

# Target dependent sources
if ("${CMAKE_FASTENHOMEAUT_TARGET}" MATCHES "STM32F0.*")
    file(GLOB_RECURSE DRIVER_SOURCE_FILES_1 "Drivers/STM32F0xx_HAL_Driver/*.c")
    file(GLOB_RECURSE DRIVER_SOURCE_FILES_2 "Drivers/CMSIS/Device/ST/STM32F0xx/Source/*.c")
    set(DRIVER_SOURCE_FILES ${DRIVER_SOURCE_FILES_1} ${DRIVER_SOURCE_FILES_2})
else()
    file(GLOB_RECURSE DRIVER_SOURCE_FILES "Drivers/x86/Src/*.c")
endif()


set(SOURCE_FILES ${FASTEN_SOURCE_FILES} ${DRIVER_SOURCE_FILES})


# Test or normal output
if ("${CMAKE_FASTENHOMEAUT_GCC_COERAGE}" STREQUAL "TRUE")
    message(STATUS "# Test coverage turned on")    

    SET(GCC_COVERAGE_COMPILE_FLAGS "-g -O0 -coverage -fprofile-arcs -ftest-coverage")
    SET(GCC_COVERAGE_LINK_FLAGS    "-coverage -lgcov")
    SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY .)

    include_directories(include)
    
    # In examples:
    #set(CATCH_HEADER_PATH ${PROJECT_SOURCE_DIR}/test/catch.hpp)
    #add_executable(FastenHomeAut_Test ${PROJECT_SOURCE_DIR}/test/test.cpp ${CATCH_HEADER_PATH}) 

    add_executable(FastenHomeAut ${SOURCE_FILES}) 
    
else()
    # Normal output
    add_executable(FastenHomeAut ${SOURCE_FILES})
endif ()
