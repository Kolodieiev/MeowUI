#define USER_SETUP_ID 52

#define ST7796_DRIVER

#define TFT_BL   12				// LED back-light control pin
#define TFT_BACKLIGHT_ON HIGH	// Level to turn ON back-light (HIGH or LOW)

#define ESP32_DMA
#define TFT_MISO 23
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS   5   // Chip select control pin
#define TFT_DC   27  // Data Command control pin
#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST


#define LOAD_FONT2  
#define LOAD_FONT4 



#define SPI_FREQUENCY		40000000
#define SPI_READ_FREQUENCY  12000000
