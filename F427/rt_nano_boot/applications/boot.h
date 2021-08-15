/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-02     hhym       the first version
 */
#ifndef APPLICATIONS_BOOT_H_
#define APPLICATIONS_BOOT_H_

#include <stdint.h>
#include "fal/fal_cfg.h"


#define BOOT_REBOOT_DELAY_MS    5000
#define BOOT_FW_INFO_HDR        0xa5a5
#define BOOT_DOWNLOAD_PART_NAME FAL_PART_DOWNLOAD
#define BOOT_FACTORY_PART_NAME  FAL_PART_FACTORY


#define QBOOT_THREAD_STACK_SIZE 4096
#define QBOOT_USING_QUICKLZ







#define QBOOT_BUF_SIZE                  4096//must is 4096
#if (defined(QBOOT_USING_QUICKLZ) || defined(QBOOT_USING_FASTLZ))
#define QBOOT_CMPRS_READ_SIZE           1024 //it can is 512, 1024, 2048, 4096,
#define QBOOT_CMPRS_BUF_SIZE            (QBOOT_BUF_SIZE + QBOOT_CMPRS_READ_SIZE)
#else
#define QBOOT_CMPRS_READ_SIZE           QBOOT_BUF_SIZE
#define QBOOT_CMPRS_BUF_SIZE            QBOOT_BUF_SIZE
#endif


#define QBOOT_ALGO_CRYPT_NONE           0
#define QBOOT_ALGO_CRYPT_XOR            1
#define QBOOT_ALGO_CRYPT_AES            2
#define QBOOT_ALGO_CRYPT_MASK           0x0F

#define QBOOT_ALGO_CMPRS_NONE           0
#define QBOOT_ALGO_CMPRS_GZIP           1
#define QBOOT_ALGO_CMPRS_QUICKLZ        2
#define QBOOT_ALGO_CMPRS_FASTLZ         3
#define QBOOT_ALGO_CMPRS_MASK           0x0F


struct fw_info{
    uint16_t hdr;
    uint8_t  fw_ver[16];
    uint8_t  pkg_date[24];
    uint8_t  prod_uid[24];
    uint8_t  part_name[15];
    uint8_t  f_jump_err  :1;
    uint8_t  f_had_copy  :1;
    uint8_t  reserve     :6;
    uint8_t  cmprs_algo;
    uint8_t  crypt_algo;
    uint32_t raw_size;
    uint32_t raw_crc;
    uint32_t pkg_size;
    uint32_t pkg_crc;
    uint32_t hdr_crc;
};
typedef struct fw_info *fw_info_t;



int boot_init(void);

#endif /* APPLICATIONS_BOOT_H_ */
