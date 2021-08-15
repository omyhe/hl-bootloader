/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-03-26     hhym       the first version
 */
#ifndef BOARD_BSP_LED_LED_H_
#define BOARD_BSP_LED_LED_H_

#include "main.h"
#include "drv_common.h"

#define ON    1
#define OFF   0


#define LED_ON_LEVEL           GPIO_PIN_RESET
#define LED_RED_ON_LEVEL       GPIO_PIN_RESET
#define LED_RED_OFF_LEVEL      !LED_RED_ON_LEVEL

#define LED_GREEN_ON_LEVEL     GPIO_PIN_RESET
#define LED_GREEN_OFF_LEVEL    !LED_GREEN_ON_LEVEL




int  led_init(void);
void led_red(rt_bool_t status);
void led_green(rt_bool_t status);
void led_red_toggle(void);
void led_green_toggle(void);


#endif /* BOARD_BSP_LED_LED_H_ */
