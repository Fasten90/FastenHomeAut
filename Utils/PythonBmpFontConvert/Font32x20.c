#include "options.h"
#include "Font32x20.h"



/*
 * This font contain only 11 "font character": 0-9 (numbers) + ':' character for hour displaying
 */

#ifdef CONFIG_DISPLAY_FONT32X20_ENABLE

/* Font - 32x20 */
const uint32_t Font32x20[FONT_32X20_NUM][FONT_32X20_STORE_SIZE] = {
    /* Font: "0" (dec: 0, hex: 0x00) */
    { 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xE0000007, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 },
    /* Font: "1" (dec: 1, hex: 0x01) */
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 },
    /* Font: "2" (dec: 2, hex: 0x02) */
    { 0x00000000, 0xF801FFFF, 0xE001FFFF, 0xE001FFFF, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xFFFFC007, 0xFFFFC01F, 0x00000000 },
    /* Font: "3" (dec: 3, hex: 0x03) */
    { 0x00000000, 0xF803E01F, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 },
    /* Font: "4" (dec: 4, hex: 0x04) */
    { 0x00000000, 0xFFFFC000, 0xFFFFC000, 0xFFFFC000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0x0001C000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 },
    /* Font: "5" (dec: 5, hex: 0x05) */
    { 0x00000000, 0xFFFFC01F, 0xFFFFC007, 0xFFFFC007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001FFFF, 0xE001FFFF, 0xF801FFFF, 0x00000000 },
    /* Font: "6" (dec: 6, hex: 0x06) */
    { 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001FFFF, 0xE001FFFF, 0xF801FFFF, 0x00000000 },
    /* Font: "7" (dec: 7, hex: 0x07) */
    { 0x00000000, 0xFFFE0000, 0xFFFE0000, 0xFFFE0000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xE0000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 },
    /* Font: "8" (dec: 8, hex: 0x08) */
    { 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 },
    /* Font: "9" (dec: 9, hex: 0x09) */
    { 0x00000000, 0xFFFFC01F, 0xFFFFC007, 0xFFFFC007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xE001C007, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 },
    /* Font: "10" (dec: 10, hex: 0x0A) */
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x07C00700, 0x07C00700, 0x07C00700, 0x07C00700, 0x07C00700, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    /* Font: "11" (dec: 11, hex: 0x0B) */
    { 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xE001C000, 0xFFFFC000, 0xFFFFC000, 0xFFFFC000, 0x00000000 },
    /* Font: "12" (dec: 12, hex: 0x0C) */
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
};
/* End of font*/


#endif    /* #ifdef CONFIG_DISPLAY_FONT32X20_ENABLE */
