// Pico Explorer Base by Pimoroni (RP2040) (ST7789 on SPI bus with 240x240 TFT)
#define USER_SETUP_ID 138

#define ST7789_DRIVER // Configure all registers

#define TFT_WIDTH 240
#define TFT_HEIGHT 240

#define CGRAM_OFFSET // Library will add offsets required

// For Pico Explorer Base (PR2040)
#define TFT_CS 17  // Chip Select pin
#define TFT_DC 16  // Data Command control pin
#define TFT_RST -1 // No Reset pin
#define TFT_MOSI 19
#define TFT_SCLK 18

#define LOAD_FONT2
#define LOAD_FONT4

#define SPI_FREQUENCY 40000000
#define SPI_READ_FREQUENCY 20000000
#define SPI_TOUCH_FREQUENCY 2500000
