#include "options.h"
#include "Font32x20.h"



/*
 * This font contain only 14 "font character": 0-9 (numbers) + ':' (colon) character for hour displaying, and 'P' as parking, and 'F'
 */

#ifdef CONFIG_DISPLAY_FONT32X20_ENABLE

/* Font - 32x20 */
const uint32_t Font32x20[FONT_32X20_NUM][FONT_32X20_STORE_SIZE] = {
    /* Font: "0" (dec: 0, hex: 0x00) */
    { 0x00000000, 0x1FFE3FF8, 0x1FFE3FF8, 0x1FFE3FF8, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0x1FFE3FF8, 0x1FFE3FF8, 0x1FFE3FF8, 0x00000000 },
    /* Font: "1" (dec: 1, hex: 0x01) */
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x1FFE3FF8, 0x1FFE3FF8, 0x1FFE3FF8, 0x00000000 },
    /* Font: "2" (dec: 2, hex: 0x02) */
    { 0x00000000, 0x00003FF8, 0x00003FF8, 0xE0003FF8, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0x1FFE0000, 0x1FFE0000, 0x1FFE0000, 0x00000000 },
    /* Font: "3" (dec: 3, hex: 0x03) */
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0x1FFE3FF8, 0x1FFE3FF8, 0x1FFE3FF8, 0x00000000 },
    /* Font: "4" (dec: 4, hex: 0x04) */
    { 0x00000000, 0x1FFE0000, 0x1FFE0000, 0x1FFE0000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x1FFE3FF8, 0x1FFE3FF8, 0x1FFE3FF8, 0x00000000 },
    /* Font: "5" (dec: 5, hex: 0x05) */
    { 0x00000000, 0x1FFE0007, 0x1FFE0007, 0x1FFE0007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0x00003FF8, 0x00003FF8, 0x00003FF8, 0x00000000 },
    /* Font: "6" (dec: 6, hex: 0x06) */
    { 0x00000000, 0x1FFE3FF8, 0x1FFE3FF8, 0x1FFE3FF8, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0x00003FF8, 0x00003FF8, 0x00003FF8, 0x00000000 },
    /* Font: "7" (dec: 7, hex: 0x07) */
    { 0x00000000, 0x1FFE0000, 0x1FFE0000, 0x1FFE0000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0x1FFE3FFF, 0x1FFE3FFF, 0x1FFE3FFF, 0x00000000 },
    /* Font: "8" (dec: 8, hex: 0x08) */
    { 0x00000000, 0x1FFE3FF8, 0x1FFE3FF8, 0x1FFE3FF8, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0x1FFE3FF8, 0x1FFE3FF8, 0x1FFE3FF8, 0x00000000 },
    /* Font: "9" (dec: 9, hex: 0x09) */
    { 0x00000000, 0x1FFE0000, 0x1FFE0000, 0x1FFE0000, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0x1FFE3FF8, 0x1FFE3FF8, 0x1FFE3FF8, 0x00000000 },
    /* Font: "10" (dec: 10, hex: 0x0A) */
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x07C00700, 0x07C00700, 0x07C00700, 0x07C00700, 0x07C00700, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    /* Font: "11" (dec: 11, hex: 0x0B) */
    { 0x00000000, 0x1FFE3FFF, 0x1FFE3FFF, 0x1FFE3FFF, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0x1FFE0000, 0x1FFE0000, 0x1FFE0000, 0x00000000 },
    /* Font: "12" (dec: 12, hex: 0x0C) */
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    /* Font: "13" (dec: 13, hex: 0x0D) */
    { 0x00000000, 0x1FFE3FF8, 0x1FFE3FF8, 0x1FFE3FF8, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
};
/* End of font*/


#endif    /* #ifdef CONFIG_DISPLAY_FONT32X20_ENABLE */
