//                            USER DEFINED SETTINGS
//   Set driver type, fonts to be loaded, pins used and SPI control method etc
//
//   See the User_Setup_Select.h file if you wish to be able to define multiple
//   setups and then easily select which setup file is used by the compiler.
//
//   If this file is edited correctly then all the library example sketches should
//   run without the need to make any more changes for a particular hardware setup!
#define USER_SETUP_ID 28

// ##################################################################################
//
// Section 0. Call up the right driver file and any options for it
//
// ##################################################################################

// Display type -  only define if RPi display
#define RPI_DISPLAY_TYPE

// Only define one driver
#define ST7796_DRIVER

// ##################################################################################
//
// Section 1. Define the pins that are used to interface with the display here
//
// ##################################################################################

// >>>> EDIT THE PIN NUMBERS IN THE LINES FOLLOWING TO SUIT YOUR ESP8266 SETUP   <<<<

// These are the recommended default

// Avoid PIN_D8 as this will prevent upload if display pulls the pin high at boot

// For NodeMCU - use pin numbers in the form PIN_Dx where Dx is the NodeMCU pin designation
#define TFT_CS   PIN_D2  // Chip select control pin D2
#define TFT_DC   PIN_D3  // Data Command control pin
#define TFT_RST  PIN_D4  // Reset pin (could connect to NodeMCU RST, see next line)
//#define TFT_RST  -1  // Set TFT_RST to -1 if the display RESET is connected to NodeMCU RST or 3.3V

#define TOUCH_CS PIN_D1     // Chip select pin (T_CS) of touch screen

// ##################################################################################
//
// Section 2. Not used for ESP32
//
// ##################################################################################


// ##################################################################################
//
// Section 3. Define the fonts that are to be used here
//
// ##################################################################################

// Comment out the #defines below with // to stop that font being loaded
// The ESP8366 and ESP32 have plenty of memory so commenting out fonts is not
// normally necessary. If all fonts are loaded the extra FLASH space required is
// about 17Kbytes. To save FLASH space only enable the fonts you need!


#define LOAD_FONT2  
#define LOAD_FONT4 

// Comment out the #define below to stop the SPIFFS filing system and smooth font code being loaded
// this will save ~20kbytes of FLASH


// ##################################################################################
//
// Section 4. Not used
//
// ##################################################################################


// ##################################################################################
//
// Section 5. Other options
//
// ##################################################################################

// Define the SPI clock frequency, this affects the graphics rendering speed. Too
// fast and the TFT driver will not keep up and display corruption appears.
// With an RPi ST7796 MH4.0 display 80MHz is OK for ESP32, 40MHz maximum for ESP8266

// #define SPI_FREQUENCY  20000000
   #define SPI_FREQUENCY  40000000

#define SPI_TOUCH_FREQUENCY  2500000

// Comment out the following #define if "SPI Transactions" do not need to be
// supported. When commented out the code size will be smaller and sketches will
// run slightly faster, so leave it commented out unless you need it!

// Transaction support is needed to work with SD library but not needed with TFT_SdFat
// Transaction support is required if other SPI devices are connected.

// Transactions are automatically enabled by the library for an ESP32 (to use HAL mutex)
// so changing it here has no effect

#define SUPPORT_TRANSACTIONS
