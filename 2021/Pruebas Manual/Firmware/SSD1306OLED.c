/******************************************************************************
 SSD1306 OLED driver for CCS PIC C compiler (SSD1306OLED.c)                   *
 Reference: Adafruit Industries SSD1306 OLED driver and graphics library.     *
                                                                              *
 The driver is for I2C mode only.                                             *
                                                                              *
 https://simple-circuit.com/                                                   *
                                                                              *
*******************************************************************************
*******************************************************************************
 This is a library for our Monochrome OLEDs based on SSD1306 drivers          *
                                                                              *
  Pick one up today in the adafruit shop!                                     *
  ------> http://www.adafruit.com/category/63_98                              *
                                                                              *
 Adafruit invests time and resources providing this open source code,         *
 please support Adafruit and open-source hardware by purchasing               *
 products from Adafruit!                                                      *
                                                                              *
 Written by Limor Fried/Ladyada  for Adafruit Industries.                     *
 BSD license, check license.txt for more information                          *
 All text above, and the splash screen must be included in any redistribution *
*******************************************************************************/



#include <stdint.h>

//------------------------------ Definitions ---------------------------------//

#ifndef SSD1306_I2C_ADDRESS
  #define SSD1306_I2C_ADDRESS   0x7a

#if !defined SSD1306_128_32 && !defined SSD1306_96_16
#define SSD1306_128_64
#endif
#if defined SSD1306_128_32 && defined SSD1306_96_16
  #error "Only one SSD1306 display can be specified at once"
#endif

#if defined SSD1306_128_64
  #define SSD1306_LCDWIDTH           128
  #define SSD1306_LCDHEIGHT           64
#endif
#if defined SSD1306_128_32
  #define SSD1306_LCDWIDTH            128
  #define SSD1306_LCDHEIGHT            32
#endif
#if defined SSD1306_96_16
  #define SSD1306_LCDWIDTH             96
  #define SSD1306_LCDHEIGHT            16
#endif

#define SSD1306_SETCONTRAST          0x81
#define SSD1306_DISPLAYALLON_RESUME  0xA4
#define SSD1306_DISPLAYALLON         0xA5
#define SSD1306_NORMALDISPLAY        0xA6
#define SSD1306_INVERTDISPLAY_       0xA7
#define SSD1306_DISPLAYOFF           0xAE
#define SSD1306_DISPLAYON            0xAF
#define SSD1306_SETDISPLAYOFFSET     0xD3
#define SSD1306_SETCOMPINS           0xDA
#define SSD1306_SETVCOMDETECT        0xDB
#define SSD1306_SETDISPLAYCLOCKDIV   0xD5
#define SSD1306_SETPRECHARGE         0xD9
#define SSD1306_SETMULTIPLEX         0xA8
#define SSD1306_SETLOWCOLUMN         0x00
#define SSD1306_SETHIGHCOLUMN        0x10
#define SSD1306_SETSTARTLINE         0x40
#define SSD1306_MEMORYMODE           0x20
#define SSD1306_COLUMNADDR           0x21
#define SSD1306_PAGEADDR             0x22
#define SSD1306_COMSCANINC           0xC0
#define SSD1306_COMSCANDEC           0xC8
#define SSD1306_SEGREMAP             0xA0
#define SSD1306_CHARGEPUMP           0x8D
#define SSD1306_EXTERNALVCC          0x01
#define SSD1306_SWITCHCAPVCC         0x02
#define DAT                          0x60

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL                      0x2F
#define SSD1306_DEACTIVATE_SCROLL                    0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A

#define ssd1306_swap(a, b) { int16 t = a; a = b; b = t; }

int8 _i2caddr, _vccstate, x_pos, y_pos, text_size;
int1 wrap = TRUE;

