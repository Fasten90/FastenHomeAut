"""
Binary file --> Code Hex text
@author	Vizi Gabor
"""


def convertBinaryFileToHexFile(binaryfilename, codefilename=None):

    print("Binary file name: " + binaryfilename)

    # Original binary file
    binaryfile = open(binaryfilename, "rb")
    data = binaryfile.read()
    binaryfile.close()

    data = bytearray(data)
    codefilecontainstring = convertBinDatasToHex(data)

    if codefilename != None:
        print("Code file name: " + codefilename)
        codefile = open(codefilename + ".c", "wt")
        codefile.write(codefilecontainstring)
        codefile.close()
    else:
        return codefilecontainstring


def convertBinDatasToHex(bindata):

    hexstring = "{ "

    bindatalength = len(bindata)
    print("Binary data length: " + str(bindatalength))
    
    for i in range(0, bindatalength):
        byte = bindata[i]
        hexstring += putByteToCode(byte)
        if i != bindatalength-1:
            # Put ','
            hexstring += ", "
        if (i % 8) == 7:
            hexstring += "\n"

    hexstring += "\n"
    hexstring += "};\n"
    hexstring += "/* End of binary file */\n"

    return hexstring


def putByteToCode(byte):
    # E.g. "0x23"
    return "0x" + format(byte, '02X')

