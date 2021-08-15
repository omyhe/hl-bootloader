/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-03-26     RT-Thread    first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "bsp/led/led.h"
#include "boot.h"



int main(void)
{
//    show_version();
//    fal_init();
    boot_init();
    while (1)
    {
        led_green_toggle();

        rt_thread_mdelay(100);
    }

    return RT_EOK;
}
