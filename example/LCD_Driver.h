#ifndef LCD_DRIVER
#define LCD_DRIVER

#include "misc.h"
#include "stm32l1xx.h"
#include "stm32l1xx_rcc.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_spi.h"
#include "stm32l1xx_tim.h"

#define CMD_CLEAR 0x04
#define CMD_WRITE_ROW 0x01
#define LCD_WIDTH 400
#define LCD_HEIGHT 240
#define LCD_WIDTH_BYTES LCD_WIDTH/8
#define LCD_DELAY 50000

#define BLACK 1
#define WHITE 0

void initGPIO();
void initTimer();
void initSPI();
void initTimer();
void SPISend(uint8_t value);
void delay(__IO uint32_t nCount);

void LCDInit();
void LCDStart();
void LCDStop();

void clearMem(int16_t color);
void drawPixel(int16_t x, int16_t y, uint16_t color);
void refresh();

#endif
