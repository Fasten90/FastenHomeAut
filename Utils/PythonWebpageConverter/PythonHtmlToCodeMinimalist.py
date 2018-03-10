"""
Original HTML file to minimalist HTML
@author	Vizi Gabor
"""


def convertOriginalHtmlfileToMinimalistHtmlFile(originalhtmlfilename, newhtmlfilename=None):

    print("Original HTML file name: " + originalhtmlfilename)

    # Original HTML file
    originalfile = open(originalhtmlfilename, 'rt')
    data = originalfile.read()
    originalfile.close()

    newfilecontainstring = convertHtmlToMinimalist(data)

    if newhtmlfilename != None:
        print("New HTML file name: " + newhtmlfilename)
        newfile = open(newhtmlfilename + ".c", 'wt')
        newfile.write(newfilecontainstring)
        newfile.close()
    else:
        return newfilecontainstring


def convertHtmlToMinimalist(originalhtml):

    newstring = ""

    originalhtmllength = len(originalhtml)
    print("Original html file character num: " + str(originalhtmllength))
    
    newhtmllength = 0

    splittedstring = originalhtml.split("\n")
    
    for line in splittedstring:
        # Trim (remove all started and ended whitespace)
        line = line.strip()
        newhtmllength += len(line)
        # Escape "
        line = line.replace("\"", "\\\"")
        # For C string (every line should show like: "string" \
        line = "\"" + line + "\" \\\n"
        newstring += line

    newstring += ";\n"
    newstring += "/* End of HTML file string */\n"
    
    print("New html file character num: " + str(newhtmllength))

    return newstring

