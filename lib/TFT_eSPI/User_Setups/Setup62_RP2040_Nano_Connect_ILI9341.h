//                            USER DEFINED SETTINGS
//   Set driver type, fonts to be loaded, pins used and SPI control method etc
//
//   See the User_Setup_Select.h file if you wish to be able to define multiple
//   setups and then easily select which setup file is used by the compiler.
//
//   If this file is edited correctly then all the library example sketches should
//   run without the need to make any more changes for a particular hardware setup!
//   Note that some sketches are designed for a particular TFT pixel width/height

#define USER_SETUP_ID 62
// ##################################################################################
//
// Section 1. Call up the right driver file and any options for it
//
// ##################################################################################

// Tell the library to use 8 bit parallel mode (otherwise SPI is assumed)
//#define TFT_PARALLEL_8_BIT

// Display type -  only define if RPi display
//#define RPI_DISPLAY_TYPE // 20MHz maximum SPI

// Only define one driver, the other ones must be commented out
#define ILI9341_DRIVER
//#define ST7735_DRIVER      // Define additional parameters below for this display
//#define ILI9163_DRIVER     // Define additional parameters below for this display
//#define S6D02A1_DRIVER
//#define RPI_ILI9486_DRIVER // 20MHz maximum SPI
//#define HX8357D_DRIVER
//#define ILI9481_DRIVER
//#define ILI9486_DRIVER
//#define ILI9488_DRIVER     // WARNING: Do not connect ILI9488 display SDO to MISO if other devices share the SPI bus (TFT SDO does NOT tristate when CS is high)
//#define ST7789_DRIVER      // Full configuration option, define additional parameters below for this display
//#define ST7789_2_DRIVER    // Minimal configuration option, define additional parameters below for this display
//#define R61581_DRIVER
//#define RM68140_DRIVER
//#define ST7796_DRIVER
//#define SSD1963_480_DRIVER
//#define SSD1963_800_DRIVER
//#define SSD1963_800ALT_DRIVER
//#define ILI9225_DRIVER

// Some displays support SPI reads via the MISO pin, other displays have a single
// bi-directional SDA pin and the library will try to read this via the MOSI line.
// To use the SDA line for reading data from the TFT uncomment the following line:

// #define TFT_SDA_READ      // This option is for ESP32 ONLY, tested with ST7789 display only

// For ST7735, ST7789 and ILI9341 ONLY, define the colour order IF the blue and red are swapped on your display
// Try ONE option at a time to find the correct colour order for your display

//  #define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue
//  #define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

// For ST7789, ST7735 and ILI9163 ONLY, define the pixel width and height in portrait orientation
// #define TFT_WIDTH  80
// #define TFT_WIDTH  128
// #define TFT_WIDTH  240 // ST7789 240 x 240 and 240 x 320
// #define TFT_HEIGHT 160
// #define TFT_HEIGHT 128
// #define TFT_HEIGHT 240 // ST7789 240 x 240
// #define TFT_HEIGHT 320 // ST7789 240 x 320

// For ST7735 ONLY, define the type of display, originally this was based on the
// colour of the tab on the screen protector film but this is not always true, so try
// out the different options below if the screen does not display graphics correctly,
// e.g. colours wrong, mirror images, or tray pixels at the edges.
// Comment out ALL BUT ONE of these options for a ST7735 display driver, save this
// this User_Setup file, then rebuild and upload the sketch to the board again:

// #define ST7735_INITB
// #define ST7735_GREENTAB
// #define ST7735_GREENTAB2
// #define ST7735_GREENTAB3
// #define ST7735_GREENTAB128    // For 128 x 128 display
// #define ST7735_GREENTAB160x80 // For 160 x 80 display (BGR, inverted, 26 offset)
// #define ST7735_REDTAB
// #define ST7735_BLACKTAB
// #define ST7735_REDTAB160x80   // For 160 x 80 display with 24 pixel offset

// If colours are inverted (white shows as black) then uncomment one of the next
// 2 lines try both options, one of the options should correct the inversion.

// #define TFT_INVERSION_ON
// #define TFT_INVERSION_OFF