void ssd1306_command(int8 c);
void SSD1306_Begin(int8 vccstate = SSD1306_SWITCHCAPVCC, int8 i2caddr = SSD1306_I2C_ADDRESS);
void SSD1306_DrawPixel(int16 x, int16 y, int1 color = TRUE);
void SSD1306_StartScrollRight(int8 start, int8 stop);
void SSD1306_StartScrollLeft(int8 start, int8 stop);
void SSD1306_StartScrollDiagRight(int8 start, int8 stop);
void SSD1306_StartScrollDiagLeft(int8 start, int8 stop);
void SSD1306_StopScroll(void);
void SSD1306_Dim(int1 dim);
void SSD1306_Display(void);
void SSD1306_ClearDisplay(void);
void SSD1306_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int1 color = TRUE);
void SSD1306_DrawFastHLine(uint8_t x, uint8_t y, uint8_t w, int1 color = TRUE);
void SSD1306_DrawFastVLine(uint8_t x, uint8_t y, uint8_t h, int1 color = TRUE);
void SSD1306_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, int1 color = TRUE);
void SSD1306_FillScreen(int1 color = TRUE);
void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r);
void SSD1306_DrawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername);
void SSD1306_FillCircle(int16_t x0, int16_t y0, int16_t r, int1 color = TRUE);
void SSD1306_FillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, int1 color = TRUE);
void SSD1306_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void SSD1306_DrawRoundRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r);
void SSD1306_FillRoundRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r, int1 color = TRUE);
void SSD1306_DrawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void SSD1306_FillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int1 color = TRUE);
void SSD1306_DrawChar(uint8_t x, uint8_t y, uint8_t c, uint8_t size = 1);
void SSD1306_DrawText(uint8_t x, uint8_t y, char *_text, uint8_t size = 1);
void SSD1306_TextSize(uint8_t t_size);
void SSD1306_GotoXY(uint8_t x, uint8_t y);
void SSD1306_Print(uint8_t c);
void SSD1306_PutCustomC(rom uint8_t *c);
void SSD1306_SetTextWrap(int1 w);
void SSD1306_InvertDisplay(int1 i);
void SSD1306_DrawBMP(uint8_t x, uint8_t y, rom uint8_t *bitmap, uint8_t w, uint8_t h);

//--------------------------------------------------------------------------//

