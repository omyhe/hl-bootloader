/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-03-26     RealThread   first version
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <stm32f4xx.h>
#include <drv_common.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*-------------------------- CHIP CONFIG BEGIN --------------------------*/

#define CHIP_FAMILY_STM32
#define CHIP_SERIES_STM32F4
#define CHIP_NAME_STM32F427VI

/*-------------------------- CHIP CONFIG END --------------------------*/

/*-------------------------- ROM/RAM CONFIG BEGIN --------------------------*/

#define ROM_START              ((uint32_t)0x08000000)
#define ROM_SIZE               (2048 * 1024)
#define ROM_END                ((uint32_t)(ROM_START + ROM_SIZE))

#define RAM_START              (0x20000000)
#define RAM_SIZE               (192 * 1024)
#define RAM_END                (RAM_START + RAM_SIZE)

/*-------------------------- ROM/RAM CONFIG END --------------------------*/

/*-------------------------- CLOCK CONFIG BEGIN --------------------------*/

#define BSP_CLOCK_SOURCE                  ("HSI")
#define BSP_CLOCK_SOURCE_FREQ_MHZ         ((int32_t)0)
#define BSP_CLOCK_SYSTEM_FREQ_MHZ         ((int32_t)180)

/*-------------------------- CLOCK CONFIG END --------------------------*/

/*-------------------------- UART CONFIG BEGIN --------------------------*/

/** After configuring corresponding UART or UART DMA, you can use it.
 *
 * STEP 1, define macro define related to the serial port opening based on the serial port number
 *                 such as     #define BSP_USING_UART1
 *
 * STEP 2, according to the corresponding pin of serial port, define the related serial port information macro
 *                 such as     #define BSP_UART1_TX_PIN       "PA9"
 *                             #define BSP_UART1_RX_PIN       "PA10"
 *
 */

#define BSP_USING_UART3
#define BSP_UART3_TX_PIN       "PB10"
#define BSP_UART3_RX_PIN       "PB11"

/*-------------------------- UART CONFIG END --------------------------*/





#define BSP_USING_ON_CHIP_FLASH














#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */
