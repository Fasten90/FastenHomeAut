import glob
#from pathlib import Path

import subprocess

import os

# Note: Similar command shall be called all gcno file
# gcov --all-blocks --function-summaries --branch-probabilities --branch-counts --unconditional-branches CMakeFiles/FastenHomeAut.dir/Src/main.c.gcno

__COMMAND = "gcov"
__COMMAND_ARG = "--all-blocks --function-summaries --branch-probabilities --branch-counts --unconditional-branches"
__gcov_file_dir = "CMakeFiles/FastenHomeAut.dir/"


source_list = []


# PATH correction

# os.path.normpath()
cwd = os.path.normcase(os.path.normpath(os.getcwd()))
script_dir = os.path.normcase(os.path.normpath(os.path.dirname(__file__)))
cwd_is_changed = False

if cwd != script_dir:
    #os.chdir("../..")
    cwd_is_changed = True
    os.chdir(script_dir)
    print("Working directory changed from: '{}' to '{}'".format(cwd, script_dir))

# Source list
source_list += glob.glob("Src/*.c")
source_list += glob.glob("Src/**/*.c")
source_list += glob.glob("Src/**/**/*.c")
#source_list += Path('Src').glob('**/*.c')


# TODO: Shall we append another?
print("Source list:")
prev_dir = ""
for src_item in source_list:
    src_item = src_item.replace("\\\\", "\\").replace("/", "\\")
    
    [dir, name] = src_item.rsplit("\\", 1)
    if dir != prev_dir:
        prev_dir = dir
        str_indent = "  " * src_item.count("\\")
        print(str_indent + "[" + dir + "]")
    
    str_indent = "  " * (src_item.count("\\") + 1)
    print(str_indent + "- " + name)



# For PATH correction
if cwd_is_changed:
    os.chdir(cwd)
    print("Working directory restored to: '{}'".format(cwd))
else:
    # TODO: Local mode?
    os.chdir("Out/CMakeBuild_GccCoverage/")
    print("Actual working directory: '{}'".format(os.getcwd()))


for source in source_list:
    # Call for all source file
    #print(source)
    # TODO: Argument
    source = source.replace("\\", "/")
    gcno_file_path = __gcov_file_dir + source + ".gcno"
    
    #print("file: '{}'".format(gcno_file_path))
    if os.path.exists(gcno_file_path):
        # Call
        command = __COMMAND + " " + __COMMAND_ARG + " " + gcno_file_path
        print("Command: {}".format(command))
        #subprocess.run(["ls", "-l"])
        #subprocess.run([__COMMAND, command_arg])
        return_code = subprocess.call(command, shell=True)
        # Debug code
        #print("  Return code: {}".format(return_code))
    else:
        # Do not call
        print("'{}' has no gcno file".format(source))


# from pathlib import Path
    