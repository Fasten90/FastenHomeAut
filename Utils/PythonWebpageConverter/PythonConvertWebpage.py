"""
Convert FastenHomeAut webpage
@author	Vizi Gabor
"""

import os

import PythonBinaryToCodeHex
import PythonHtmlToCodeMinimalist


def ConvertWebpage(webpagedirectory="Webpage", destination="Dest", webpage_c_filename="WebpageList.c", startfilepath="WebpageList_Template_start.c", endfilepath="WebpageList_Template_end.c"):
    """ Convert entire webpage directory """

    convertedfiles = []

    # Read directory of Webpage
    dirlist = os.listdir(webpagedirectory)
    
    print("Webpages directory list: " + str(dirlist))
    
    for dir_item in dirlist:
        webpagelement = {}
        webpagelement["fullname"] = dir_item
        webpagelement["shortname"] = dir_item.split(".", 1)[0]
        webpagelement["type"] = ""
        filepath = webpagedirectory + "/" + dir_item

        if dir_item.endswith(".html"):
            # Convert this HTML file
            #htmlfile = open(dir_item, 'rt')
            print("HTML file name found for convert: " + dir_item)
            webpagelement["contain"] = PythonHtmlToCodeMinimalist.convertOriginalHtmlfileToMinimalistHtmlFile(filepath)
            webpagelement["type"] = "text/html"
            convertedfiles.append(webpagelement)
        elif dir_item.endswith(".ico"):
            # Convert this ico file (binary9
            print("Binary file name found for convert: " + dir_item)
            webpagelement["contain"] = PythonBinaryToCodeHex.convertBinaryFileToHexFile(filepath)
            webpagelement["type"] = "image/x-icon"
            convertedfiles.append(webpagelement)
            

    # convertedfiles contain important things

    # Write html strings to code file
    newfilepath = destination + "/" + webpage_c_filename

    newfilecontainstring = writeDatasToFile(convertedfiles, newfilepath, startfilepath, endfilepath)


def writeDatasToFile(convertedfiles, destfilepath, startfilepath, endfilepath):

    filecontain = ""

    # Step on the file list and print
    for convertelement in convertedfiles:
        # write "///< <name> file:"
        filecontain += "///< "
        filecontain += convertelement["fullname"]
        filecontain += " file:\n"

        # write: "const char WebpageList_<name>[] =
        filecontain += "static const char WebpageList_"
        filecontain += convertelement["shortname"]
        filecontain += "[] =\n"

        # write: element (string or hexs)
        filecontain += convertelement["contain"]

        filecontain += "\n\n"

    # "WebpageInformation_t WebpageList[] = { ... }"
    filecontain += "///< Webpage List\n"
    filecontain += "const WebpageInformation_t WebpageList[] =\n"
    filecontain += "{\n"
    for convertelement in convertedfiles:
        # One webpage structure
        #const char * webpageName;
        #const char * webpageContain;
        #const size_t webpageLength;
        #const char * webpageType;
        filecontain += "    {\n"
        filecontain += "        .webpageName    = " + "\"" + convertelement["fullname"] + "\",\n"
        filecontain += "        .webpageContain = " + "WebpageList_" + convertelement["shortname"] + ",\n"
        filecontain += "        .webpageLength  = " + "sizeof(WebpageList_" + convertelement["shortname"] + "),\n"  
        filecontain += "        .webpageType    = " + "\"" + convertelement["type"]  + "\"\n"
        filecontain += "    },\n"

    # End of Information
    filecontain += "};\n"
    filecontain += "\n\n"


    # Webpage list count: NUM_OF(WebpageList)
    filecontain += "///< Webpage list count\n"
    filecontain += "const uint8_t WebpageList_Count = NUM_OF(WebpageList);\n"
    filecontain += "\n\n"
    
    
    # Index of 404 page
    # Step on the file list and search the name "404"
    page404index = 0
    i = 0
    for convertelement in convertedfiles:
        # write "///< <name> file:"
        if convertelement["shortname"] == "404":
            page404index = i
            break 
        i += 1

    filecontain += "///< Webpage 404 index (in the list)\n"
    filecontain += "const uint8_t WebpageList_404PageIndex = " + str(page404index) + ";\n"
    filecontain += "\n\n"


     # Read begin of destfile
    startfilepath = open(startfilepath, 'rt')
    startfilecontain = startfilepath.read()
    startfilepath.close() 

    # Read end of destfile
    endfile = open(endfilepath, 'rt')
    endfilecontain = endfile.read()
    endfile.close() 

    # Merge string
    filecontain = startfilecontain + filecontain + endfilecontain

    # Write new file
    destfile = open(destfilepath, 'wt')
    destfile.write(filecontain)
    destfile.close() 


if __name__ == "__main__":
    #ConvertWebpage(webpagedirectory = "../Webpage", destination = "../../src/List/", webpage_c_filename = "WebpageList.c")
    ConvertWebpage(webpagedirectory = "c:\\Engineer\\Projects\\AtollicWorkspace\\FastenHomeAut\\Utils/Webpage", \
                   destination = "c:\\Engineer\\Projects\\AtollicWorkspace\\FastenHomeAut/src/List/",
                   webpage_c_filename = "WebpageList.c",
                   startfilepath="c:\\Engineer\\Projects\\AtollicWorkspace\\FastenHomeAut\\Utils/PythonWebpageConverter/WebpageList_Template_start.c",
                   endfilepath="c:\\Engineer\\Projects\\AtollicWorkspace\\FastenHomeAut\\Utils/PythonWebpageConverter/WebpageList_Template_end.c")