const char Font[] = {
0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x5F, 0x00, 0x00,
0x00, 0x07, 0x00, 0x07, 0x00,
0x14, 0x7F, 0x14, 0x7F, 0x14,
0x24, 0x2A, 0x7F, 0x2A, 0x12,
0x23, 0x13, 0x08, 0x64, 0x62,
0x36, 0x49, 0x56, 0x20, 0x50,
0x00, 0x08, 0x07, 0x03, 0x00,
0x00, 0x1C, 0x22, 0x41, 0x00,
0x00, 0x41, 0x22, 0x1C, 0x00,
0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
0x08, 0x08, 0x3E, 0x08, 0x08,
0x00, 0x80, 0x70, 0x30, 0x00,
0x08, 0x08, 0x08, 0x08, 0x08,
0x00, 0x00, 0x60, 0x60, 0x00,
0x20, 0x10, 0x08, 0x04, 0x02,
0x3E, 0x51, 0x49, 0x45, 0x3E,
0x00, 0x42, 0x7F, 0x40, 0x00,
0x72, 0x49, 0x49, 0x49, 0x46,
0x21, 0x41, 0x49, 0x4D, 0x33,
0x18, 0x14, 0x12, 0x7F, 0x10,
0x27, 0x45, 0x45, 0x45, 0x39,
0x3C, 0x4A, 0x49, 0x49, 0x31,
0x41, 0x21, 0x11, 0x09, 0x07,
0x36, 0x49, 0x49, 0x49, 0x36,
0x46, 0x49, 0x49, 0x29, 0x1E,
0x00, 0x00, 0x14, 0x00, 0x00,
0x00, 0x40, 0x34, 0x00, 0x00,
0x00, 0x08, 0x14, 0x22, 0x41,
0x14, 0x14, 0x14, 0x14, 0x14,
0x00, 0x41, 0x22, 0x14, 0x08,
0x02, 0x01, 0x59, 0x09, 0x06,
0x3E, 0x41, 0x5D, 0x59, 0x4E,
0x7C, 0x12, 0x11, 0x12, 0x7C,
0x7F, 0x49, 0x49, 0x49, 0x36,
0x3E, 0x41, 0x41, 0x41, 0x22,
0x7F, 0x41, 0x41, 0x41, 0x3E,
0x7F, 0x49, 0x49, 0x49, 0x41,
0x7F, 0x09, 0x09, 0x09, 0x01,
0x3E, 0x41, 0x41, 0x51, 0x73,
0x7F, 0x08, 0x08, 0x08, 0x7F,
0x00, 0x41, 0x7F, 0x41, 0x00,
0x20, 0x40, 0x41, 0x3F, 0x01,
0x7F, 0x08, 0x14, 0x22, 0x41,
0x7F, 0x40, 0x40, 0x40, 0x40,
0x7F, 0x02, 0x1C, 0x02, 0x7F,
0x7F, 0x04, 0x08, 0x10, 0x7F,
0x3E, 0x41, 0x41, 0x41, 0x3E,
0x7F, 0x09, 0x09, 0x09, 0x06,
0x3E, 0x41, 0x51, 0x21, 0x5E,
0x7F, 0x09, 0x19, 0x29, 0x46
};
const char Font2[] = {
0x26, 0x49, 0x49, 0x49, 0x32,
0x03, 0x01, 0x7F, 0x01, 0x03,
0x3F, 0x40, 0x40, 0x40, 0x3F,
0x1F, 0x20, 0x40, 0x20, 0x1F,
0x3F, 0x40, 0x38, 0x40, 0x3F,
0x63, 0x14, 0x08, 0x14, 0x63,
0x03, 0x04, 0x78, 0x04, 0x03,
0x61, 0x59, 0x49, 0x4D, 0x43,
0x00, 0x7F, 0x41, 0x41, 0x41,
0x02, 0x04, 0x08, 0x10, 0x20,
0x00, 0x41, 0x41, 0x41, 0x7F,
0x04, 0x02, 0x01, 0x02, 0x04,
0x40, 0x40, 0x40, 0x40, 0x40,
0x00, 0x03, 0x07, 0x08, 0x00,
0x20, 0x54, 0x54, 0x78, 0x40,
0x7F, 0x28, 0x44, 0x44, 0x38,
0x38, 0x44, 0x44, 0x44, 0x28,
0x38, 0x44, 0x44, 0x28, 0x7F,
0x38, 0x54, 0x54, 0x54, 0x18,
0x00, 0x08, 0x7E, 0x09, 0x02,
0x18, 0xA4, 0xA4, 0x9C, 0x78,
0x7F, 0x08, 0x04, 0x04, 0x78,
0x00, 0x44, 0x7D, 0x40, 0x00,
0x20, 0x40, 0x40, 0x3D, 0x00,
0x7F, 0x10, 0x28, 0x44, 0x00,
0x00, 0x41, 0x7F, 0x40, 0x00,
0x7C, 0x04, 0x78, 0x04, 0x78,
0x7C, 0x08, 0x04, 0x04, 0x78,
0x38, 0x44, 0x44, 0x44, 0x38,
0xFC, 0x18, 0x24, 0x24, 0x18,
0x18, 0x24, 0x24, 0x18, 0xFC,
0x7C, 0x08, 0x04, 0x04, 0x08,
0x48, 0x54, 0x54, 0x54, 0x24,
0x04, 0x04, 0x3F, 0x44, 0x24,
0x3C, 0x40, 0x40, 0x20, 0x7C,
0x1C, 0x20, 0x40, 0x20, 0x1C,
0x3C, 0x40, 0x30, 0x40, 0x3C,
0x44, 0x28, 0x10, 0x28, 0x44,
0x4C, 0x90, 0x90, 0x90, 0x7C,
0x44, 0x64, 0x54, 0x4C, 0x44,
0x00, 0x08, 0x36, 0x41, 0x00,
0x00, 0x00, 0x77, 0x00, 0x00,
0x00, 0x41, 0x36, 0x08, 0x00,
0x02, 0x01, 0x02, 0x04, 0x02
};

