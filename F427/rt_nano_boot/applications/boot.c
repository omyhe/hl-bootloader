/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-02     hhym       the first version
 */

#include <rtthread.h>
#include "boot.h"
#include "fal/inc/fal.h"
#include <qboot_aes.h>
#include <qboot_gzip.h>
#include <qboot_fastlz.h>
#include <qboot_quicklz.h>
#include <string.h>
#include "crc32.h"


static uint8_t cmprs_buf[QBOOT_CMPRS_BUF_SIZE];
#if (defined(QBOOT_USING_AES) || defined(QBOOT_USING_GZIP) || defined(QBOOT_USING_QUICKLZ) || defined(QBOOT_USING_FASTLZ))
static uint8_t crypt_buf[QBOOT_BUF_SIZE];
#else
static uint8_t *crypt_buf = NULL;
#endif

#ifdef QBOOT_USING_GZIP
#define GZIP_REMAIN_BUF_SIZE 32
static int gzip_remain_len = 0;
static uint8_t gzip_remain_buf[GZIP_REMAIN_BUF_SIZE];
#endif


#define BT_DEBUG
#ifdef BT_DEBUG
#define BT_PRINTF(...)  \
{                       \
    rt_kprintf("[%d]", rt_tick_get()); \
    rt_kprintf(__VA_ARGS__);    \
}
#else
#define BT_PRINTF(...)
#endif






void show_version(void)
{
    BT_PRINTF("bootloader startup...\n");
    BT_PRINTF("bootloader version: %s\n", BOOT_VERSION);
}


static void jump_to_app(void)
{
    typedef void (*app_func_t)(void);
    uint32_t app_addr = fal_partition_start_addr_find(FAL_PART_APP);
    uint32_t stk_addr = *((__IO uint32_t *)app_addr);
    app_func_t app_func = (app_func_t)(*((__IO uint32_t *)(app_addr + 4)));

    if ((((uint32_t)app_func & 0xff000000) != 0x08000000) || ((stk_addr & 0x2ff00000) != 0x20000000))
    {
        BT_PRINTF("no legitimate application.\n");
        return;
    }

    BT_PRINTF("jump to application running ... \n");
    rt_thread_mdelay(200);

    __disable_irq();
    HAL_DeInit();

    for(int i=0; i<128; i++)
    {
        HAL_NVIC_DisableIRQ(i);
        HAL_NVIC_ClearPendingIRQ(i);
    }

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    HAL_RCC_DeInit();

    __set_CONTROL(0);
    __set_MSP(stk_addr);

    app_func();//Jump to application running

    BT_PRINTF("boot jump to application fail\n");
}


static bool qbt_fw_decrypt_init(int crypt_type)
{
    switch (crypt_type)
    {
    case QBOOT_ALGO_CRYPT_NONE:
        break;

    #ifdef QBOOT_USING_AES
    case QBOOT_ALGO_CRYPT_AES:
        qbt_aes_decrypt_init();
        break;
    #endif

    default:
        return(false);
    }

    return(true);
}

static bool qbt_fw_decompress_init(int cmprs_type)
{
    switch (cmprs_type)
    {
    case QBOOT_ALGO_CMPRS_NONE:
        break;

    #ifdef QBOOT_USING_GZIP
    case QBOOT_ALGO_CMPRS_GZIP:
        gzip_remain_len = 0;
        qbt_gzip_init();
        break;
    #endif

    #ifdef QBOOT_USING_QUICKLZ
    case QBOOT_ALGO_CMPRS_QUICKLZ:
        qbt_quicklz_state_init();
        break;
    #endif

    #ifdef QBOOT_USING_FASTLZ
    case QBOOT_ALGO_CMPRS_FASTLZ:
        break;
    #endif

    default:
        return(false);
    }

    return(true);
}

static bool qbt_fw_decompress_deinit(int cmprs_type)
{
    switch (cmprs_type)
    {
    case QBOOT_ALGO_CMPRS_NONE:
        break;

    #ifdef QBOOT_USING_GZIP
    case QBOOT_ALGO_CMPRS_GZIP:
        qbt_gzip_deinit();
        break;
    #endif

    #ifdef QBOOT_USING_QUICKLZ
    case QBOOT_ALGO_CMPRS_QUICKLZ:
        break;
    #endif

    #ifdef QBOOT_USING_FASTLZ
    case QBOOT_ALGO_CMPRS_FASTLZ:
        break;
    #endif

    default:
        return(false);
    }

    return(true);
}

