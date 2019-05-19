import glob
#from pathlib import Path

import subprocess

import os

# gcov --all-blocks --function-summaries --branch-probabilities --branch-counts --unconditional-branches CMakeFiles/FastenHomeAut.dir/Src/main.c.gcno

__COMMAND = "gcov"
#__COMMAND_ARG = "--all-blocks --function-summaries --branch-probabilities --branch-counts --unconditional-branches CMakeFiles/FastenHomeAut.dir/"
__COMMAND_ARG = "CMakeFiles/FastenHomeAut.dir/"


source_list = []

# PATH correction
os.chdir("../..")

source_list += glob.glob("Src/*.c")
source_list += glob.glob("Src/**/*.c")
source_list += glob.glob("Src/**/**/*.c")
#source_list += Path('Src').glob('**/*.c')

# TODO: Shall we append another?
print("Source list: {}".format(source_list))


# For PATH correction
os.chdir("Out/CMakeBuild_GccCoverage")


for source in source_list:
    # Call for all source file
    print(source)
    source = source.replace("\\", "/")
    command_arg = __COMMAND_ARG + source + ".gcno"
    print("Command: {}".format(command_arg))
    #subprocess.run(["ls", "-l"])
    #subprocess.run([__COMMAND, command_arg])
    #return_code = subprocess.call("echo Hello World", shell=True)  
    return_code = subprocess.call(__COMMAND + " " + command_arg, shell=True)  

# from pathlib import Path
    