// For ESP32
// See SetupX_Template.h for all options available
#define USER_SETUP_ID 11

#define RPI_DISPLAY_TYPE
#define ILI9486_DRIVER

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   15  // Chip select control pin
#define TFT_DC    2  // Data Command control pin
#define TFT_RST   4  // Reset pin (could connect to RST pin)
//#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST

#define TOUCH_CS 22     // Chip select pin (T_CS) of touch screen


#define LOAD_FONT2
#define LOAD_FONT4


#define SPI_FREQUENCY  20000000 // Some displays will operate at higher frequencies

#define SPI_TOUCH_FREQUENCY  2500000
