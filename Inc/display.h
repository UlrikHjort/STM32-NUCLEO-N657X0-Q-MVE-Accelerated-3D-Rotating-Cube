/***************************************************************************
;  STM32N6 3.5inch Arduino 8BIT Module MAR3501 Display driver (write only)
;
;           Copyright (C) 2026 By Ulrik Hørlyk Hjort
;
; Permission is hereby granted, free of charge, to any person obtaining
; a copy of this software and associated documentation files (the
; "Software"), to deal in the Software without restriction, including
; without limitation the rights to use, copy, modify, merge, publish,
; distribute, sublicense, and/or sell copies of the Software, and to
; permit persons to whom the Software is furnished to do so, subject to
; the following conditions:
;
; The above copyright notice and this permission notice shall be
; included in all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
; EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
; MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
; NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
; LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
; OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
; WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
; ***************************************************************************/

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "gpio.h"
#include <stdint.h>

#define LCD_HEIGHT 320
#define LCD_WIDTH 480



#define LCD_CS_CLR GPIOA->BRR = LCD_CS_Pin
#define LCD_CS_SET GPIOA->BSRR = LCD_CS_Pin

#define LCD_RD_CLR GPIOF->BRR = LCD_RD_Pin
#define LCD_RD_SET GPIOF->BSRR = LCD_RD_Pin

#define LCD_RS_CLR GPIOF->BRR = LCD_RS_Pin
#define LCD_RS_SET GPIOF->BSRR = LCD_RS_Pin

#define LCD_RST_CLR GPIOC->BRR = LCD_RST_Pin
#define LCD_RST_SET GPIOC->BSRR = LCD_RST_Pin

#define LCD_WR_CLR GPIOC->BRR = LCD_WR_Pin
#define LCD_WR_SET GPIOC->BSRR = LCD_WR_Pin



void display_init();
void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color);


void lcd_draw_area(uint16_t start_x, uint16_t start_y, uint16_t width, uint16_t height, uint32_t *buffer);
void lcd_clear(uint16_t color, uint16_t width, uint16_t height);
void lcd_clear_area(uint16_t start_x, uint16_t start_y, uint16_t width, uint16_t height, uint16_t color);
void lcd_set_window(uint16_t x_start, uint16_t y_start,uint16_t x_end,uint16_t y_end);


static inline uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b) {
    return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}



#endif /* INC_DISPLAY_H_ */
