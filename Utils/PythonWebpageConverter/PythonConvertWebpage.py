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
        webpagelement["name"] = dir_item.split(".", 1)[0]
        filepath = webpagedirectory + "/" + dir_item

        if dir_item.endswith(".html"):
            # Convert this HTML file
            #htmlfile = open(dir_item, 'rt')
            print("HTML file name found for convert: " + dir_item)
            webpagelement["contain"] = PythonHtmlToCodeMinimalist.convertOriginalHtmlfileToMinimalistHtmlFile(filepath)
            convertedfiles.append(webpagelement)
        elif dir_item.endswith(".ico"):
            # Convert this ico file (binary9
            print("Binary file name found for convert: " + dir_item)
            webpagelement["contain"] = PythonBinaryToCodeHex.convertBinaryFileToHexFile(filepath)
            convertedfiles.append(webpagelement)
            

    # convertedfiles contain important things

    # Write html strings to code file
    newfilepath = destination + "/" + webpage_c_filename

    newfilecontainstring = writeDatasToFile(convertedfiles, newfilepath, startfilepath, endfilepath)


def writeDatasToFile(convertedfiles, destfilepath, startfilepath, endfilepath):
    
    filecontain = ""
    
    # TODO: Start elements:
    
    for convertelement in convertedfiles:
        # write "///< <name> file:"
        filecontain += "///< "
        filecontain += convertelement["name"]
        filecontain += " file:\n"
        
        # write: "const char WebpageList_<name>[] =
        filecontain += "const char WebpageList_"
        filecontain += convertelement["name"]
        filecontain += "[] =\n"
        
        # write: element (string or hexs)
        filecontain += convertelement["contain"]
        
        filecontain += "\n\n"
        
        # write length variable
        # write: const size_t WebpageList_<name>_length = sizeof(WebpageList_<name>);
        filecontain += "const size_t WebpageList_"
        filecontain += convertelement["name"]
        filecontain += "_length = sizeof(WebpageList_"
        filecontain += convertelement["name"]
        filecontain += ");\n"
        
        filecontain += "\n\n"
        
    # TODO: Add end elements
    
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