static bool qbt_fw_pkg_read(const fal_partition_t part, uint32_t pos, uint8_t *buf, uint32_t read_len, uint8_t *crypt_buf, int crypt_type)
{
    switch(crypt_type)
    {
    case QBOOT_ALGO_CRYPT_NONE:
        if (fal_partition_read(part, pos, buf, read_len) < 0)
        {
            return(false);
        }
        break;

    #ifdef QBOOT_USING_AES
    case QBOOT_ALGO_CRYPT_AES:
        if (fal_partition_read(part, pos, crypt_buf, read_len) < 0)
        {
           return(false);
        }
        qbt_aes_decrypt(buf, crypt_buf, read_len);
        break;
    #endif

    default:
        return(false);
    }

    return(true);
}

static int qbt_dest_part_write(fal_partition_t part, uint32_t pos, uint8_t *decmprs_buf, uint8_t *cmprs_buf, uint32_t *p_cmprs_len, int cmprs_type)
{
    int write_len = 0;
    int cmprs_len = 0;
    int decomp_len = 0;
    int block_size = 0;

    cmprs_len = *p_cmprs_len;

    switch(cmprs_type)
    {
    case QBOOT_ALGO_CMPRS_NONE:
        if (fal_partition_write(part, pos, cmprs_buf, cmprs_len) < 0)
        {
            return(-1);
        }
        write_len = cmprs_len;
        cmprs_len = 0;
        break;

    #ifdef QBOOT_USING_GZIP
    case QBOOT_ALGO_CMPRS_GZIP:
        qbt_gzip_set_in(cmprs_buf, cmprs_len);
        while(1)
        {
            bool is_end;

            memcpy(decmprs_buf, gzip_remain_buf, gzip_remain_len);
            decomp_len = qbt_gzip_decompress(decmprs_buf + gzip_remain_len, QBOOT_BUF_SIZE - gzip_remain_len);
            if (decomp_len < 0)
            {
                write_len = -1;
                cmprs_len = 0;
                break;
            }
            is_end = (decomp_len < (QBOOT_BUF_SIZE - gzip_remain_len));
            decomp_len += gzip_remain_len;
            gzip_remain_len = decomp_len % GZIP_REMAIN_BUF_SIZE;
            decomp_len -= gzip_remain_len;
            memcpy(gzip_remain_buf, decmprs_buf + decomp_len, gzip_remain_len);
            if (decomp_len > 0)
            {
                if (fal_partition_write(part, pos, decmprs_buf, decomp_len) < 0)
                {
                    write_len = -1;
                    cmprs_len = 0;
                    break;
                }
            }
            pos += decomp_len;
            write_len += decomp_len;
            if (is_end && (cmprs_len < QBOOT_CMPRS_READ_SIZE) && (gzip_remain_len > 0))//last package and remain > 0
            {
                memset(gzip_remain_buf + gzip_remain_len, 0xFF, GZIP_REMAIN_BUF_SIZE - gzip_remain_len);
                fal_partition_write(part, pos, gzip_remain_buf, GZIP_REMAIN_BUF_SIZE);
                write_len += GZIP_REMAIN_BUF_SIZE;
            }
            if (is_end)
            {
                cmprs_len = 0;
                break;
            }
        }
        break;
    #endif

    #ifdef QBOOT_USING_QUICKLZ
    case QBOOT_ALGO_CMPRS_QUICKLZ:
        while(1)
        {
            if (cmprs_len < QBOOT_QUICKLZ_BLOCK_HDR_SIZE)
            {
                break;
            }
            block_size = qbt_quicklz_get_block_size(cmprs_buf);
            if (block_size <= 0)
            {
                break;
            }
            if (cmprs_len < block_size + QBOOT_QUICKLZ_BLOCK_HDR_SIZE)
            {
                break;
            }
            decomp_len = qbt_quicklz_decompress(decmprs_buf, cmprs_buf + QBOOT_QUICKLZ_BLOCK_HDR_SIZE);
            if (decomp_len <= 0)
            {
                write_len = -1;
                cmprs_len = 0;
                break;
            }
            if (fal_partition_write(part, pos, decmprs_buf, decomp_len) < 0)
            {
                write_len = -1;
                cmprs_len = 0;
                break;
            }
            pos += decomp_len;
            write_len += decomp_len;
            cmprs_len -= (block_size + QBOOT_QUICKLZ_BLOCK_HDR_SIZE);
            memcpy(cmprs_buf, cmprs_buf + (block_size + QBOOT_QUICKLZ_BLOCK_HDR_SIZE), cmprs_len);
        }
        break;
    #endif

    #ifdef QBOOT_USING_FASTLZ
    case QBOOT_ALGO_CMPRS_FASTLZ:
        while(1)
        {
            if (cmprs_len < QBOOT_FASTLZ_BLOCK_HDR_SIZE)
            {
                break;
            }
            block_size = qbt_fastlz_get_block_size(cmprs_buf);
            if (block_size <= 0)
            {
                break;
            }
            if (cmprs_len < block_size + QBOOT_FASTLZ_BLOCK_HDR_SIZE)
            {
                break;
            }
            decomp_len = qbt_fastlz_decompress(decmprs_buf, QBOOT_BUF_SIZE, cmprs_buf + QBOOT_FASTLZ_BLOCK_HDR_SIZE, block_size);
            if (decomp_len <= 0)
            {
                write_len = -1;
                cmprs_len = 0;
                break;
            }
            if (fal_partition_write(part, pos, decmprs_buf, decomp_len) < 0)
            {
                write_len = -1;
                cmprs_len = 0;
                break;
            }
            pos += decomp_len;
            write_len += decomp_len;
            cmprs_len -= (block_size + QBOOT_FASTLZ_BLOCK_HDR_SIZE);
            memcpy(cmprs_buf, cmprs_buf + (block_size + QBOOT_FASTLZ_BLOCK_HDR_SIZE), cmprs_len);
        }
        break;
    #endif

    default:
        write_len = -1;
        cmprs_len = 0;
        break;
    }

    *p_cmprs_len = cmprs_len;

    return(write_len);
}

