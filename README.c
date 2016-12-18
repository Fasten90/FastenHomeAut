/** \mainpage
 * HomeAut
 * ========================
 *
 * Description
 * ------------------------
 * Welcome in HomeAut project main page.
 *
 * This document created by [Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html)
 *
 * HomeAut is a "Hardware tester" software, which written in C.
 *
 *
 * Requirements
 * ------------------------
 *
 * Recommended IDE is the Atollic TrueSTUDIO 6.0, but you can use an other IDE.
 * - Install [Atollic TrueSTUDIO](http://atollic.com/)
 * - Install a terminal (serial port reader) software
 * 
 * 
 * How to use it?
 * ------------------------
 * 
 * - Download binaries to your Device
 *   + If you have source code (project) and an IDE, you can compile and Run/Debug your code on the device
 * - Start run / Reset device.
 * - Connect Device to UART-USB converter or other serial converter
 *   + Connect device UART pins (PC6 - TX, PC7 - RX) to UART converter
 *   + Plug in the UART-USB converter
 * - Start Serial Terminal
 *   + For example:
 *      - HpyerTerminal
 *      - PuTTY
 *      - ZOC
 *      - [FastenTerminal](http://fasten.e5tv.hu/)
 * - Connect Terminal to Serial COM port, with 9600 baudrate, and 8N1 configs
 * - Type "help" and press enter (send with '\\r' or '\\n' or together '\\r\\n')
 *   + Now, you set "help" command and available commands printed out on terminal
 * - Enjoy it
 * 
 *
 * Common HW functions
 * ------------------------
 *
 * First:
 * If you need help, type on terminal: \code help \endcode
 * If you can't use a command, type this: \code help <CommandName> \endcode
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
 * - Send command, and you received Analog states (voltages): \code adc \endcode
 * - If you want periodical ADC reading: \code adcread <milliSec> \endcode
 *   + Where the <milliSec> is the time of period
 *
 * For DAC-s (Digital-Analog Converter):
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
 *   + Example: \code led blue off \endcode
 *     - Blue LED will turn off
 *
 * How development?
 * ------------------------
 *
 * - First, find Vizi Gábor
 * - Check C language(embedded) tutorials
 * - Read and check Atollic tutorials
 * - Download or request HomeAut embedded codes
 * - Develop it, write codes
 * - Program your device
 * - Enjoy it
 *
 *
 * \author	Vizi Gábor
 *
 * <fasten90@gmail.com>
 *
 * <http://fasten.e5tv.hu>
*/
