/***************************************************************************
;                        Rotating Cube Main
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
#include <rotating_cube.h>
#include "main.h"
#include "gpio.h"

void SystemClock_Config(void);

int main(void) {

	HAL_Init();
	HAL_PWREx_EnableVddIO4();
	SystemClock_Config();
	MX_GPIO_Init();

	if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_ICACHE_Enable() != HAL_OK) {
		Error_Handler();
	}

	run_cube();
	while (1) {
	}

}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the System Power Supply
	 */
	if (HAL_PWREx_ConfigSupply(PWR_EXTERNAL_SOURCE_SUPPLY) != HAL_OK) {
		Error_Handler();
	}

	/* Enable HSI */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Get current CPU/System buses clocks configuration and if necessary switch
	 to intermediate HSI clock to ensure target clock can be set
	 */
	HAL_RCC_GetClockConfig(&RCC_ClkInitStruct);
	if ((RCC_ClkInitStruct.CPUCLKSource == RCC_CPUCLKSOURCE_IC1)
			|| (RCC_ClkInitStruct.SYSCLKSource == RCC_SYSCLKSOURCE_IC2_IC6_IC11)) {
		RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_CPUCLK
				| RCC_CLOCKTYPE_SYSCLK);
		RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_HSI;
		RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
		if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK) {
			/* Initialization Error */
			Error_Handler();
		}
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
	RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL1.PLLM = 2;
	RCC_OscInitStruct.PLL1.PLLN = 75;
	RCC_OscInitStruct.PLL1.PLLFractional = 0;
	RCC_OscInitStruct.PLL1.PLLP1 = 1;
	RCC_OscInitStruct.PLL1.PLLP2 = 1;
	RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
			| RCC_CLOCKTYPE_PCLK5 | RCC_CLOCKTYPE_PCLK4;
	RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_IC1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_IC2_IC6_IC11;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;
	RCC_ClkInitStruct.APB5CLKDivider = RCC_APB5_DIV1;
	RCC_ClkInitStruct.IC1Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
	RCC_ClkInitStruct.IC1Selection.ClockDivider = 3;
	RCC_ClkInitStruct.IC2Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
	RCC_ClkInitStruct.IC2Selection.ClockDivider = 6;
	RCC_ClkInitStruct.IC6Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
	RCC_ClkInitStruct.IC6Selection.ClockDivider = 4;
	RCC_ClkInitStruct.IC11Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
	RCC_ClkInitStruct.IC11Selection.ClockDivider = 4;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK) {
		Error_Handler();
	}
}

void Error_Handler(void) {
	__disable_irq();
	while (1) {
	}
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) { }
#endif /* USE_FULL_ASSERT */
