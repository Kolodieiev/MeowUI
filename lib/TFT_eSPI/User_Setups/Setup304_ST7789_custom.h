#define DISABLE_ALL_LIBRARY_WARNINGS // Вимкнути попередження бібліотеки.
#define USER_SETUP_ID 304
#define ST7789_DRIVER         // Configure all registers
#define TFT_RGB_ORDER TFT_BGR // Colour order Blue-Green-Red
// #define SPI_FREQUENCY 40000000
#define SPI_FREQUENCY 80000000
//---------------------------------------------------
#define TFT_WIDTH 240
#define TFT_HEIGHT 280

// #define TFT_WIDTH 128
// #define TFT_HEIGHT 128

// #define TFT_WIDTH 240
// #define TFT_HEIGHT 240

// ---------------------------------------------------
#define LOAD_FONT2
#define LOAD_FONT4

//---------------------------------------------------
// Generic ESP32 setup
// #define TFT_MOSI 23 // sda
// #define TFT_SCLK 18 // scl
// #define TFT_DC 4
// #define TFT_CS 27

// ESP32 S3
//  #define TFT_MOSI 11 // sda
//  #define TFT_SCLK 12 // scl
//  #define TFT_DC 42
//  #define TFT_CS 10

// Лілка
#define TFT_DC 15
#define TFT_CS 7

// #define TFT_RST  25      // TFT reset pin connect to NodeMCU RST, must also then add 10K pull down to TFT SCK
#define TFT_RST 1 // TFT reset pin connect to NodeMCU RST, must also then add 10K pull down to TFT SCK