static int8 ssd1306_buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8]  = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xE0, 0xE0,
0xE0, 0xE0, 0x60, 0x00, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0xE0, 0xE0, 0xE0,
0xE0, 0xE0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0x3F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xF0, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0x7F, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0x7F, 0x0F, 0x01,
0x00, 0x00, 0xE0, 0xF8, 0xFB, 0x1B, 0xE3, 0xE0, 0xE3, 0x63, 0x63, 0x63, 0x63, 0xE3, 0xE3, 0xE3,
0xE3, 0x83, 0xC3, 0xE3, 0x63, 0x63, 0x63, 0x63, 0x63, 0xE3, 0xE3, 0xE3, 0x63, 0x83, 0xF3, 0xFB,
0x39, 0xC1, 0xE0, 0xE0, 0x60, 0x60, 0x60, 0x60, 0x60, 0xE0, 0xE0, 0xE0, 0x00, 0xE0, 0xF8, 0xF8,
0x78, 0x60, 0x60, 0x60, 0x60, 0xE0, 0xE0, 0xE0, 0x00, 0x00, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60,
0x60, 0xE0, 0xE0, 0xC0, 0x00, 0xF0, 0xF8, 0x78, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1C, 0x1F, 0x1F, 0x1F, 0x0F, 0x03, 0x00, 0x00, 0x00,
0x10, 0x1E, 0x1F, 0x07, 0x18, 0x1F, 0x1F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x18, 0x1F, 0x1F, 0x07,
0x18, 0x1F, 0x1F, 0x1B, 0x18, 0x18, 0x98, 0xF8, 0xF8, 0x7F, 0x1F, 0x03, 0x1C, 0x1F, 0x1F, 0x01,
0x1E, 0x1F, 0x1F, 0x19, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x1F, 0x0F, 0x11, 0x1E, 0x1F, 0x1F, 0x18,
0x18, 0x18, 0x18, 0x18, 0x1E, 0x1F, 0x07, 0x08, 0x1E, 0x1F, 0x1F, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B,
0x1F, 0x1F, 0x07, 0x18, 0x1F, 0x1F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void ssd1306_command(int8 c)
{
    uint8_t control = 0x00;   // Co = 0, D/C = 0
    i2c_Start(SSD1306_STREAM);
    i2c_Write(SSD1306_STREAM, _i2caddr);
    i2c_Write(SSD1306_STREAM, control);
    i2c_Write(SSD1306_STREAM, c);
    i2c_Stop(SSD1306_STREAM);
}

void SSD1306_Begin(int8 vccstate = SSD1306_SWITCHCAPVCC, int8 i2caddr = SSD1306_I2C_ADDRESS)
{
  _vccstate = vccstate;
  _i2caddr = i2caddr;

    // I2C Init

   #if defined SSD1306_128_32
    // Init sequence for 128x32 OLED module
    SSD1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    SSD1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    SSD1306_command(0x80);                                  // the suggested ratio 0x80
    SSD1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    SSD1306_command(0x1F);
    SSD1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    SSD1306_command(0x0);                                   // no offset
    SSD1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0 
    SSD1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC) 
      { SSD1306_command(0x10); }
    else 
      { SSD1306_command(0x14); }
    SSD1306_command(SSD1306_MEMORYMODE);                    // 0x20
    SSD1306_command(0x00);                                  // 0x0 act like ks0108
    SSD1306_command(SSD1306_SEGREMAP | 0x1);
    SSD1306_command(SSD1306_COMSCANDEC);
    SSD1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    SSD1306_command(0x02);
    SSD1306_command(SSD1306_SETCONTRAST);                   // 0x81
    SSD1306_command(0x8F);
    SSD1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC) 
      { SSD1306_command(0x22); }
    else 
      { SSD1306_command(0xF1); }
    SSD1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    SSD1306_command(0x40);
    SSD1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    SSD1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
  #endif

  #if defined SSD1306_128_64
    // Init sequence for 128x64 OLED module
    SSD1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    SSD1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    SSD1306_command(0x80);                                  // the suggested ratio 0x80
    SSD1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    SSD1306_command(0x3F);
    SSD1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    SSD1306_command(0x00);                                   // no offset
   
    SSD1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0 0x40
    SSD1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC) 
      { SSD1306_command(0x10); }
    else 
      { SSD1306_command(0x14); }
    SSD1306_command(SSD1306_MEMORYMODE);                    // 0x20
    SSD1306_command(0x00);                                  // 0x0 act like ks0108
    SSD1306_command(SSD1306_SEGREMAP | 0x1);
    SSD1306_command(SSD1306_COMSCANDEC);
    SSD1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    SSD1306_command(0x12);
    SSD1306_command(SSD1306_SETCONTRAST);                   // 0x81
    if (vccstate == SSD1306_EXTERNALVCC) 
      { SSD1306_command(0x9F); }
    else 
      { SSD1306_command(0xCF); }
    SSD1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC) 
      { SSD1306_command(0x22); }
    else 
      { SSD1306_command(0xF1); }
    SSD1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    SSD1306_command(0x40);
    SSD1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    SSD1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
  #endif

  SSD1306_command(SSD1306_DISPLAYON);//--turn on oled panel
}

