/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-09-17     hhym       the first version
 */
#ifndef APPLICATIONS_USER_CFG_H_
#define APPLICATIONS_COM_CFG_H_


#define FIRMWARE_VERSION "[ctrl:T4.0.1]"



#define BOOT_VERSION "V1.0.0"



/* system packages */

#define PKG_USING_FAL
//#define FAL_DEBUG_CONFIG
//#define FAL_DEBUG 1
#define FAL_PART_HAS_TABLE_CFG
#define PKG_USING_FAL_V00500
#define PKG_FAL_VER_NUM 0x00500
#define PKG_USING_QBOOT
#define QBOOT_APP_PART_NAME "app"
#define QBOOT_DOWNLOAD_PART_NAME "download"
#define QBOOT_FACTORY_PART_NAME "factory"
#define QBOOT_USING_QUICKLZ
#define QBOOT_THREAD_STACK_SIZE 4096
#define QBOOT_THREAD_PRIO 5
#define PKG_USING_QBOOT_LATEST_VERSION

/* Micrium: Micrium software products porting for RT-Thread */

/* end of Micrium: Micrium software products porting for RT-Thread */
/* end of system packages */

/* peripheral libraries and drivers */

/* end of peripheral libraries and drivers */

/* AI packages */

/* end of AI packages */

/* miscellaneous packages */

#define PKG_USING_QUICKLZ
#define QLZ_COMPRESSION_LEVEL 3
#define PKG_USING_QUICKLZ_V101

/* samples: kernel and components samples */

/* end of samples: kernel and components samples */
#define PKG_USING_CRCLIB
#define CRCLIB_USING_CRC8
#define CRC8_USING_CONST_TABLE
#define CRC8_POLY_8C
#define CRC8_POLY 140
#define CRCLIB_USING_CRC16
#define CRC16_USING_CONST_TABLE
#define CRC16_POLY_A001
#define CRC16_POLY 40961
#define CRCLIB_USING_CRC32
#define CRC32_USING_CONST_TABLE
#define CRC32_POLY_EDB88320
#define CRC32_POLY 3988292384
#define PKG_USING_CRCLIB_V100

#endif /* APPLICATIONS_USER_CFG_H_ */
