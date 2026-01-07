/**************************************************************************
 --  STM32N6 3.5inch Arduino 8BIT Module MAR3501 Display gpio setup for 8080
 --
 --           Copyright (C) 2025 By Ulrik Hørlyk Hjort
 --
 --  This Program is Free Software; You Can Redistribute It and/or
 --  Modify It Under The Terms of The GNU General Public License
 --  As Published By The Free Software Foundation; Either Version 2
 --  of The License, or (at Your Option) Any Later Version.
 --
 --  This Program is Distributed in The Hope That It Will Be Useful,
 --  But WITHOUT ANY WARRANTY; Without Even The Implied Warranty of
 --  MERCHANTABILITY or FITNESS for A PARTICULAR PURPOSE.  See The
 --  GNU General Public License for More Details.
 --
 -- You Should Have Received A Copy of The GNU General Public License
 -- Along with This Program; if not, See <Http://Www.Gnu.Org/Licenses/>.
***************************************************************************/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32n6xx_hal.h"
#include "gpio_defines.h"


void MX_GPIO_Init(void);


#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

