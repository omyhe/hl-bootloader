/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-03-26     hhym       the first version
 */
#ifndef APPLICATIONS_FAL_FAL_CFG_H_
#define APPLICATIONS_FAL_FAL_CFG_H_

#include <board.h>

#define NOR_FLASH_DEV_NAME             "w25qxx"
#define ONCHIP_FLASH16K_DEV_NAME       "onchip_flash_16k"
#define ONCHIP_FLASH64K_DEV_NAME       "onchip_flash_64k"
#define ONCHIP_FLASH128K_DEV_NAME      "onchip_flash_128k"

#define FAL_PART_BL                    "bl"
#define FAL_PART_APP                   "app"
#define FAL_PART_DOWNLOAD              "download"
#define FAL_PART_PARAM                 "param"
#define FAL_PART_DL                    "download2"
#define FAL_PART_FACTORY               "factory"
#define FAL_PART_FATFS                 "fatfs"

#define FAL_PART_BL_SIZE               (4*16*1024+1*64*1024)
#define FAL_PART_APP_SIZE              (3*128*1024)
#define FAL_PART_DOWNLOAD_SIZE         (4*128*1024)
/* ===================== Flash device Configuration ========================= */
#define FLASH_SIZE_GRANULARITY_16K    ((1+1+1+1)*16*1024)
#define FLASH_SIZE_GRANULARITY_64K    (1*64*1024)
#define FLASH_SIZE_GRANULARITY_128K   (15*128*1024)

#define STM32_FLASH_START_ADRESS_16K    ROM_START
#define STM32_FLASH_START_ADRESS_64K    STM32_FLASH_START_ADRESS_16K + FLASH_SIZE_GRANULARITY_16K
#define STM32_FLASH_START_ADRESS_128K   STM32_FLASH_START_ADRESS_64K + FLASH_SIZE_GRANULARITY_64K


//extern const struct fal_flash_dev stm32f4_onchip_flash;
extern const struct fal_flash_dev stm32_onchip_flash_16k;
extern const struct fal_flash_dev stm32_onchip_flash_64k;
extern const struct fal_flash_dev stm32_onchip_flash_128k;

extern struct fal_flash_dev nor_flash0;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
    &stm32_onchip_flash_16k,                                         \
    &stm32_onchip_flash_64k,                                         \
    &stm32_onchip_flash_128k,                                        \
}

/* ====================== Partition Configuration ========================== */
/* partition table */
#define FAL_PART_TABLE                                                               \
{                                                                                    \
    {FAL_PART_MAGIC_WORD, FAL_PART_BL      ,  ONCHIP_FLASH16K_DEV_NAME,          0,                  4*16*1024, 0}, \
    {FAL_PART_MAGIC_WORD, FAL_PART_APP     , ONCHIP_FLASH128K_DEV_NAME,          0,                 4*128*1024, 0}, \
    {FAL_PART_MAGIC_WORD, FAL_PART_DOWNLOAD, ONCHIP_FLASH128K_DEV_NAME, 7*128*1024,                 4*128*1024, 0}, \
    {FAL_PART_MAGIC_WORD, FAL_PART_FACTORY , ONCHIP_FLASH128K_DEV_NAME,11*128*1024,                 4*128*1024, 0}, \
}

#endif /* APPLICATIONS_FAL_FAL_CFG_H_ */