// ##################################################################################
//
// Section 2. Define the pins that are used to interface with the display here
//
// ##################################################################################

// If a backlight control signal is available then define the TFT_BL pin in Section 2
// below. The backlight will be turned ON when tft.begin() is called, but the library
// needs to know if the LEDs are ON with the pin HIGH or LOW. If the LEDs are to be
// driven with a PWM signal or turned OFF/ON then this must be handled by the user
// sketch. e.g. with digitalWrite(TFT_BL, LOW);

// #define TFT_BL   32            // LED back-light control pin
// #define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)

// We must use hardware SPI, a minimum of 3 GPIO pins is needed.
// Typical setup for the RP2040 is :
//
// Display SDO/MISO  to RP2040 pin D0 (or leave disconnected if not reading TFT)
// Display LED       to RP2040 pin 3V3 or 5V
// Display SCK       to RP2040 pin D2
// Display SDI/MOSI  to RP2040 pin D3
// Display DC (RS/AO)to RP2040 pin D18 (can use another pin if desired)
// Display RESET     to RP2040 pin D19 (can use another pin if desired)
// Display CS        to RP2040 pin D20 (can use another pin if desired, or GND, see below)
// Display GND       to RP2040 pin GND (0V)
// Display VCC       to RP2040 5V or 3.3V (5v if display has a 5V to 3.3V regulator fitted)
//
// The DC (Data Command) pin may be labelled AO or RS (Register Select)
//
// With some displays such as the ILI9341 the TFT CS pin can be connected to GND if no more
// SPI devices (e.g. an SD Card) are connected, in this case comment out the #define TFT_CS
// line below so it is NOT defined. Other displays such at the ST7735 require the TFT CS pin
// to be toggled during setup, so in these cases the TFT_CS line must be defined and connected.

// The TFT_eSPI library always uses GPIO numbers, this means the following pin designators
// must be used dependant on the RP2040 board package loaded
// For the Arduino Mbed package the GPIO numbers must be used prefixed with p:
#ifdef ARDUINO_ARCH_MBED
  #define TFT_MISO   p4  // Arduino pin D12
  #define TFT_MOSI   p7  // Arduino pin D11
  #define TFT_SCLK   p6  // Arduino pin D13
  #define TFT_CS     p5  // Arduino pin D10
  #define TFT_DC    p20  // Arduino pin D8
  #define TFT_RST   p21  // Arduino pin D9
  #define TOUCH_CS  p19  // Arduino pin D7
#else // For Earle Philhower's package the Arduino Dx pin designations must be used:
  #define TFT_MISO  D12
  #define TFT_MOSI  D11
  #define TFT_SCLK  D13
  #define TFT_CS    D10  // Chip select control pin
  #define TFT_DC    D8   // Data Command control pin
  #define TFT_RST   D9   // Reset pin (could connect to Arduino RESET pin)
  #define TOUCH_CS  D7
#endif


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

// ##################################################################################
//
// Section 4. Other options
//
// ##################################################################################

// For the RP2040 processor define the SPI port channel used, default is 0
// #define TFT_SPI_PORT 1 // Set to 0 if SPI0 pins are used, or 1 if spi1 pins used

// Define the SPI clock frequency, this affects the graphics rendering speed. Too
// fast and the TFT driver will not keep up and display corruption appears.
// With an ILI9341 display 40MHz works OK, 80MHz sometimes fails
// With a ST7735 display more than 27MHz may not work (spurious pixels and lines)
// With an ILI9163 display 27 MHz works OK.

// #define SPI_FREQUENCY   1000000
// #define SPI_FREQUENCY   5000000
// #define SPI_FREQUENCY  10000000
// #define SPI_FREQUENCY  20000000
// #define SPI_FREQUENCY  32000000
 #define SPI_FREQUENCY  70000000

// Optional reduced SPI frequency for reading TFT
#define SPI_READ_FREQUENCY  20000000

// The XPT2046 requires a lower SPI clock rate of 2.5MHz so we define that here:
#define SPI_TOUCH_FREQUENCY  2500000

