        ////////////////////////////////////////////////////
        // Setup for Nucleo 64 or 144 and ILI9341 display //
        ////////////////////////////////////////////////////

#define USER_SETUP_ID 30

// See SetupX_Template.h for all options available

// Define STM32 to invoke optimised processor support
#define STM32

// Defining the board allows the library to optimise the performance
// for UNO compatible "MCUfriend" style shields
#define NUCLEO_64_TFT
//#define NUCLEO_144_TFT

// Tell the library to use 8 bit parallel mode (otherwise SPI is assumed)
#define TFT_PARALLEL_8_BIT

// Define the display driver chip type
#define ILI9341_DRIVER
//#define ILI9481_DRIVER

// Define the Nucleo 64/144 pins used for the parallel interface TFT
// The pins can be changed here but these are the ones used by the
// common "MCUfriend" shields
#define TFT_CS   A3  // Chip select control pin
#define TFT_DC   A2  // Data Command control pin
#define TFT_RST  A4  // Reset pin

#define TFT_WR   A1  // Write strobe control pin 
#define TFT_RD   A0  // Read pin

#define TFT_D0   D8  // 8 bit parallel bus to TFT
#define TFT_D1   D9
#define TFT_D2   D2
#define TFT_D3   D3
#define TFT_D4   D4
#define TFT_D5   D5
#define TFT_D6   D6
#define TFT_D7   D7

// Fonts to be available

#define LOAD_FONT2  
#define LOAD_FONT4 
