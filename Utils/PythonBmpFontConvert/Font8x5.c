#include "Font8x5.h"


/* Font - 8x5 */
const uint8_t Font8x5[FONT_NUM][FONT_STORE_SIZE] = {
	/* Font: " " (dec: 0, hex: 0x00) */
	{ 0x08, 0x10, 0x3F, 0x10, 0x08 },
	/* Font: "" (dec: 1, hex: 0x01) */
	{ 0x04, 0x02, 0x3F, 0x02, 0x04 },
	/* Font: "" (dec: 2, hex: 0x02) */
	{ 0x04, 0x0E, 0x15, 0x04, 0x04 },
	/* Font: "" (dec: 3, hex: 0x03) */
	{ 0x04, 0x04, 0x15, 0x0E, 0x04 },
	/* Font: "" (dec: 4, hex: 0x04) */
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
	/* Font: "" (dec: 5, hex: 0x05) */
	{ 0xFF, 0xAA, 0xFF, 0xAA, 0xFF },
	/* Font: "" (dec: 6, hex: 0x06) */
	{ 0xFF, 0x55, 0xFF, 0x55, 0xFF },
	/* Font: "" (dec: 7, hex: 0x07) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 8, hex: 0x08) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "	" (dec: 9, hex: 0x09) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "
" (dec: 10, hex: 0x0A) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 11, hex: 0x0B) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 12, hex: 0x0C) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 13, hex: 0x0D) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 14, hex: 0x0E) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 15, hex: 0x0F) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 16, hex: 0x10) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 17, hex: 0x11) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 18, hex: 0x12) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 19, hex: 0x13) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 20, hex: 0x14) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 21, hex: 0x15) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 22, hex: 0x16) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 23, hex: 0x17) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 24, hex: 0x18) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 25, hex: 0x19) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 26, hex: 0x1A) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 27, hex: 0x1B) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 28, hex: 0x1C) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 29, hex: 0x1D) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 30, hex: 0x1E) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "" (dec: 31, hex: 0x1F) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: " " (dec: 32, hex: 0x20) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },
	/* Font: "!" (dec: 33, hex: 0x21) */
	{ 0x00, 0x00, 0x7D, 0x00, 0x00 },
	/* Font: """ (dec: 34, hex: 0x22) */
	{ 0x00, 0x60, 0x00, 0x60, 0x00 },
	/* Font: "#" (dec: 35, hex: 0x23) */
	{ 0x17, 0x3A, 0x12, 0x17, 0x3A },
	/* Font: "$" (dec: 36, hex: 0x24) */
	{ 0x30, 0x49, 0x7F, 0x49, 0x26 },
	/* Font: "%" (dec: 37, hex: 0x25) */
	{ 0x31, 0x32, 0x0C, 0x13, 0x23 },
	/* Font: "&" (dec: 38, hex: 0x26) */
	{ 0x36, 0x49, 0x4D, 0x32, 0x05 },
	/* Font: "'" (dec: 39, hex: 0x27) */
	{ 0x00, 0x70, 0x00, 0x00, 0x00 },
	/* Font: "(" (dec: 40, hex: 0x28) */
	{ 0x00, 0x3E, 0x41, 0x41, 0x00 },
	/* Font: ")" (dec: 41, hex: 0x29) */
	{ 0x00, 0x41, 0x41, 0x3E, 0x00 },
	/* Font: "*" (dec: 42, hex: 0x2A) */
	{ 0x11, 0x0A, 0x3F, 0x14, 0x22 },
	/* Font: "+" (dec: 43, hex: 0x2B) */
	{ 0x08, 0x08, 0x3E, 0x08, 0x08 },
	/* Font: "," (dec: 44, hex: 0x2C) */
	{ 0x00, 0x0D, 0x0E, 0x00, 0x00 },
	/* Font: "-" (dec: 45, hex: 0x2D) */
	{ 0x08, 0x08, 0x08, 0x08, 0x00 },
	/* Font: "." (dec: 46, hex: 0x2E) */
	{ 0x00, 0x02, 0x00, 0x00, 0x00 },
	/* Font: "/" (dec: 47, hex: 0x2F) */
	{ 0x03, 0x0C, 0x10, 0x60, 0x00 },
	/* Font: "0" (dec: 48, hex: 0x30) */
	{ 0x3E, 0x41, 0x41, 0x41, 0x3E },
	/* Font: "1" (dec: 49, hex: 0x31) */
	{ 0x10, 0x20, 0x7F, 0x00, 0x00 },
	/* Font: "2" (dec: 50, hex: 0x32) */
	{ 0x21, 0x43, 0x45, 0x39, 0x00 },
	/* Font: "3" (dec: 51, hex: 0x33) */
	{ 0x41, 0x49, 0x49, 0x36, 0x00 },
	/* Font: "4" (dec: 52, hex: 0x34) */
	{ 0x78, 0x08, 0x08, 0x7F, 0x00 },
	/* Font: "5" (dec: 53, hex: 0x35) */
	{ 0x78, 0x49, 0x49, 0x46, 0x00 },
	/* Font: "6" (dec: 54, hex: 0x36) */
	{ 0x36, 0x49, 0x49, 0x49, 0x06 },
	/* Font: "7" (dec: 55, hex: 0x37) */
	{ 0x43, 0x46, 0x5C, 0x70, 0x00 },
	/* Font: "8" (dec: 56, hex: 0x38) */
	{ 0x36, 0x49, 0x49, 0x36, 0x00 },
	/* Font: "9" (dec: 57, hex: 0x39) */
	{ 0x30, 0x49, 0x49, 0x3E, 0x00 },
	/* Font: ":" (dec: 58, hex: 0x3A) */
	{ 0x00, 0x24, 0x00, 0x00, 0x00 },
	/* Font: ";" (dec: 59, hex: 0x3B) */
	{ 0x00, 0x25, 0x06, 0x00, 0x00 },
	/* Font: "<" (dec: 60, hex: 0x3C) */
	{ 0x08, 0x14, 0x22, 0x41, 0x00 },
	/* Font: "=" (dec: 61, hex: 0x3D) */
	{ 0x14, 0x14, 0x14, 0x14, 0x00 },
	/* Font: ">" (dec: 62, hex: 0x3E) */
	{ 0x41, 0x22, 0x14, 0x08, 0x00 },
	/* Font: "?" (dec: 63, hex: 0x3F) */
	{ 0x20, 0x40, 0x4D, 0x30, 0x00 },
	/* Font: "@" (dec: 64, hex: 0x40) */
	{ 0x3E, 0x25, 0x2B, 0x25, 0x1C },
	/* Font: "A" (dec: 65, hex: 0x41) */
	{ 0x07, 0x3C, 0x64, 0x3C, 0x07 },
	/* Font: "B" (dec: 66, hex: 0x42) */
	{ 0x7F, 0x49, 0x49, 0x36, 0x00 },
	/* Font: "C" (dec: 67, hex: 0x43) */
	{ 0x1C, 0x22, 0x41, 0x41, 0x22 },
	/* Font: "D" (dec: 68, hex: 0x44) */
	{ 0x7F, 0x41, 0x41, 0x22, 0x1C },
	/* Font: "E" (dec: 69, hex: 0x45) */
	{ 0x7F, 0x49, 0x49, 0x49, 0x41 },
	/* Font: "F" (dec: 70, hex: 0x46) */
	{ 0x7F, 0x48, 0x48, 0x48, 0x40 },
	/* Font: "G" (dec: 71, hex: 0x47) */
	{ 0x3E, 0x41, 0x49, 0x49, 0x2E },
	/* Font: "H" (dec: 72, hex: 0x48) */
	{ 0x7F, 0x08, 0x08, 0x7F, 0x00 },
	/* Font: "I" (dec: 73, hex: 0x49) */
	{ 0x41, 0x7F, 0x41, 0x00, 0x00 },
	/* Font: "J" (dec: 74, hex: 0x4A) */
	{ 0x02, 0x01, 0x01, 0x7E, 0x00 },
	/* Font: "K" (dec: 75, hex: 0x4B) */
	{ 0x7F, 0x08, 0x14, 0x22, 0x41 },
	/* Font: "L" (dec: 76, hex: 0x4C) */
	{ 0x7F, 0x01, 0x01, 0x01, 0x01 },
	/* Font: "M" (dec: 77, hex: 0x4D) */
	{ 0x7F, 0x20, 0x10, 0x20, 0x7F },
	/* Font: "N" (dec: 78, hex: 0x4E) */
	{ 0x7F, 0x30, 0x08, 0x06, 0x7F },
	/* Font: "O" (dec: 79, hex: 0x4F) */
	{ 0x1C, 0x22, 0x41, 0x22, 0x1C },
	/* Font: "P" (dec: 80, hex: 0x50) */
	{ 0x7F, 0x48, 0x48, 0x48, 0x30 },
	/* Font: "Q" (dec: 81, hex: 0x51) */
	{ 0x3E, 0x41, 0x43, 0x3D, 0x01 },
	/* Font: "R" (dec: 82, hex: 0x52) */
	{ 0x7F, 0x44, 0x44, 0x46, 0x39 },
	/* Font: "S" (dec: 83, hex: 0x53) */
	{ 0x31, 0x49, 0x49, 0x46, 0x00 },
	/* Font: "T" (dec: 84, hex: 0x54) */
	{ 0x40, 0x40, 0x7F, 0x40, 0x40 },
	/* Font: "U" (dec: 85, hex: 0x55) */
	{ 0x7E, 0x01, 0x01, 0x01, 0x7E },
	/* Font: "V" (dec: 86, hex: 0x56) */
	{ 0x30, 0x0C, 0x03, 0x0C, 0x30 },
	/* Font: "W" (dec: 87, hex: 0x57) */
	{ 0x7C, 0x03, 0x1C, 0x03, 0x7C },
	/* Font: "X" (dec: 88, hex: 0x58) */
	{ 0x41, 0x36, 0x08, 0x36, 0x41 },
	/* Font: "Y" (dec: 89, hex: 0x59) */
	{ 0x41, 0x32, 0x0C, 0x30, 0x40 },
	/* Font: "Z" (dec: 90, hex: 0x5A) */
	{ 0x43, 0x45, 0x49, 0x51, 0x61 },
	/* Font: "[" (dec: 91, hex: 0x5B) */
	{ 0x00, 0x7F, 0x41, 0x00, 0x00 },
	/* Font: "\" (dec: 92, hex: 0x5C) */
	{ 0xE0, 0x30, 0x0C, 0x03, 0x01 },
	/* Font: "]" (dec: 93, hex: 0x5D) */
	{ 0x00, 0x41, 0x7F, 0x00, 0x00 },
	/* Font: "^" (dec: 94, hex: 0x5E) */
	{ 0x10, 0x20, 0x40, 0x20, 0x10 },
	/* Font: "_" (dec: 95, hex: 0x5F) */
	{ 0x01, 0x01, 0x01, 0x01, 0x01 },
	/* Font: "`" (dec: 96, hex: 0x60) */
	{ 0x40, 0x20, 0x10, 0x00, 0x00 },
	/* Font: "a" (dec: 97, hex: 0x61) */
	{ 0x02, 0x15, 0x15, 0x0E, 0x01 },
	/* Font: "b" (dec: 98, hex: 0x62) */
	{ 0x7F, 0x09, 0x09, 0x06, 0x00 },
	/* Font: "c" (dec: 99, hex: 0x63) */
	{ 0x06, 0x09, 0x09, 0x00, 0x00 },
	/* Font: "d" (dec: 100, hex: 0x64) */
	{ 0x06, 0x09, 0x09, 0x7F, 0x00 },
	/* Font: "e" (dec: 101, hex: 0x65) */
	{ 0x0E, 0x15, 0x15, 0x0C, 0x00 },
	/* Font: "f" (dec: 102, hex: 0x66) */
	{ 0x00, 0x1F, 0x24, 0x24, 0x00 },
	/* Font: "g" (dec: 103, hex: 0x67) */
	{ 0x00, 0x09, 0x15, 0x15, 0x0E },
	/* Font: "h" (dec: 104, hex: 0x68) */
	{ 0x3F, 0x08, 0x08, 0x07, 0x00 },
	/* Font: "i" (dec: 105, hex: 0x69) */
	{ 0x00, 0x00, 0x2F, 0x00, 0x00 },
	/* Font: "j" (dec: 106, hex: 0x6A) */
	{ 0x02, 0x01, 0x01, 0x5E, 0x00 },
	/* Font: "k" (dec: 107, hex: 0x6B) */
	{ 0x7F, 0x04, 0x0A, 0x11, 0x00 },
	/* Font: "l" (dec: 108, hex: 0x6C) */
	{ 0x00, 0x00, 0x7F, 0x00, 0x00 },
	/* Font: "m" (dec: 109, hex: 0x6D) */
	{ 0x07, 0x08, 0x04, 0x08, 0x07 },
	/* Font: "n" (dec: 110, hex: 0x6E) */
	{ 0x0F, 0x04, 0x08, 0x08, 0x07 },
	/* Font: "o" (dec: 111, hex: 0x6F) */
	{ 0x06, 0x09, 0x09, 0x06, 0x00 },
	/* Font: "p" (dec: 112, hex: 0x70) */
	{ 0x00, 0x3F, 0x28, 0x38, 0x00 },
	/* Font: "q" (dec: 113, hex: 0x71) */
	{ 0x00, 0x38, 0x28, 0x3F, 0x00 },
	/* Font: "r" (dec: 114, hex: 0x72) */
	{ 0x0F, 0x04, 0x08, 0x08, 0x00 },
	/* Font: "s" (dec: 115, hex: 0x73) */
	{ 0x08, 0x15, 0x15, 0x02, 0x00 },
	/* Font: "t" (dec: 116, hex: 0x74) */
	{ 0x08, 0x3E, 0x0B, 0x09, 0x00 },
	/* Font: "u" (dec: 117, hex: 0x75) */
	{ 0x0E, 0x01, 0x01, 0x0E, 0x00 },
	/* Font: "v" (dec: 118, hex: 0x76) */
	{ 0x08, 0x06, 0x01, 0x06, 0x08 },
	/* Font: "w" (dec: 119, hex: 0x77) */
	{ 0x0E, 0x01, 0x06, 0x01, 0x0E },
	/* Font: "x" (dec: 120, hex: 0x78) */
	{ 0x11, 0x0A, 0x04, 0x0A, 0x11 },
	/* Font: "y" (dec: 121, hex: 0x79) */
	{ 0x11, 0x0A, 0x04, 0x08, 0x10 },
	/* Font: "z" (dec: 122, hex: 0x7A) */
	{ 0x23, 0x25, 0x29, 0x31, 0x00 },
	/* Font: "{" (dec: 123, hex: 0x7B) */
	{ 0x08, 0x36, 0x41, 0x00, 0x00 },
	/* Font: "|" (dec: 124, hex: 0x7C) */
	{ 0x00, 0x00, 0x7F, 0x00, 0x00 },
	/* Font: "}" (dec: 125, hex: 0x7D) */
	{ 0x41, 0x36, 0x08, 0x00, 0x00 },
	/* Font: "~" (dec: 126, hex: 0x7E) */
	{ 0x08, 0x10, 0x08, 0x04, 0x08 },
	/* Font: "" (dec: 127, hex: 0x7F) */
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
};
/* End of font*/