void SSD1306_DrawPixel(int16 x, int16 y, int1 color = TRUE)
{
    if ((x < 0) || (x >= SSD1306_LCDWIDTH) || (y < 0) || (y >= SSD1306_LCDHEIGHT))
    return;

  if (color)
    ssd1306_buffer[x + (y/8) * SSD1306_LCDWIDTH] |=  (1 << (y & 7));
  else
    ssd1306_buffer[x + (y/8) * SSD1306_LCDWIDTH] &=  ~(1 << (y & 7));
}

void SSD1306_StartScrollRight(uint8_t start, uint8_t stop)
{
  ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X00);
  ssd1306_command(0XFF);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_StartScrollLeft(uint8_t start, uint8_t stop)
{
  ssd1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X00);
  ssd1306_command(0XFF);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_StartScrollDiagRight(uint8_t start, uint8_t stop)
{
  ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
  ssd1306_command(0X00);
  ssd1306_command(SSD1306_LCDHEIGHT);
  ssd1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X01);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_StartScrollDiagLeft(uint8_t start, uint8_t stop)
{
  ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
  ssd1306_command(0X00);
  ssd1306_command(SSD1306_LCDHEIGHT);
  ssd1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X01);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_StopScroll(void)
{
  ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
}

void SSD1306_Dim(int1 dim)
{
  uint8_t contrast;
  if (dim)
    contrast = 0; // Dimmed display
  else {
    if (_vccstate == SSD1306_EXTERNALVCC)
      contrast = 0x9F;
    else
      contrast = 0xCF;
  }
  // the range of contrast to too small to be really useful
  // it is useful to dim the display
  ssd1306_command(SSD1306_SETCONTRAST);
  ssd1306_command(contrast);
}

void SSD1306_Display(void)
{
   ssd1306_command(ssd1306_SETLOWCOLUMN | 0x0);  // low col = 0
   ssd1306_command(ssd1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
   ssd1306_command(ssd1306_SETSTARTLINE | 0x0); // line #0
  
   byte height=64;
   byte width=128; 
   byte m_row = 0;
   byte m_col = 2;
   
   height >>= 3;
   width >>= 3;
   //Serial.println(width);
   
   int16 p = 0;
   
   byte i, j, k =0;
   
   for ( i = 0; i < height; i++) {
      // send a bunch of data in one xmission
        ssd1306_command(0xB0 + i + m_row);//set page address
        ssd1306_command(m_col & 0xf);//set lower column address
        ssd1306_command(0x10 | (m_col >> 4));//set higher column address
      
        for( j = 0; j < 8; j++){        
         i2c_Start(SSD1306_STREAM);
         i2c_Write(SSD1306_STREAM, _i2caddr);
         i2c_Write(0x40);
            for ( k = 0; k < width; k++) {
               i2c_Write(SSD1306_STREAM, ssd1306_buffer[p]);
               p++;
            }
            i2c_Stop(SSD1306_STREAM);
        }
   }
}

void SSD1306_ClearDisplay(void)
{
   int16 i;
  for (i = 0; i < (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT / 8); i++)
    ssd1306_buffer[i] = 0;
}

void SSD1306_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int1 color = TRUE)
{
  int1 steep;
  int8_t ystep;
  uint8_t dx, dy;
  int16_t err;
  steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    ssd1306_swap(x0, y0);
    ssd1306_swap(x1, y1);
  }
  if (x0 > x1) {
    ssd1306_swap(x0, x1);
    ssd1306_swap(y0, y1);
  }
  dx = x1 - x0;
  dy = abs(y1 - y0);

  err = dx / 2;
  if (y0 < y1)
    ystep = 1;
  else
    ystep = -1;

  for (; x0 <= x1; x0++) {
    if (steep) {
      if(color) SSD1306_DrawPixel(y0, x0);
      else      SSD1306_DrawPixel(y0, x0, FALSE);
    }
    else {
      if(color) SSD1306_DrawPixel(x0, y0);
      else      SSD1306_DrawPixel(x0, y0, FALSE);
    }
    err -= dy;
    if (err < 0) {
      y0  += ystep;
      err += dx;
    }
  }
}

