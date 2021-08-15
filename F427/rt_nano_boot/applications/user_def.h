/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-09-17     hhym       the first version
 */
#ifndef APPLICATIONS_USER_DEF_H_
#define APPLICATIONS_COM_DEF_H_

#include <rtthread.h>
//#include <rtdevice.h>


#define DEV_FIND(dev, name)                                 \
{                                                           \
    dev = rt_device_find(name);                             \
    if (dev == RT_NULL)                                     \
    {                                                       \
        rt_kprintf("Not find rt %s device!\n", name);       \
        return RT_ERROR;                                    \
    }                                                       \
}





#endif /* APPLICATIONS_USER_DEF_H_ */
