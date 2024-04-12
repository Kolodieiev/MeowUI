// This setup is for the RP2040 processor only when used with 8 bit parallel displays
// See SetupX_Template.h for all options available
#define USER_SETUP_ID 107

////////////////////////////////////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////////////////////////////////////
//#define TFT_PARALLEL_8_BIT
#define TFT_PARALLEL_16_BIT

////////////////////////////////////////////////////////////////////////////////////////////
// Display driver type
////////////////////////////////////////////////////////////////////////////////////////////
#define ILI9341_DRIVER
//#define ILI9481_DRIVER // Tested
//#define HX8357B_DRIVER // Tested
//#define HX8357C_DRIVER // Tested
//#define SSD1963_800_DRIVER

////////////////////////////////////////////////////////////////////////////////////////////
// RP2040 pins used
////////////////////////////////////////////////////////////////////////////////////////////

// These pins can be moved and are controlled directly by the library software
#define TFT_RST   18  // Reset pin
#define TFT_CS    19  // Do not define if chip select control pin permanently connected to 0V
//#define TFT_RD   -1   // Do not define, read pin must be permanently connected to 3V3

// Note: All the following pins are PIO hardware configured and driven
  #define TFT_WR   16    // Write strobe pin
  #define TFT_DC   17    // Data Command control pin

  // PIO requires these to be sequentially increasing
  #define TFT_D0    0
  #define TFT_D1    1
  #define TFT_D2    2
  #define TFT_D3    3
  #define TFT_D4    4
  #define TFT_D5    5
  #define TFT_D6    6
  #define TFT_D7    7
  #define TFT_D8    8
  #define TFT_D9    9
  #define TFT_D10  10
  #define TFT_D11  11
  #define TFT_D12  12
  #define TFT_D13  13
  #define TFT_D14  14
  #define TFT_D15  15
//*/

////////////////////////////////////////////////////////////////////////////////////////////
// Fonts to be available
////////////////////////////////////////////////////////////////////////////////////////////

#define LOAD_FONT2  
#define LOAD_FONT4 

////////////////////////////////////////////////////////////////////////////////////////////