static bool qbt_fw_release(const char *dst_part_name, const char *src_part_name, fw_info_t fw_info)
{
    uint32_t cmprs_len = 0;
    uint32_t dst_write_pos = 0;
    uint32_t src_read_pos = sizeof(struct fw_info);
    fal_partition_t src_part = (fal_partition_t)fal_partition_find(src_part_name);
    fal_partition_t dst_part = (fal_partition_t)fal_partition_find(dst_part_name);
    int crypt_type = fw_info->crypt_algo;
    int cmprs_type = fw_info->cmprs_algo;

    if ( ! qbt_fw_decrypt_init(crypt_type))
    {
        BT_PRINTF("boot release firmware fail. nonsupport encrypt type.\n");
        return(false);
    }

    if ( ! qbt_fw_decompress_init(cmprs_type))
    {
        BT_PRINTF("boot release firmware fail. nonsupport compress type.\n");
        return(false);
    }

    BT_PRINTF("start erase partition %s ...\n", dst_part_name);
    if ((fal_partition_erase(dst_part, 0, fw_info->raw_size) < 0)
        || (fal_partition_erase(dst_part, dst_part->len - sizeof(struct fw_info), sizeof(struct fw_info)) < 0))
    {
        qbt_fw_decompress_deinit(cmprs_type);
        BT_PRINTF("boot release firmware fail. erase %s error.\n", dst_part_name);
        return(false);
    }

    BT_PRINTF("start release firmware to %s ...     ", dst_part_name);
    while(dst_write_pos < fw_info->raw_size)
    {
        int write_len = 0;
        int read_len = QBOOT_CMPRS_READ_SIZE;
        int remain_len = (fw_info->pkg_size + sizeof(struct fw_info) - src_read_pos);
        if (read_len > remain_len)
        {
            read_len = remain_len;
        }
        if ( ! qbt_fw_pkg_read(src_part, src_read_pos, cmprs_buf + cmprs_len, read_len, crypt_buf, crypt_type))
        {
            qbt_fw_decompress_deinit(cmprs_type);
            BT_PRINTF("boot release firmware fail. read package error, part = %s, addr = %08X, length = %d\n", src_part_name, src_read_pos, read_len);
            return(false);
        }
        src_read_pos += read_len;
        cmprs_len += read_len;

        write_len = qbt_dest_part_write(dst_part, dst_write_pos, crypt_buf, cmprs_buf, &cmprs_len, cmprs_type);
        if (write_len < 0)
        {
            qbt_fw_decompress_deinit(cmprs_type);
            BT_PRINTF("boot release firmware fail. write destination error, part = %s, addr = %08X\n", dst_part_name, dst_write_pos);
            return(false);
        }
        dst_write_pos += write_len;

        rt_kprintf("\b\b\b%02d%%", (dst_write_pos * 100 / fw_info->raw_size));
    }
    rt_kprintf("\n");

    qbt_fw_decompress_deinit(cmprs_type);
//    if ( ! qbt_fw_info_write(dst_part_name, fw_info, true))
//    {
//        LOG_E("boot release firmware fail. write firmware to %s fail.", dst_part_name);
//        return(false);
//    }

    return(true);
}

int fw_info_read(const char *part_name, fw_info_t fw_info)
{
    const struct fal_partition *part = fal_partition_find(part_name);
    if (part == RT_NULL)
    {
        BT_PRINTF("fal_partition(%s) is not exist\n", part_name);
        return -1;
    }
    if (fal_partition_read(part, 0, (uint8_t *)fw_info, sizeof(struct fw_info)) < 0)
    {
        BT_PRINTF("fal_partition(%s) read fail\n", part_name);
        return -1;
    }

    return RT_EOK;
}