void SSD1306_DrawFastHLine(uint8_t x, uint8_t y, uint8_t w, int1 color = TRUE)
{
   SSD1306_DrawLine(x, y, x + w - 1, y, color);
}

void SSD1306_DrawFastVLine(uint8_t x, uint8_t y, uint8_t h, int1 color = TRUE)
{
  SSD1306_DrawLine(x, y, x, y + h - 1, color);
}

void SSD1306_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, int1 color = TRUE)
{
   int16 i;
  for (i = x; i < x + w; i++)
    SSD1306_DrawFastVLine(i, y, h, color);
}

void SSD1306_FillScreen(int1 color = TRUE) {
  SSD1306_FillRect(0, 0, SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT, color);
}

void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  SSD1306_DrawPixel(x0  , y0 + r);
  SSD1306_DrawPixel(x0  , y0 - r);
  SSD1306_DrawPixel(x0 + r, y0);
  SSD1306_DrawPixel(x0 - r, y0);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    SSD1306_DrawPixel(x0 + x, y0 + y);
    SSD1306_DrawPixel(x0 - x, y0 + y);
    SSD1306_DrawPixel(x0 + x, y0 - y);
    SSD1306_DrawPixel(x0 - x, y0 - y);
    SSD1306_DrawPixel(x0 + y, y0 + x);
    SSD1306_DrawPixel(x0 - y, y0 + x);
    SSD1306_DrawPixel(x0 + y, y0 - x);
    SSD1306_DrawPixel(x0 - y, y0 - x);
  }

}

void SSD1306_DrawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      SSD1306_DrawPixel(x0 + x, y0 + y);
      SSD1306_DrawPixel(x0 + y, y0 + x);
    }
    if (cornername & 0x2) {
      SSD1306_DrawPixel(x0 + x, y0 - y);
      SSD1306_DrawPixel(x0 + y, y0 - x);
    }
    if (cornername & 0x8) {
      SSD1306_DrawPixel(x0 - y, y0 + x);
      SSD1306_DrawPixel(x0 - x, y0 + y);
    }
    if (cornername & 0x1) {
      SSD1306_DrawPixel(x0 - y, y0 - x);
      SSD1306_DrawPixel(x0 - x, y0 - y);
    }
  }

}

void SSD1306_FillCircle(int16_t x0, int16_t y0, int16_t r, int1 color = TRUE)
{
  SSD1306_DrawFastVLine(x0, y0 - r, 2 * r + 1, color);
  SSD1306_FillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void SSD1306_FillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, int1 color = TRUE) {
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x01) {
      SSD1306_DrawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
      SSD1306_DrawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
    }
    if (cornername & 0x02) {
      SSD1306_DrawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
      SSD1306_DrawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
    }
  }

}

// Draw a rectangle
void SSD1306_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  SSD1306_DrawFastHLine(x, y, w);
  SSD1306_DrawFastHLine(x, y + h - 1, w);
  SSD1306_DrawFastVLine(x, y, h);
  SSD1306_DrawFastVLine(x + w - 1, y, h);
}

