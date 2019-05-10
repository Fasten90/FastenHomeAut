#cmake_minimum_required (VERSION 2.6)
cmake_minimum_required (VERSION 3.0)
project (FastenHomeAut)

# reported in Azure pipeline - Windows - MSVC?
cmake_policy(SET CMP0054 NEW) 


# C parameters
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c11")


# Compiler type
message("Compiler ID: ${CMAKE_C_COMPILER_ID}")


# CI vs Local build
# Includes, compilers
if(DEFINED ENV{CI})
    # CI build
    message(STATUS "CI environment variable defined")
else()
    # LOCAL build
    message(STATUS "CI environment variable has not defined")
endif()


# Include list
# ".." is necessary because make called from <project>\build directory
# Now used: "../.." because <project>/Out/<MakeBuildDirectory>
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I../../Drivers/x86/Inc -I../../Inc/Common -I../../Inc/Communication -I../../Inc/HwTester -I../../Inc/Modules -I../../Inc -I../../Inc/Application -I../../Inc/List -I../../Inc/Display -I../../Inc/Common/Helper -I../../Inc/Common/Handler -I../../Inc/SelfTest -DCONFIG_USE_PANEL_PC=1")

# Compiler type
message("Compiler ID: ${CMAKE_C_COMPILER_ID}")

# Set compiler dependent settings
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    # Clang error suppression
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__clang__=1")
    # using Clang
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I/Programs/Engineer/LLVM/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v -o Out/ClangCmakeCompiled/")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    # using GCC, MinGW32, MinGW64
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__clang__=1")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "TinyCC")
    # using TCC
    message("Compiler TCC - TODO")
    # TCC include
    # TODO: Why not necessary?
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I/Programs/Engineer/tcc/include -std=c11 -target x86_64-w64-mingw64 -Wall -S -v -o Out/CmakeTccCompiled/")
    # Memory and Bound checks: -b # TODO: Not works yet
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -b")
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
file(GLOB_RECURSE DRIVER_SOURCE_FILES "Drivers/x86/Src/*.c")

set(SOURCE_FILES ${FASTEN_SOURCE_FILES} ${DRIVER_SOURCE_FILES})

add_executable(FastenHomeAut ${SOURCE_FILES})
