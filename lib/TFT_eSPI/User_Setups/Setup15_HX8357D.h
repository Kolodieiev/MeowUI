// See SetupX_Template.h for all options available
#define USER_SETUP_ID 15

#define HX8357D_DRIVER

// If green and blue swapped on display then change the RGB colour order
// Only uncomment ONE of the following options
#define TFT_RGB_ORDER TFT_RGB    //   Red-Green-Blue
//#define TFT_RGB_ORDER TFT_BGR    //   Blue-Green-RED

// For NodeMCU - use pin numbers in the form PIN_Dx where Dx is the NodeMCU pin designation
#define TFT_CS   PIN_D8  // Chip select control pin D8
#define TFT_DC   PIN_D3  // Data Command control pin
#define TFT_RST  PIN_D4  // Reset pin (could connect to NodeMCU RST, see next line)
//#define TFT_RST  -1  // Set TFT_RST to -1 if the display RESET is connected to NodeMCU RST or 3.3V



#define LOAD_FONT2  
#define LOAD_FONT4 


// #define SPI_FREQUENCY  20000000
#define SPI_FREQUENCY  27000000
// #define SPI_FREQUENCY  40000000

#define SPI_TOUCH_FREQUENCY  2500000


// #define SUPPORT_TRANSACTIONS
