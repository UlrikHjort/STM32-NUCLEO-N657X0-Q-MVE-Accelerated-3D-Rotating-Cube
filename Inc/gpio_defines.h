/***************************************************************************
;  STM32N6 3.5inch Arduino 8BIT Module MAR3501 GPIO mapping
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
#ifndef INC_GPIO_DEFINES_H_
#define INC_GPIO_DEFINES_H_

#define LCD_WR_Pin GPIO_PIN_10
#define LCD_WR_GPIO_Port GPIOC
#define LCD_D3_Pin GPIO_PIN_9
#define LCD_D3_GPIO_Port GPIOE
#define LCD_D5_Pin GPIO_PIN_10
#define LCD_D5_GPIO_Port GPIOE
#define LCD_RST_Pin GPIO_PIN_12
#define LCD_RST_GPIO_Port GPIOC
#define LCD_D0_Pin GPIO_PIN_12
#define LCD_D0_GPIO_Port GPIOD
#define LCD_D2_Pin GPIO_PIN_0
#define LCD_D2_GPIO_Port GPIOD
#define LCD_D6_Pin GPIO_PIN_5
#define LCD_D6_GPIO_Port GPIOD
#define LCD_D7_Pin GPIO_PIN_11
#define LCD_D7_GPIO_Port GPIOE
#define LCD_D1_Pin GPIO_PIN_7
#define LCD_D1_GPIO_Port GPIOD
#define LCD_D4_Pin GPIO_PIN_0
#define LCD_D4_GPIO_Port GPIOE
#define LCD_RS_Pin GPIO_PIN_6
#define LCD_RS_GPIO_Port GPIOF
#define LCD_RD_Pin GPIO_PIN_5
#define LCD_RD_GPIO_Port GPIOF
#define LCD_CS_Pin GPIO_PIN_2
#define LCD_CS_GPIO_Port GPIOA

#endif /* INC_GPIO_DEFINES_H_ */
