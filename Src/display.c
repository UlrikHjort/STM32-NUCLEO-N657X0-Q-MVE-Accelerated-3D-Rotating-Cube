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
#include <stdint.h>
#include "main.h"
#include "display.h"

// --- Bus masks for clearing (bits on ports for all data pins) ---
static const volatile uint32_t MASK_D_CLEAR = ((1u << 12) | (1u << 7)
		| (1u << 0) | (1u << 5)); // GPIOD bits used
static const volatile uint32_t MASK_E_CLEAR = ((1u << 9) | (1u << 0)
		| (1u << 10) | (1u << 11)); // GPIOE bits used

// Lookup tables (32-bit)
static volatile uint32_t LUT_mask_D[256];
static volatile uint32_t LUT_mask_E[256];

// Small inline NOP-based delay to extend WR low time (compiler may optimize, but NOPs usually stay)
static inline void short_delay_nops(void) {
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
}

// WR pulse (guarantee WR low pulse and proper CS hold)
static inline void LCD_WR_Pulse(void) {
	// Ensure WR is high before starting (safe)
	LCD_WR_SET;
	// pull low
	LCD_WR_CLR;
	// keep low for a few cycles
	short_delay_nops();
	// release
	LCD_WR_SET;
}

// write raw bus: clear then set bits for the 8-bit value
static inline void write_data_bus(uint8_t data) {
	// Clear D pins (use BSRR upper half to reset)
	GPIOD->BSRR = (MASK_D_CLEAR << 16); // reset those pins on GPIOD
	GPIOE->BSRR = (MASK_E_CLEAR << 16); // reset those pins on GPIOE

	// Set pins according to LUT (lower 16 bits set pins)
	GPIOD->BSRR = LUT_mask_D[data];
	GPIOE->BSRR = LUT_mask_E[data];
}


void LCD_WR_CMD(uint8_t cmd) {
	LCD_CS_CLR;
	LCD_RS_CLR;            // command
	write_data_bus(cmd);
	LCD_WR_Pulse();
	LCD_CS_SET;
}

void LCD_WR_DATA(uint8_t data) {
	LCD_CS_CLR;
	LCD_RS_SET;            // data
	write_data_bus(data);
	LCD_WR_Pulse();
	LCD_CS_SET;
}

void LCD_WR_DATA16(uint16_t val) {
	// send high byte then low byte
	LCD_WR_DATA((uint8_t) (val >> 8));
	LCD_WR_DATA((uint8_t) (val & 0xFF));
}

// Precompute LUTs mapping 8-bit value bits to actual port BSRR set masks
void precompute_masks(void) {
	for (int v = 0; v < 256; v++) {
		uint32_t d = 0;
		uint32_t e = 0;

		// Map LCD D0..D7 to MCU pins
		if (v & (1 << 0))
			d |= (1u << 12);  // D0 -> PD12
		if (v & (1 << 1))
			d |= (1u << 7);   // D1 -> PD7
		if (v & (1 << 2))
			d |= (1u << 0);   // D2 -> PD0
		if (v & (1 << 6))
			d |= (1u << 5);   // D6 -> PD5

		if (v & (1 << 3))
			e |= (1u << 9);   // D3 -> PE9
		if (v & (1 << 4))
			e |= (1u << 0);   // D4 -> PE0
		if (v & (1 << 5))
			e |= (1u << 10);  // D5 -> PE10
		if (v & (1 << 7))
			e |= (1u << 11);  // D7 -> PE11

		LUT_mask_D[v] = d;
		LUT_mask_E[v] = e;
	}
}

// Reset with longer post-high delay
void LCD_RESET(void) {
	LCD_RST_CLR;
	HAL_Delay(100);
	LCD_RST_SET;
	HAL_Delay(150);
}

// Basic set window (16-bit coordinates)
void lcd_set_window(uint16_t x_start, uint16_t y_start, uint16_t x_end,
		uint16_t y_end) {
	// Column addr
	LCD_WR_CMD(0x2A);
	LCD_WR_DATA((uint8_t) (x_start >> 8));
	LCD_WR_DATA((uint8_t) (x_start & 0xFF));
	LCD_WR_DATA((uint8_t) (x_end >> 8));
	LCD_WR_DATA((uint8_t) (x_end & 0xFF));

	// Row addr
	LCD_WR_CMD(0x2B);
	LCD_WR_DATA((uint8_t) (y_start >> 8));
	LCD_WR_DATA((uint8_t) (y_start & 0xFF));
	LCD_WR_DATA((uint8_t) (y_end >> 8));
	LCD_WR_DATA((uint8_t) (y_end & 0xFF));
}


