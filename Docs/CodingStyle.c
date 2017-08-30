/*
 *		CodingStyle.c
 *
 *		Created on:		2017-08-30
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. aug. 30.
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/**
 * \brief	Coding style examples
 */
void foo(void)
{

	// Indent, brackets example
	if (true)
	{
		ToDo();
	}
	else
	{
		ToDo2();
	}


	// Return statements
	return;
	return myDisk.size();
	return (size ? size : defaultSize);


	// One statement in one line
	argv++;		// ok

	argv++; argc--; // AVOID!


	// if, if-else, if-else if-else statements
	if (condition)
	{
		ToDo();
	}

	if (condition1)
	{
		ToDo1();
	}
	else
	{
		ToDo2();
	}

	if (condition1)
	{
		ToDo1();
	}
	else if (condtion2)
	{
		ToDo2();
	}
	else
	{
		ToDo3();
	}


	// for statements
	uint8_t i;
	for (i = 0; i < 10; i++)
	{
		ToDo(i);
	}


	// while statements
	while (condition)
	{
		ToDo();
	}


	// do-while statements
	do
	{
		ToDo();
	} while (condition);


	// switch statements
	switch (condition)
	{
		case 1:
			ToDo1();
			break;
		case 2:
			ToDo2();
			break;
		case 3:
		default:
			ToDoDefault();
			break;
	}


	// Spaces

	// Spaces - operators
	a = b + c;
	b = a + ToDo();
	const char * str = "string";
	if (value == 5)
	{
		array[i + 5] = ToDo();
	}

	// Spaces - between function parameters
	ToDo(1, 2, 3);
	uint32_t ExampleIntegerFunction(uint8_t a, uint32_t b);

	// Space - others
	(uint32_t)plsTypeCastMe;	// No space after type cast


	// Function declaration
	void ExampleFunction(void);
	void ExampleFunction(void)
	{
		ToDo1();
	}

	uint32_t ExampleIntegerFunction(uint8_t a, uint32_t b);
	uint32_t ExampleVeryLongFunctionWithMoreParameters(uint32_t a,
			uint32_t b, uint32_c);


	// Variable declaration
	uint32_t a = 0;
	uint8_t  b = 1;

	// Avoid more declaration in one line
	uint8_t c, d;


	// Comments

	// One line comment, started with "//" and ' ' space

	// This is an uncommented code. Only if necessary

	//UnCommentedCode();

	/*
	 * More line comments
	 */

	/*
	 More line comments (2.)
	 */

	// TODO: Bugfix it!

	// XXX: Important comment

	// \note      Note for others


	// Naming conventions
	bool localVariable;

	uint32_t Module_GlobalVar;

#define CONSTANT	    (5)
	uint32_t a = b + CONSTANT;

	// Naming conventions - functions
	void Module_Init(void);
	void Module_Start(uint32_t a);

	// Naming conventions - avoid:
	void initModuleButNotStartedWithModuleName();   // Avoid: Not started with module name
	void kill();  // Avoid: Not understand, who kill who and why and which module


	// Types
	uint8_t
	uint16_t
	uint32_t
	int8_t
	int16_t
	int32_t
	float
	bool
	char
	char *
	const char *
	;
	uint8_t a    = 255;
	uint16_t  b  = 65535;
	uint32_t c   = 12345;
	int8_t d     = -127;
	int16_t  e   = -30000;
	int32_t f    = -1234567;
	float g      = 5.61;
	bool isBool = true;
	char aCharacter = 'a';
	char * str = "string";
	const char * str = "string";


	// Preprocessor macros
	// At long #ifdef-#endif pair, put the condition after #endif
#define MACRO1
#define MACRO2
#define MACRO3
#ifdef MACRO1
	#if defined(MACRO2) || defined(MACRO3)
	ToDo1();
	#else
	ToDo2();
	#endif
#endif // MACRO1

}


/**
 * \brief	Coding style - Special cases
 */
void SpecialCases(void)
{
	// Special conditions: lot of condition
	if ((con1 && con2) || (con3 || con4))
	{
		ToDo1();
	}
	// Very short statements (one line). Recommended use only with return
	if (con1)
		VeryShortAndSimpleToDo();
	if (con1 && checkParameter)
		return;
	// Special switch: It is beautiful
	switch (a)
	{
		case 0:	b = "string0"; break;
		case 1:	b = "string1"; break;
		case 2:	b = "string2"; break;
		case 3:	b = "string3"; break;
		case 4:	b = "string4"; break;
		default:
			b = "";
				      break;
	}
}


/**
 * \brief     This Module function is an example for Coding Style
 * \param     a      Example a parameter
 * \param     b      Example b parameter
 * \return    Return a test value
 */
uint16_t Module_LargeExample(uint32_t a, uint32_t b)
{
	bool aBool = false;
	uint16_t aNumber = 1234;
	if (a > 10)
	{
		switch (b)
		{
			case 0:
			      ToDo1();
			      break;
			case 1:
			      ToDo2();
			      break;
			case 3:
			default:
			      ToDo3();
			      break;
		}
		aNumber += 12345;
	}
	else if (b > 10)
	{
		aBool = ToDo4();
		aNumber++;
		while (aBool)
		{
			aNumber -= 1;
		}
	}
	return aNumber;
}