int fw_info_write(const char *part_name, fw_info_t fw_info)
{
    const struct fal_partition *part = fal_partition_find(part_name);
    if (part == RT_NULL)
    {
        BT_PRINTF("fal_partition(%s) is not exist\n", part_name);
        return -1;
    }
    if (fal_partition_write(part, 0, (uint8_t *)fw_info, sizeof(struct fw_info)) < 0)
    {
        BT_PRINTF("fal_partition(%s) read fail\n", part_name);
        return -1;
    }

    return RT_EOK;
}

int fw_info_chk(const char *part_name, fw_info_t fw_info)
{
    if (fw_info_read(part_name, fw_info) != RT_EOK)
    {
        return -1;
    }

    if (!fw_info->f_jump_err)
    {
        BT_PRINTF("app had jump fail, not need jump\n", part_name);
        return -1;
    }

    if (fw_info->hdr != BOOT_FW_INFO_HDR)
    {
        BT_PRINTF("fal_partition(%s) is not code\n", part_name);
        return 1;
    }

    if (crc32_cal((uint8_t *)fw_info, (sizeof(struct fw_info) - sizeof(uint32_t))) != fw_info->hdr_crc)
    {
        BT_PRINTF("fal_partition(%s) fw_info header crc verify fail\n", part_name);
        return -1;
    }

    return RT_EOK;
}



int fal_fart_chk(const char *part_name)
{
    struct fw_info fw_info = {0};
    uint32_t app_addr, part_addr;
    int ret = RT_EOK;

    BT_PRINTF("fal_partition(%s) hdr check start...\n", part_name);
    ret = fw_info_chk(part_name, &fw_info);
    if (ret != RT_EOK)
    {
        return ret;
    }

//    if (fw_info.f_had_copy)
//    {
//        goto __fw_need_release;
//    }

    app_addr = fal_partition_start_addr_find(FAL_PART_APP);
    if (crc32_cal((uint8_t *)app_addr, fw_info.raw_size) == fw_info.raw_crc)
    {
        BT_PRINTF("fal_partition(app) had the same code\n");
        return RT_EOK;
    }
    BT_PRINTF("fal_partition(app) code crc verify fail\n");

//__fw_need_release:
    part_addr = fal_partition_start_addr_find(part_name);
    if (crc32_cal((uint8_t *)part_addr+sizeof(struct fw_info), fw_info.pkg_size) != fw_info.pkg_crc)
    {
        BT_PRINTF("fal_partition(%s) pkg crc verify fail\n", part_name);
        return -1;
    }
    BT_PRINTF("fal_partition(%s) pkg crc verify seccuss\n", part_name);

    if (!qbt_fw_release((const char *)fw_info.part_name, part_name, &fw_info))
    {
        BT_PRINTF("fal_partition(%s) fw_release fail\n", part_name);
        return -1;
    }

//    fw_info.f_had_copy = 0;
//    fw_info.hdr_crc = crc32_cal((uint8_t *)&fw_info, (sizeof(struct fw_info) - sizeof(uint32_t)));
//    if (fw_info_write(part_name, &fw_info) != RT_EOK)
//    {
//        return -1;
//    }

    return RT_EOK;
}


static void boot_entry(void *paramters)
{
    show_version();

    if (fal_init() <= 0)
    {
        BT_PRINTF("fal init fail\n");
        BT_PRINTF("bootloader will reboot after %d ms.", BOOT_REBOOT_DELAY_MS);
        rt_thread_mdelay(BOOT_REBOOT_DELAY_MS);
        rt_hw_cpu_reset();
    }

    if (CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)
    {
        BT_PRINTF("<<debug mode>>:direct jump to app!\n");
        jump_to_app();
    }

    #ifdef QBOOT_USING_FACTORY_KEY
    if (qbt_factory_key_check())
    {
        if (qbt_app_resume_from(QBOOT_FACTORY_PART_NAME))
        {
            qbt_jump_to_app();
        }
    }
    #endif

    while(1)
    {
        if (fal_fart_chk(BOOT_DOWNLOAD_PART_NAME) >= RT_EOK)
        {
            jump_to_app();
        }
        if (fal_fart_chk(BOOT_FACTORY_PART_NAME) >= RT_EOK)
        {
            jump_to_app();
        }

        rt_thread_mdelay(BOOT_REBOOT_DELAY_MS);
    }
}


int boot_init(void)
{
    static struct rt_thread tid = {0};
    static char stack_buf[QBOOT_THREAD_STACK_SIZE] = {0};


    rt_err_t err = rt_thread_init(&tid, "boot", boot_entry, NULL, (void *)stack_buf, QBOOT_THREAD_STACK_SIZE, QBOOT_THREAD_PRIO, 20);
    if (err != RT_EOK)
    {
        return -RT_ERROR;
    }
    rt_thread_startup(&tid);

    return RT_EOK;
}
//INIT_APP_EXPORT(boot_init);