void init_sequence(void) {
	LCD_RESET();

	LCD_WR_CMD(0xF1);
	LCD_WR_DATA(0x36);
	LCD_WR_DATA(0x04);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x3C);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x8F);

	LCD_WR_CMD(0xF2);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0xA3);
	LCD_WR_DATA(0x12);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0xB2);
	LCD_WR_DATA(0x12);
	LCD_WR_DATA(0xFF);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x00);

	LCD_WR_CMD(0xF8);
	LCD_WR_DATA(0x21);
	LCD_WR_DATA(0x04);

	LCD_WR_CMD(0xF9);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x08);

	LCD_WR_CMD(0xB4);
	LCD_WR_DATA(0x00);

	LCD_WR_CMD(0xC1);
	LCD_WR_DATA(0x41);

	LCD_WR_CMD(0xC5);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x91);
	LCD_WR_DATA(0x80);
	LCD_WR_DATA(0x00);

	LCD_WR_CMD(0xE0);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x1F);
	LCD_WR_DATA(0x1C);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x48);
	LCD_WR_DATA(0x98);
	LCD_WR_DATA(0x37);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x13);
	LCD_WR_DATA(0x04);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x0D);
	LCD_WR_DATA(0x00);

	LCD_WR_CMD(0xE1);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x32);
	LCD_WR_DATA(0x2E);
	LCD_WR_DATA(0x0B);
	LCD_WR_DATA(0x0D);
	LCD_WR_DATA(0x05);
	LCD_WR_DATA(0x47);
	LCD_WR_DATA(0x75);
	LCD_WR_DATA(0x37);
	LCD_WR_DATA(0x06);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x24);
	LCD_WR_DATA(0x20);
	LCD_WR_DATA(0x00);

	LCD_WR_CMD(0x3A);
	LCD_WR_DATA(0x55); // 16-bit/pixel

	LCD_WR_CMD(0x11); // Sleep out
	HAL_Delay(120);

	LCD_WR_CMD(0x36);
	LCD_WR_DATA(0x28);

	HAL_Delay(120);
	LCD_WR_CMD(0x29); // Display ON

}

void display_init(void) {
	LCD_RD_SET; // Set RD high (needed for write) and let it stay high "forever"
	precompute_masks();
	init_sequence();

}

// Assumes RD is always HIGH
// Optimization need to be disabled to prevent compiler to reorder
// TODO: Figure out what it exactly reorder here and why when -O > 0
__attribute__((optimize("O0")))
static inline void lcd_wr_16(uint16_t color) {

	// Send high byte
	GPIOD->BRR = MASK_D_CLEAR;
	GPIOE->BRR = MASK_E_CLEAR;
	GPIOD->BSRR = LUT_mask_D[color >> 8];
	GPIOE->BSRR = LUT_mask_E[color >> 8];
	LCD_WR_CLR;
	__NOP();
	__NOP();
	LCD_WR_SET;

	// Send low byte
	GPIOD->BRR = MASK_D_CLEAR;
	GPIOE->BRR = MASK_E_CLEAR;
	GPIOD->BSRR = LUT_mask_D[color & 0xFF];
	GPIOE->BSRR = LUT_mask_E[color & 0xFF];
	LCD_WR_CLR;
	__NOP();
	__NOP();
	LCD_WR_SET;
}


void lcd_clear(uint16_t color, uint16_t width, uint16_t height) {
	uint32_t total = (uint32_t) width * (uint32_t) height;
	lcd_set_window(0, 0, width - 1, height - 1);
	LCD_WR_CMD(0x2C);

	// keep CS low across whole fill for speed
	LCD_RS_SET;
	LCD_CS_CLR;
	while (total--) {
		// write 2 bytes per pixel without extra CS toggles
		write_data_bus((uint8_t) (color >> 8));
		LCD_WR_Pulse();
		write_data_bus((uint8_t) (color & 0xFF));
		LCD_WR_Pulse();
	}
	LCD_CS_SET;
}

void lcd_clear_area(uint16_t start_x, uint16_t start_y, uint16_t width,
		uint16_t height, uint16_t color) {
	uint32_t total = (uint32_t) width * (uint32_t) height;
	lcd_set_window(start_x, start_y, start_x + width - 1, start_y + height - 1);
	LCD_WR_CMD(0x2C); // start write

	// keep CS low across whole fill for speed
	LCD_RS_SET;
	LCD_CS_CLR;
	while (total--) {
		lcd_wr_16(color);
	}
	LCD_CS_SET;
}

void lcd_draw_area(uint16_t start_x, uint16_t start_y, uint16_t width,
		uint16_t height, uint32_t *buffer) {

	lcd_set_window(start_x, start_y, start_x + width - 1, start_y + height - 1);
	LCD_WR_CMD(0x2C);

	LCD_RS_SET;
	LCD_CS_CLR;

	for (size_t x = 0; x < height * width; x++) {
		uint32_t color = buffer[x];
		lcd_wr_16(color);
	}
	LCD_CS_SET;
}

void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
	lcd_set_window(x, y, x, y); // one-pixel window
	LCD_WR_CMD(0x2C);
	LCD_CS_CLR;
	LCD_RS_SET;
	lcd_wr_16(color);
	LCD_CS_SET;
}

