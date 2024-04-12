// For ESP8266
// See SetupX_Template.h for all options available
#define USER_SETUP_ID 10

#define RPI_DISPLAY_TYPE
#define ILI9486_DRIVER

// For NodeMCU - use pin numbers in the form PIN_Dx where Dx is the NodeMCU pin designation
#define TFT_CS   PIN_D2  // Chip select control pin D2
#define TFT_DC   PIN_D3  // Data Command control pin
#define TFT_RST  PIN_D4  // Reset pin (could connect to NodeMCU RST, see next line)
//#define TFT_RST  -1  // Set TFT_RST to -1 if the display RESET is connected to NodeMCU RST or 3.3V

#define TOUCH_CS PIN_D1     // Chip select pin (T_CS) of touch screen


#define LOAD_FONT2
#define LOAD_FONT4


#define SPI_FREQUENCY  16000000 // Some displays will operate at higher frequencies

#define SPI_TOUCH_FREQUENCY  2500000


// #define SUPPORT_TRANSACTIONS
