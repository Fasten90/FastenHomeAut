/*
 * formattedMessage.h
 *
 *  Created on: Sep 10, 2016
 *      Author: Vizi Gábor
 */

#ifndef FORMATTEDMESSAGE_H_
#define FORMATTEDMESSAGE_H_


/// DEFINES

// Example:
// USART_SendString("\x1B" "[30m");	= Black text color

#define USART_ESCAPE_TEXT_START		"\x1B" "[3"
#define USART_ESCAPE_TEXT_END		"m"

// Example:
// USART_SendString("\x1B" "[47m"); = White background color
#define USART_ESCAPE_BACKGROUND_START	"\x1B" "[4"
#define USART_ESCAPE_BACKGROUND_END		"m"

/// TYPEDEFS


// Intensity 	0 	1 	2 	3 	4 	5 	6 	7
// Normal 	Black 	Red 	Green 	Yellow 	Blue 	Magenta 	Cyan 	White

typedef enum
{
	Color_Black = 0,
	Color_Red,
	Color_Green,
	Color_Yellow,
	Color_Blue,
	Color_Magenta,
	Color_Cyan,
	Color_White
} FormattedStringColors_t;



/// FUNCTION PROTOTYPES

void SendColouredMessageWithBackgroundColor (const char* message, FormattedStringColors_t textColor, FormattedStringColors_t backgroundColor);
void SendColouredMessage (const char* message, FormattedStringColors_t textColor);
void SendTextColor (FormattedStringColors_t textColor);
void SendBackgroundColor (FormattedStringColors_t backgroundColor);
void SendErrorMessage (const char *message);






#endif /* FORMATTEDMESSAGE_H_ */
