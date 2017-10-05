/** \mainpage
 * FastenHomeAut project
 * ========================
 *
 * Description
 * ------------------------
 * Welcome in FastenHomeAut project main page.
 *
 * This document created by [Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html)
 *
 * FastenHomeAut is a simple demo and "Hardware tester" software, which written in C by Vizi G�bor.
 *
 * This project has these important software modules:
 * - "TaskHandler" for task scheduling (it is not RTOS!)
 * - "CommandHandler" for command executing from anywhere
 * - "EventHandler" for easy event handling and task schedule requiring
 * - "GlobalVarHandler" for global variable get-set function (it help for debug)
 * - Display: 0.96 OLED
 *
 *
 * Requirements
 * ------------------------
 *
 * Recommended IDE is the Atollic TrueSTUDIO 6.0 or newer, but you can use newer or an other IDE.
 * - Install [Atollic TrueSTUDIO](http://atollic.com/)
 *   + Alternative IDEs: Eclipse GNU Arm, SW4STM32, etc.
 * - Install a terminal (serial port reader) software
 *   + PuTTY
 *   + HyperTerminal
 *   + ZOC
 *   + Termite
 *   + or my terminal: [FastenTerminal](http://fasten.e5tv.hu/)
 * 
 * 
 * How to use it?
 * ------------------------
 * 
 * - Download binaries to your Device
 *   + If you have source code (project) and an IDE, you can compile and Run/Debug your code on the device
 * - Start run / Reset device.
 * - Connect Device to UART-USB converter or other serial converter
 *   + Connect device UART pins to USB-UART converter (Do not forget the swap: TX-RX):
 *     - In STM32F4Discovery, the UART3 used for Debug UART:
 *       + PC6 (TX)
 *       + PC7 (RX)
 *       + GND
 *     - In FastenNode (STM32F0) there is Debug UART connector
 *     - Check the actual pin configurations in Inc\board.h file!
 *   + Plug in the UART-USB converter
 * - Start serial terminal
 * - Connect Terminal to Serial COM port, with 115200 baudrate, and 8N1 configs
 * - Type "help" and press enter (send with '\\r' or '\\n' or together '\\r\\n')
 *   + Now, you set "help" command and available commands printed out on terminal
 * - Enjoy it
 * 
 *
 * Common HW functions (~Hardware tester)
 * ------------------------
 *
 * First:
 * - If you need help, type on terminal: \code help \endcode
 * - If you can't use a command, type this: \code help <CommandName> \endcode
 *   + Example: \code help adcread \endcode
 *
 * For common IO-s:
 * - Send IO pin initialization command:
 *    \code	ioinit <port><pin> <input/output> \endcode
 *   + Example: \code ioinit a1 input  \endcode or \code ioinit b13 output \endcode
 * - Send read / write command:
 *   + Read pin: \code ioin <port><pin> \endcode
 *     - Example: \code ioin a1 \endcode
 *   + Write pin: \code ioout <port><pin> <set/reset> \endcode
 *     - Example: \code ioout b13 set \endcode
 *
 * For ADC-s (Analog-Digital Converter):
 * - Send command, and you received the last Analog states (voltages): \code adc \endcode
 * - If you want periodical ADC reading: \code adcread <milliSec> (pin) \endcode
 *   + milliSec: is the time of period (in millisecond)
 *   + pin: is the num of pin (1-2-3)
 *   + Example: \code adcread 1000 \endcode
 *     - Read adc values time of 1 second (1000 millisecond)
 *
 * For DAC-s (Digital-Analog Converter):
 * - \note Not available in all STM32F4xx!
 * - Send: \code dac <1/2> <voltage with decimal point> \endcode
 *   + Example: \code dac 1 3.0 \endcode
 *   + 1. = A4 pin
 *   + 2. = A5 pin
 *
 * LED functions:
 * - Command: \code led <1/2/3> <on/off/toggle/status> \endcode
 *   + Example: \code led 1 on \endcode
 *     - 1. LED will set on
 *   + Example: \code led status \endcode
 *     - LED statuses will printed out on Terminal
 *   + Example: \code led green off \endcode
 *     - Green LED will turn off
 *
 *
 * How development?
 * ------------------------
 *
 * - First, find Vizi G�bor
 * - Check C language(embedded) tutorials
 * - Read and check Atollic tutorials
 * - Download or request HomeAut embedded codes / project
 * - Develop it, write codes
 *   + Use Doxygen document comment style
 *   + Use cppcheck static code analyzer
 *   + Read "Configs.txt" --> Docs\Configs\Configs.txt for project configuration
 * - Compile
 * - Debug / Program your device
 * - Enjoy it
 *
 *
 * \author	Vizi G�bor
 *
 * <fasten90@gmail.com>
 * <vizi.gabor90@gmail.com>
 *
 * <http://fasten.e5tv.hu>
*/