// Draw a rounded rectangle
void SSD1306_DrawRoundRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r)
{
  // smarter version
  SSD1306_DrawFastHLine(x + r, y, w - 2 * r); // Top
  SSD1306_DrawFastHLine(x + r, y + h - 1, w - 2 * r); // Bottom
  SSD1306_DrawFastVLine(x, y + r, h - 2 * r); // Left
  SSD1306_DrawFastVLine(x + w - 1, y + r, h - 2 * r); // Right
  // draw four corners
  SSD1306_DrawCircleHelper(x + r, y + r, r, 1);
  SSD1306_DrawCircleHelper(x + w - r - 1, y + r, r, 2);
  SSD1306_DrawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4);
  SSD1306_DrawCircleHelper(x + r, y + h - r - 1, r, 8);
}

// Fill a rounded rectangle
void SSD1306_FillRoundRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r, int1 color = TRUE)
{
  // smarter version
  SSD1306_FillRect(x + r, y, w - 2 * r, h, color);
  // draw four corners
  SSD1306_FillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
  SSD1306_FillCircleHelper(x + r        , y + r, r, 2, h - 2 * r - 1, color);
}

// Draw a triangle
void SSD1306_DrawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
  SSD1306_DrawLine(x0, y0, x1, y1);
  SSD1306_DrawLine(x1, y1, x2, y2);
  SSD1306_DrawLine(x2, y2, x0, y0);
}

// Fill a triangle
void SSD1306_FillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int1 color = TRUE)
{
  int16_t a, b, y, last;
  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    ssd1306_swap(y0, y1); ssd1306_swap(x0, x1);
  }
  if (y1 > y2) {
    ssd1306_swap(y2, y1); ssd1306_swap(x2, x1);
  }
  if (y0 > y1) {
    ssd1306_swap(y0, y1); ssd1306_swap(x0, x1);
  }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    SSD1306_DrawFastHLine(a, y0, b - a + 1, color);
    return;
  }

  int16_t
  dx01 = x1 - x0,
  dy01 = y1 - y0,
  dx02 = x2 - x0,
  dy02 = y2 - y0,
  dx12 = x2 - x1,
  dy12 = y2 - y1;
  int32_t  sa   = 0, sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1 - 1; // Skip it

  for(y = y0; y <= last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) ssd1306_swap(a, b);
    SSD1306_DrawFastHLine(a, y, b - a + 1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y <= y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) ssd1306_swap(a, b);
    SSD1306_DrawFastHLine(a, y, b - a + 1, color);
  }
}

// invert the display
void SSD1306_InvertDisplay(int1 i)
{
  if (i)
    ssd1306_command(SSD1306_INVERTDISPLAY_);
  else
    ssd1306_command(SSD1306_NORMALDISPLAY);
}

void SSD1306_SetTextWrap(int1 w)
{
  wrap = w;
}

void SSD1306_DrawChar(uint8_t x, uint8_t y, uint8_t c, uint8_t size = 1)
{
  SSD1306_GotoXY(x, y);
  SSD1306_TextSize(size);
  SSD1306_Print(c);
}

void SSD1306_DrawText(uint8_t x, uint8_t y, char *_text, uint8_t size = 1)
{
  SSD1306_GotoXY(x, y);
  SSD1306_TextSize(size);
  while(*_text != '\0')
    SSD1306_Print(*_text++);

}

// move cursor to position (x, y)
void SSD1306_GotoXY(uint8_t x, uint8_t y)
{
  if((x >= SSD1306_LCDWIDTH) || (y >= SSD1306_LCDHEIGHT))
    return;
  x_pos = x;
  y_pos = y;
}

// set text size
void SSD1306_TextSize(uint8_t t_size)
{
  if(t_size < 1)
    t_size = 1;
  text_size = t_size;
}

