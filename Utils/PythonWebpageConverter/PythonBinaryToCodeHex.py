"""
Binary file --> Code Hex text
@author	Vizi Gabor
"""


def convertBinaryFileToHexFile(binaryfilename, codefilename):

    print("Binary file name: " + binaryfilename)
    print("Code file name: " + codefilename)


    binaryfile = open(binaryfilename, 'rb')
    codefile = open(codefilename + ".c", 'wt')

    data = binaryfile.read()

    binaryfile.close()

    codefilecontainstring = convertBinDatasToHex(data)

    codefile.write(codefilecontainstring)
    codefile.close()


def convertBinDatasToHex(bindata):

    hexstring = "{ "

    bindatalength = len(bindata)
    for i in range(0, bindatalength):
        byte = bindata[i]
        hexstring += putByteToCode(byte)
        if i != bindatalength-1:
            # Put ','
            hexstring += ", "
        if (i % 16) == 15:
            hexstring += "\n"

    hexstring += " };\n"
    hexstring += "/* End of binary file */"

    return hexstring


def putByteToCode(byte):
    # E.g. "0x23"
    return "0x" + format(byte, '02X')

