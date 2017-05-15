"""
Python - BMP -> Font converter
For Display - HomeAut system (FastenNode)

@author	Vizi Gábor
"""


# Parameters

"""
# Font 8x5 - 128 ASCII characters in 4 line
imagename = 'Font_8x5_v6.bmp'
codefilename = 'Font8x5'

fontname = "Font - 8x5"

font_store_height = 8
font_store_width = 5

column_storing = True
byte_storing = 1

font_line_numbers = 4
font_character_numbers = 128
"""

"""
# Font 12x8 - 128 ASCII characters in 4 line
imagename = 'Font_12x8_v2.bmp'
codefilename = 'Font12x8'

fontname = "Font - 12x8"

font_store_height = 12
font_store_width = 8

column_storing = False
byte_storing = 1

font_line_numbers = 4
font_character_numbers = 128
"""

"""
# Font 32x20 - "0-9" + ":"
imagename = 'Font_32x20.bmp'
codefilename = 'Font32x20'

fontname = "Font - 32x20"

font_store_height = 32
font_store_width = 20

column_storing = True
byte_storing = 4

font_line_numbers = 1
font_character_numbers = 11
"""


print("Image file name: " + imagename)
print("Code file name: " + codefilename)


file = open(imagename,'rb')
codefile = open(codefilename + ".c", 'wt')

data = file.read()

file.close()

sizex = int.from_bytes(data[18:21], byteorder='little', signed=False)
sizey = int.from_bytes(data[22:25], byteorder='little', signed=False)

print("Size X: " + str(sizex))
print("Size Y: " + str(sizey))



bitmap_process = []

bitmap_process = data[54:]

bitmap = [0 for x in range(sizex*sizey)]

bitmap2 = []


def process_font(font, index):
	""" Process a font character (print stdout and file)"""
	global font_store_height
	print("Index: " + str(index))
	print_font(font)
	print_font_to_codefile(font, index)


def print_font(font):
	""" Print a font character to stdout """
	for line in range(0, font_store_height):
		print_font_line(font[line])


def print_font_line(font_line):
	""" Print a font line (on console) """
	global font_store_width
	line_string = ""
	for i in range(0, font_store_width):
		if (font_line[i] == 204):
			line_string += "X"
		elif (font_line[i] <= 240):
			line_string += "#"
		else:
			line_string += " "
	print(line_string)

	
def print_font_to_codefile(font, index):
	""" Print a font character to code file """
	global codefile
	# Print index
	codefile.write("\t/* Font: \"" + chr(index) + "\" (dec: " + str(index) + ", hex: 0x" + format(index, '02X') +") */\n")

	codefile.write("\t{ ")	

	if column_storing:
		# 1. 2. 3...
		# 1. 2. 3...
		# 1. 2. 3...
		# 1. 2. 3...
		# Step on colums ( --> )
		for column in range(0, font_store_width):
			# Create a column
			# Step on rows:
			#  |
			#  |
			#  ˇ
			code_byte = 0x00
			for row in range(0, font_store_height):
				pixel = font[row][column]
				if pixel <= 240:
					code_byte |= 0x01
				if row != font_store_height-1:
					code_byte <<= 1

			# Mask & Write bytes
			if byte_storing == 1:
				code_byte &= 0xFF
				codefile.write("0x" + format(code_byte, '02X'))
			elif byte_storing == 4:
				code_byte &= 0xFFFFFFFF
				codefile.write("0x" + format(code_byte, '08X'))
			else:
				code_byte &= 0xFF
				codefile.write("0x" + format(code_byte, '02X'))
			if column != font_store_width-1:
				codefile.write(", ")
	else:
		# row_storing
		for row in range(0, font_store_height):
			code_byte = 0x00
			for column in range(0, font_store_width):
				pixel = font[row][column]
				if pixel <= 240:
					code_byte |= 0x01

				if column != font_store_width-1:
					code_byte <<= 1
			code_byte &= 0xFF
			codefile.write("0x" + format(code_byte, '02X'))
			if row != font_store_height-1:
				codefile.write(", ")
	# End of this font
	codefile.write(" },\n")


# Convert pixel colours to one color (BLUE)
skip_index = 0
for i in range(0, sizex*sizey):
	try:
		if (sizex % 4) != 0:
			if i % (sizex+1) == 0:
				# Skip last byte(s) of row
				skip_index += sizex%4
			# Copy pixel
			bitmap[i] = bitmap_process[i*3+skip_index]
		else:
			# Copy pixel without problem
			bitmap[i] = bitmap_process[i*3]
	except Exception as excpt:
		print("Error:" + str(excpt))


#if need_extend:
	# Restore row length
#	 sizex = original_sizex

# Convert bitmap to top --> bottom bitmap
for bitmap_line_index in range (0, sizey):
	bitmap2[(bitmap_line_index*sizex) : ((bitmap_line_index + 1) * sizex)] \
		= bitmap[(sizex*sizey - (bitmap_line_index + 1) * sizex) : (sizex*sizey - bitmap_line_index * sizex)]


bitmap = bitmap2


# Convert to matrix
Matrix = [[0 for x in range(sizex)] for y in range(sizey)]

for bitmap_line_index in range(0, sizey):
	Matrix[bitmap_line_index][0:sizex] = bitmap[bitmap_line_index*sizex : (bitmap_line_index+1)*sizex]


# Skip every font_store_height + 1. index line
# Skip "------------------------------"
line_index = 0
for bitmap_line_index in range (0, sizey):
	if (bitmap_line_index % (font_store_height+1)) != 0:
		# Copy this line
		Matrix[line_index][:] = Matrix[bitmap_line_index][:]
		line_index += 1


sizey = font_store_height * font_line_numbers


# Skip every "|"
new_bitmap_width = int(font_store_width * font_character_numbers / font_line_numbers)
column_index = 0

# Process a colum
for bitmap_column_index in range(0, sizex):
	# Check actual column
	try:
		if (bitmap_column_index % (font_store_width+1)) != 0:
			# Copy actual column, if good
			for line in range(0, sizey):
				Matrix[line][column_index] = Matrix[line][bitmap_column_index]
			column_index += 1
	except Exception as excpt:
		print("Error...")

sizex = new_bitmap_width


# Lines
font = [[0 for x in range(font_store_width)] for y in range(font_store_height)]


# Print header of code file
# TODO: include
codefile.write("#include \"" + codefilename + ".h\"\n")
codefile.write("\n")
codefile.write("\n")
codefile.write("/* " + fontname + " */\n")
codefile.write("const uint8_t " + codefilename +"[FONT_NUM][FONT_STORE_SIZE] = {\n")


for font_line_index in range(0, font_line_numbers):
	for font_column_index in range(0, int(font_character_numbers / font_line_numbers)):
		# This code not worked?
		#font[0:font_store_height][0:font_store_width] \
		#	= Matrix[font_line_index*font_store_height:(font_line_index+1)*font_store_height] \
		#		[font_column_index*font_store_width:(font_column_index+1)*font_store_width]
		for y in range(font_store_height):	
			for x in range(font_store_width):
				font[y][x] \
					= Matrix[(font_line_index*font_store_height) + y] \
						[(font_column_index*font_store_width) + x]

		# Process this font
		index = font_line_index*int(font_character_numbers / font_line_numbers) + font_column_index
		process_font(font, index)


# Print end of code file
codefile.write("};\n")
codefile.write("/* End of font*/\n")
