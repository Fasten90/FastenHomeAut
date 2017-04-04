"""
Image -> Binary converter
for 0.96'OLED
@author	Vizi Gabor
"""



# Parameters
imagename = 'Car_Wheel_3_16x16.bmp'
codefilename = 'Image_CarWheel_3_16x16'

img_width = 16
img_height = 16

"""
imagename = 'car_v3_cutted_120x48.bmp'
codefilename = 'Image_Car_120x48'

img_width = 120
img_height = 48
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



def print_image(line):
	""" Print a font line (on console) """
	line_string = ""
	"""
	for pixel in range(0, sizex):
		if (line[pixel] == 204):
			line_string += "X"
		elif (line[pixel] <= 240):
			line_string += "#"
		else:
			line_string += " "
		"""
	for pixel in range(0, int(sizex/2)):
		twopixel = (line[pixel*2] + line[(pixel*2)+1]) / 2
		if (twopixel > 200):
			line_string += " "
		elif (twopixel > 100):
			line_string += "X"
		else:
			line_string += "#"
	print(line_string)
	return line_string

	

def print_bytepixels_to_codefile(pixels):
	global codefile

	# Check all pixels
	code_byte = 0x00
	for i in range(0, 8):
		# Check this pixel
		pixel = pixels[i]
		if pixel <= 240:
			# Black pixel
			code_byte <<= 1
			code_byte |= 0x01
		else:
			# White (empty) pixel
			code_byte <<= 1
	code_byte &= 0xFF
	codefile.write("0x" + format(code_byte, '02X'))
	
	codefile.write(", ")
	# End of this byte



# Convert pixel colours to one color (BLUE)
skip_index = 0
for i in range(0, sizex*sizey):
	try:
		if (sizex % 4) != 0:
			if i % (sizex+1) == 0:
				# Skip last byte
				skip_index += 1
			else:
				bitmap[i] = bitmap_process[i*3+skip_index]
		else:
			bitmap[i] = bitmap_process[i*3]
	except Exception as excpt:
		print("Error:" + str(excpt))


# Convert bitmap to top --> bottom bitmap
for bitmap_line_index in range (0, sizey):
	bitmap2[(bitmap_line_index*sizex) : ((bitmap_line_index + 1) * sizex)] \
		= bitmap[(sizex*sizey - (bitmap_line_index + 1) * sizex) : (sizex*sizey - bitmap_line_index * sizex)]


bitmap = bitmap2


# Convert to matrix
Matrix = [[0 for x in range(sizex)] for y in range(sizey)]

for bitmap_line_index in range(0, sizey):
	Matrix[bitmap_line_index][0:sizex] = bitmap[bitmap_line_index*sizex : (bitmap_line_index+1)*sizex]


# Print header of code file
codefile.write("#include \"" + codefilename + ".h\"\n")
codefile.write("\n\n\n")
codefile.write("/* " + codefilename + " */\n")



# Normal image:
# Print image to stdout (print) & index image to code file

codefile.write("/*\n")
for row in range(0, img_height):
	# Print this line
	line = []
	line = Matrix[row][:]
	line_string = print_image(line)
	codefile.write(line_string)
	codefile.write("\n")

codefile.write("*/\n")


# Print image to binary code

# "const uint8_t image[16 * 16 / 8] =
codefile.write("const uint8_t " + codefilename + "[" + str(img_width) + " * " + str(img_height) + " / 8] = {\n")

for row in range(0, int(img_height/8)):
	for column in range(0, img_width):
		# Process this 8x1 pixel
		try:
			pixels = [0 for x in range(8)]
			for pixel in range(0, 8):
				pixels[pixel] = Matrix[(row+1)*8-pixel-1][column]
				#pixels[pixel] = Matrix[row*8 + pixel][column]
				#pixels[pixel] = Matrix[row][column*8 + pixel]
		except Exception as excpt:
			print("Error: " + str(excpt))
		print_bytepixels_to_codefile(pixels)
	# Print \n	
	codefile.write("\n")	



# Print end of code file
codefile.write("};\n")
codefile.write("/* End of image*/\n")
