/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-03-26     hhym       the first version
 */

#include "led.h"


void led_red(rt_bool_t status)
{
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, (status == ON ? LED_RED_ON_LEVEL:LED_RED_OFF_LEVEL));
}

void led_green(rt_bool_t status)
{
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, (status == ON ? LED_GREEN_ON_LEVEL:LED_GREEN_OFF_LEVEL));
}

void led_red_toggle(void)
{
    HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
}

void led_green_toggle(void)
{
    HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
}


int led_init(void)
{
    extern void MX_GPIO_Init(void);
    MX_GPIO_Init();

    led_red(ON);
    led_green(ON);

    return RT_EOK;
}
INIT_PREV_EXPORT(led_init);


//static void pin(int argc, char **argv)
//{
//    uint32_t pin_num = 0;
//    uint8_t pin_level = 0;
//
//    if(argc > 1)
//    {
//        if(!rt_strcmp(argv[1], "set"))
//        {
//            if(argc == 5)
//            {
//                pin_num = atoi(argv[3]);
//                if (!rt_strcmp(argv[4], "0"))
//                {
//                    pin_level = PIN_LOW;
//                }
//                else
//                {
//                    pin_level = PIN_HIGH;
//                }
//
//                     if(!rt_strcmp(argv[2], "A")) {rt_pin_write(GET_PIN(A, pin_num), pin_level);}
//                else if(!rt_strcmp(argv[2], "B")) {rt_pin_write(GET_PIN(B, pin_num), pin_level);}
//                else if(!rt_strcmp(argv[2], "C")) {rt_pin_write(GET_PIN(C, pin_num), pin_level);}
//                else if(!rt_strcmp(argv[2], "D")) {rt_pin_write(GET_PIN(D, pin_num), pin_level);}
//                else if(!rt_strcmp(argv[2], "E")) {rt_pin_write(GET_PIN(E, pin_num), pin_level);}
//                else if(!rt_strcmp(argv[2], "F")) {rt_pin_write(GET_PIN(F, pin_num), pin_level);}
//                else if(!rt_strcmp(argv[2], "G")) {rt_pin_write(GET_PIN(G, pin_num), pin_level);}
//                else {
//                    rt_kprintf("%s pin port %s is invlid\n", argv[1], argv[2]);
//                    return;
//                }
//                rt_kprintf("%s pin P%s%s=%s ok!\n", argv[1], argv[2], argv[3], argv[4]);
//            }
//        }
//        else if(!rt_strcmp(argv[1], "get"))
//        {
//            if(argc == 4)
//            {
//                pin_num = atoi(argv[3]);
//
//                     if(!rt_strcmp(argv[2], "A")) {pin_level = rt_pin_read(GET_PIN(A, pin_num));}
//                else if(!rt_strcmp(argv[2], "B")) {pin_level = rt_pin_read(GET_PIN(B, pin_num));}
//                else if(!rt_strcmp(argv[2], "C")) {pin_level = rt_pin_read(GET_PIN(C, pin_num));}
//                else if(!rt_strcmp(argv[2], "D")) {pin_level = rt_pin_read(GET_PIN(D, pin_num));}
//                else if(!rt_strcmp(argv[2], "E")) {pin_level = rt_pin_read(GET_PIN(E, pin_num));}
//                else if(!rt_strcmp(argv[2], "F")) {pin_level = rt_pin_read(GET_PIN(F, pin_num));}
//                else if(!rt_strcmp(argv[2], "G")) {pin_level = rt_pin_read(GET_PIN(G, pin_num));}
//                else {
//                    rt_kprintf("%s pin port %s is invlid\n", argv[1], argv[2]);
//                    return;
//                }
//                rt_kprintf("%s pin P%s%s=%d ok!\n", argv[1], argv[2], argv[3], pin_level);
//            }
//        }
//    }
//}
//MSH_CMD_EXPORT(pin, pin set function);


