"""
Original HTML file to minimalist HTML
@author	Vizi Gabor
"""


def convertOriginalHtmlfileToMinimalistHtmlFile(originalhtmlfilename, newhtmlfilename):

    print("Original HTML file name: " + originalhtmlfilename)
    print("Code file name: " + newhtmlfilename)


    originalfile = open(originalhtmlfilename, 'rt')
    newfile = open(newhtmlfilename + ".c", 'wt')

    data = originalfile.read()

    originalfile.close()

    newfilecontainstring = convertHtmlToMinimalist(data)

    newfile.write(newfilecontainstring)
    newfile.close()


def convertHtmlToMinimalist(originalhtml):

    newstring = ""
    newstring += "\n"

    originalhtmllength = len(originalhtml)
    print("Original html file character num: " + str(originalhtmllength))

    splittedstring = originalhtml.split("\n")
    
    for line in splittedstring:
        # Trim (remove all started and ended whitespace)
        line = line.strip()
        # Escape "
        line = line.replace("\"", "\\\"")
        # For C string (every line should show like: "string" \
        line = "\"" + line + "\" \\\n"
        newstring += line

    newstring += ";\n"
    newstring += "/* End of HTML file string */"

    return newstring