/* print single char
    \a  Set cursor position to upper left (0, 0)
    \b  Move back one position
    \n  Go to start of current line
    \r  Go to line below
*/
void SSD1306_Print(uint8_t c)
{
  int1 _color;
  uint8_t i, j, line;
  
  if (c == ' ' && x_pos == 0 && wrap)
    return;
  if(c == '\a') {
    x_pos = y_pos = 0;
    return;
  }
  if( (c == '\b') && (x_pos >= text_size * 6) ) {
    x_pos -= text_size * 6;
    return;
  }
  if(c == '\r') {
    x_pos = 0;
    return;
  }
  if(c == '\n') {
    y_pos += text_size * 8;
    if((y_pos + text_size * 7) > SSD1306_LCDHEIGHT)
      y_pos = 0;
    return;
  }

  if((c < ' ') || (c > '~'))
    c = '?';
  
  for(i = 0; i < 5; i++ ) {
    if(c < 'S')
      line = font[(c - ' ') * 5 + i];
    else
      line = font2[(c - 'S') * 5 + i];
    
    for(j = 0; j < 7; j++, line >>= 1) {
      if(line & 0x01)
        _color = TRUE;
      else
        _color = FALSE;
      if(text_size == 1) SSD1306_DrawPixel(x_pos + i, y_pos + j, _color);
      else               SSD1306_FillRect(x_pos + (i * text_size), y_pos + (j * text_size), text_size, text_size, _color);
    }
  }

  SSD1306_FillRect(x_pos + (5 * text_size), y_pos, text_size, 7 * text_size, FALSE);
  
  x_pos += text_size * 6;

  if( x_pos > (SSD1306_LCDWIDTH + text_size * 6) )
    x_pos = SSD1306_LCDWIDTH;

  if (wrap && (x_pos + (text_size * 5)) > SSD1306_LCDWIDTH)
  {
    x_pos = 0;
    y_pos += text_size * 8;
    if((y_pos + text_size * 7) > SSD1306_LCDHEIGHT)
      y_pos = 0;
  }
}

// print custom char (dimension: 7x5 pixel)
void SSD1306_PutCustomC(rom uint8_t *c)
{
  int1 _color;
  uint8_t i, j, line;
  
  for(i = 0; i < 5; i++ ) {
    line = c[i];

    for(j = 0; j < 7; j++, line >>= 1) {
      if(line & 0x01)
        _color = TRUE;
      else
        _color = FALSE;
      if(text_size == 1) SSD1306_DrawPixel(x_pos + i, y_pos + j, _color);
      else               SSD1306_FillRect(x_pos + (i * text_size), y_pos + (j * text_size), text_size, text_size, _color);
    }
  }

  SSD1306_FillRect(x_pos + (5 * text_size), y_pos, text_size, 7 * text_size, FALSE);

  x_pos += (text_size * 6);

  if( x_pos > (SSD1306_LCDWIDTH + text_size * 6) )
    x_pos = SSD1306_LCDWIDTH;

  if (wrap && (x_pos + (text_size * 5)) > SSD1306_LCDWIDTH)
  {
    x_pos = 0;
    y_pos += text_size * 8;
    if((y_pos + text_size * 7) > SSD1306_LCDHEIGHT)
      y_pos = 0;
  }
}

// draw BMP stored in ROM
void SSD1306_ROMBMP(uint8_t x, uint8_t y, rom uint8_t *bitmap, uint8_t w, uint8_t h)
{
   int16 i,j;
  for( i = 0; i < h/8; i++ )
  {    
    for( j = 0; j < (uint16_t)w * 8; j++ )
    {      
      if( bit_test(bitmap[j/8 + i*w], j % 8) == 1 )
        SSD1306_DrawPixel(x + j/8, y + i*8 + (j % 8));
      else
        SSD1306_DrawPixel(x + j/8, y + i*8 + (j % 8), 0);  
    }
  }
}

void OLED_print_Image(unsigned char bmp, unsigned char pixel)
{
    unsigned char x_pos = 0;
    unsigned char page = 0;

    if(pixel != 0)
    {
        pixel = 0xFF;
    }
    else
    {
        pixel = 0x00;
    }

    for(page = 0; page < 8; page++)
    {
         SSD1306_GotoXY(0, page);

        I2C_Start();
        I2C_Write(SSD1306_I2C_Address);
        I2C_Write(DAT);

         for(x_pos = 0; x_pos < 128; x_pos++)
         {
           I2C_Write((bmp++ ^ pixel));
         }

        I2C_Stop();
     }
}

// end of driver code.
