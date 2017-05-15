#include "options.h"
#include "Font32x20.h"


/*
 * This font contain only 11 "font character": 0-9 (numbers) + ':' character for hour displaying
 */

#ifdef CONFIG_MODULE_DISPLAY_FONT32X20_ENABLE


/* Font - 32x20 */
const uint32_t Font32x20[FONT_32X20_NUM][FONT_32X20_STORE_SIZE] =
{
		/* Font: " " (dec: 0, hex: 0x00) */
		{ 0x00FFFF00, 0x03FFFFC0, 0x0FF00FF0, 0x1F8000F8, 0x3E00003C, 0x7800001E, 0x7000000E, 0xF000000F, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xF000000F, 0x7000000E, 0x7800001E, 0x3C00003C, 0x1F8000F8, 0x0FF007F0, 0x03FFFFE0, 0x00FFFF00 },
		/* Font: "" (dec: 1, hex: 0x01) */
		{ 0x00000000, 0x00000000, 0x00000000, 0x01C00000, 0x03C00000, 0x07C00000, 0x0F800000, 0x1F000007, 0x3E000007, 0x7C000007, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000007, 0x00000007, 0x00000007, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
		/* Font: "" (dec: 2, hex: 0x02) */
		{ 0x3800001F, 0x7C00003F, 0xFC00007F, 0xF80000FF, 0xF00001F7, 0xE00003E7, 0xE00007C7, 0xE0000F87, 0xE0001F07, 0xE0003E07, 0xE0007C07, 0xF000F807, 0xF001F007, 0xF803E007, 0x7C07C007, 0x3E0F8007, 0x1F1F0007, 0x0FFE0007, 0x07FC0007, 0x03F80007 },
		/* Font: "" (dec: 3, hex: 0x03) */
		{ 0x00000000, 0x6000000C, 0xF003001E, 0xF007801F, 0xF007801F, 0xE007800F, 0xE0078007, 0xE0078007, 0xE0078007, 0xE0078007, 0xE0078007, 0xE00F800F, 0xE01FC00F, 0xF03FE01F, 0xF87FF03E, 0x7FFDF87E, 0x3FF0FFFC, 0x1FE07FF8, 0x0FC03FF0, 0x00001FE0 },
		/* Font: "" (dec: 4, hex: 0x04) */
		{ 0x003FE000, 0x007FE000, 0x03FFE000, 0x0FFFE000, 0x1FF1E000, 0x7F81E000, 0xFE01E000, 0xF801E000, 0x7001E000, 0x0001E000, 0x0001E000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x0001E000, 0x0001E000, 0x0001E000, 0x0001E000, 0x0001E000, 0x0001E000 },
		/* Font: "" (dec: 5, hex: 0x05) */
		{ 0xFFFC000E, 0xFFFC001F, 0xFFFC001F, 0xE01C000F, 0xE01C0007, 0xE01C0007, 0xE01C0007, 0xE01E0007, 0xE01E0007, 0xE01F0007, 0xE00F0007, 0xE00F000F, 0xE007801F, 0xE007E07F, 0xE007FFFE, 0xE001FFF8, 0xE000FFF0, 0x00003FC0, 0x00000000, 0x00000000 },
		/* Font: "" (dec: 6, hex: 0x06) */
		{ 0x007FFFE0, 0x01FFFFF8, 0x0FFFFFFE, 0x1FF0FFFF, 0x3FC0F83F, 0x7E01F00F, 0x7C01E00F, 0xF803C007, 0xF003C007, 0xF003C007, 0xE003C007, 0xE003C007, 0xE003C007, 0xE003E00F, 0xE001E00F, 0x0001F01F, 0x0000F83E, 0x0000FFFE, 0x00007FFC, 0x00001FF0 },
		/* Font: "" (dec: 7, hex: 0x07) */
		{ 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE00001FF, 0xE000FFFF, 0xE007FFFF, 0xE03FFF80, 0xE07FC000, 0xE0FF0000, 0xE1F80000, 0xE3F00000, 0xE7E00000, 0xEFC00000, 0xFF800000, 0xFF000000, 0xFE000000, 0xF0000000, 0x00000000, 0x00000000 },
		/* Font: "" (dec: 8, hex: 0x08) */
		{ 0x00000FF0, 0x07E03FF8, 0x3FF87FFC, 0x3FFCFC7E, 0x7C3FF83F, 0x700FF01F, 0xF007E01F, 0xE007E00F, 0xE003C007, 0xE001C007, 0xE001C007, 0xE003C007, 0xE007E00F, 0x700FE01F, 0x701FF01F, 0x3C3DF83F, 0x3FF8FC7E, 0x0FF07FFC, 0x07C03FF8, 0x00000FF0 },
		/* Font: "	" (dec: 9, hex: 0x09) */
		{ 0x07FC0000, 0x1FFE0000, 0x3F1F0000, 0x7C0F803C, 0xF807803E, 0xF003C03E, 0xE003C01F, 0xE001C00F, 0xE001C007, 0xE001C007, 0xE001C007, 0xE003C007, 0xE003C00F, 0xE007801F, 0xF007803E, 0xF80F007E, 0x7C3E01FC, 0x3FFFFFF8, 0x1FFFFFF0, 0x07FFFF80 },
		/* Font: "
	" (dec: 10, hex: 0x0A) */
		{ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00800200, 0x01C00700, 0x00800200, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
};
/* End of font*/



#endif	// #ifdef CONFIG_MODULE_DISPLAY_FONT32X20_ENABLE
