// USART
// #define F_CPU 16000000UL
#define UBRR                    F_CPU / 16 / BAUD - 1 // from documentation
#define BAUD                    9600    // baud rate

// DELAYS
#define SCAN_DELAY              10      // in us, it is necessary to wait between switching direction and reading while scanning
#define DEBOUNCE_DELAY          20      // in ms, to debounce keyboard
#define PRESS_DELAY             500     // in ms, if delay between button presses is greater than this, device sends out the last pressed key

// PRINTING
#define ASCII_NUMERAL_OFFSET    48      // turns numerals into their respected char
#define ASCII_UPPER_CASE_OFFSET 32      // distance between lower and upper case letter in ASCII
#define COLUMN_COUNT            3       // keyboard matrix size
#define ROW_COUNT               4
#define DELETE_SYMBOL           "<-"    // can be detected by recieving device
#define CAP_DEL_DISPLAY_SIZE    true    // if true, only deletable characters on LCD are deletable in USART output

// DEBUG
#define DEBUG_PRINT_KEYPAD_READING false