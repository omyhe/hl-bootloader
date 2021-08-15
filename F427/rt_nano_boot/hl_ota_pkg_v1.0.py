# -*- coding:utf-8 -*-
import os
import sys
import time
import configparser
import binascii
import quicklz

INI_dict = {
    'src_file': '',
    'dst_file': '',
    'product_code': '00010203040506070809',
    'cmprs_type': '2',
    'crypt_type': '0',
    'crypt_key': '0123456789ABCDEF0123456789ABCDEF',
    'crypt_iv': '0123456789ABCDEF',
    'part_name': 'app',
    'version': 'v4.0.0'
}

def cfg_default_write(cfg):
    cfg.add_section('OTA')
    cfg.set('OTA', 'src_file',     INI_dict['src_file'])
    cfg.set('OTA', 'dst_file',     INI_dict['dst_file'])
    cfg.set('OTA', 'product_code', INI_dict['product_code'])
    cfg.set('OTA', 'cmprs_type',   INI_dict['cmprs_type'])
    cfg.set('OTA', 'crypt_type',   INI_dict['crypt_type'])
    cfg.set('OTA', 'crypt_key',    INI_dict['crypt_key'])
    cfg.set('OTA', 'crypt_iv',     INI_dict['crypt_iv'])
    cfg.set('OTA', 'part_name',    INI_dict['part_name'])
    cfg.set('OTA', 'version',      INI_dict['version'])
    cfg.write(open("ota_pkg.ini", "w"))

def cfg_load(cfg):
    sect = cfg['OTA']
    INI_dict['src_file'] = sect['src_file']
    INI_dict['dst_file'] = sect['dst_file']
    INI_dict['product_code'] = sect['product_code']
    INI_dict['cmprs_type'] = sect['cmprs_type']
    INI_dict['crypt_type'] = sect['crypt_type']
    INI_dict['crypt_key'] = sect['crypt_key']
    INI_dict['crypt_iv'] = sect['crypt_iv']
    INI_dict['part_name'] = sect['part_name']
    INI_dict['version'] = sect['version']

def cfg_init():
    try:
        f_cfg = open("ota_pkg.ini", 'r')
    except IOError:
        f_cfg = open("ota_pkg.ini", 'w+')
        print("create ota_pkg.ini success!")
    f_cfg.close()
    config = configparser.ConfigParser()
    config.read("ota_pkg.ini", encoding="utf-8")
    if config.has_section('OTA'):
        cfg_load(config)
    else:
        cfg_default_write(config)



CMPRS_NONE = 0
CMPRS_GZIP = 1
CMPRS_QUICKLZ = 2
CMPRS_FASTLZ = 3

def qklz_compress(out_file, in_file):
    need_cmpr_size = os.path.getsize(in_file)
    cmpr_total_size = 0
    in_fd = open(in_file, 'rb')
    out_fd = open(out_file, 'wb+')
    while need_cmpr_size > 0:
        if need_cmpr_size < 4096:
            per_cmpr_size = need_cmpr_size
        else:
            per_cmpr_size = 4096
        cmpr_content = quicklz.compress(in_fd.read(per_cmpr_size))
        cmpr_size = len(cmpr_content)
        out_fd.write(cmpr_size.to_bytes(4, byteorder='big'))
        out_fd.write(cmpr_content)
        cmpr_total_size += (4+cmpr_size)
        need_cmpr_size -= per_cmpr_size
    in_fd.close()
    out_fd.close()
    print("create %s(size=%d) seccessfully!" % (out_file, cmpr_total_size))
    return cmpr_total_size







app_bin_defualt_filepath = "./"


def bytes_write_int(fd, addr, int, int_len=4):
    fd.seek(addr)
    fd.write(int.to_bytes(int_len, byteorder='little'))

def bytes_write_str(fd, addr, str):
    fd.seek(addr)
    fd.write(str.encode('utf-8'))

def look_file(dir, ext):
    for root, dirs, files in os.walk(dir):  # 遍历该文件夹
        for file in files:  # 遍历刚获得的文件名files
            # print(file)
            (filename, extension) = os.path.splitext(file)  # 将文件名拆分为文件名与后缀
            if (extension == ext):  # 判断该后缀是否为.uvprojx文件
                return filename
    return None


def enter():
    print(">>>>>>")

def exit():
    print("<<<<<<")


def ota_package():
    enter()
    num = 1
    try:
        input_file = sys.argv[num]
        # print(input_file)
        if input_file.split('.')[-1] != "bin":
            print("input file is not *.bin file!")
            exit()
            return
        else:
            app_base_bin_file = "./" + input_file
            app_base_bin_filename = app_base_bin_file.split('/')[-1]
            app_base_bin_filepath = app_base_bin_file.split(app_base_bin_filename)[0]
            app_bin_filename = app_base_bin_filepath + app_base_bin_filename.split('.')[0] + ".ota"
            compress_file = app_base_bin_filepath + app_base_bin_filename.split('.')[0] + ".lz"
            num += 1
    except:
        app_base_bin_filename = look_file("./", ".bin")
        if not app_base_bin_filename:
            print("Not find .bin file!")
            exit()
            return
        else:
            print("find %s.bin file!" % app_base_bin_filename)
        app_base_bin_file = app_base_bin_filename + ".bin"
        app_bin_filename = app_base_bin_filename.split(".bin")[0] + ".ota"
        compress_file = app_base_bin_filename.split('.bin')[0] + ".lz"

    try:
        output_file = sys.argv[num]
        # print(output_file)
        if output_file.split('.')[-1] != "ota":
            print("output file is not *.ota file!")
            exit()
            return
        else:
            app_bin_file = "./" + output_file
            app_bin_filename = app_bin_file.split('/')[-1]
    except:
        app_bin_file = app_bin_defualt_filepath + app_bin_filename

    app_base_bin_size = os.path.getsize(app_base_bin_file)
    print("using the %s(size=%d) create %s" % (app_base_bin_file, app_base_bin_size, app_bin_file))
    # print("create %s start ..." % app_bin_filename)
    cfg_init()

    f_app_bin = open(app_bin_file, 'wb+')
    f_app_bin.write(b'\x00'*96)
    fw_info_hdr = 0xa5a5
    time_stamp = int(time.time())
    crypt_algo = int(INI_dict['crypt_type'])
    cmprs_algo = int(INI_dict['cmprs_type'])

    bytes_write_int(f_app_bin, 0, fw_info_hdr, 2)
    bytes_write_str(f_app_bin, 2, INI_dict['version'])

    bytes_write_int(f_app_bin, 18, time_stamp)
    bytes_write_str(f_app_bin, 42, INI_dict['product_code'])
    bytes_write_str(f_app_bin, 66, INI_dict['part_name'])
    bytes_write_int(f_app_bin, 81, 0xff, 1)
    bytes_write_int(f_app_bin, 82, cmprs_algo, 1)
    bytes_write_int(f_app_bin, 83, crypt_algo, 1)

    try:
        with open(app_base_bin_file, 'rb') as f_app_base:
            bytes_write_int(f_app_bin, 84, app_base_bin_size)

            f_app_base_content = f_app_base.read()
            raw_crc32 = binascii.crc32(f_app_base_content)
            bytes_write_int(f_app_bin, 88, raw_crc32)
            print("raw   crc32 = 0x%08x, size = %dbytes." % (raw_crc32, app_base_bin_size))
    except IOError:
        print("open %s failed!" % app_base_bin_file)
        print("create %s failed!" % app_bin_filename)

    if int(INI_dict['cmprs_type']) == CMPRS_NONE:
        cmpr_total_size = app_base_bin_size
        f_compress = open(app_base_bin_file, 'rb')
    elif int(INI_dict['cmprs_type']) == CMPRS_QUICKLZ:
        cmpr_total_size = qklz_compress(compress_file, app_base_bin_file)
        f_compress = open(compress_file, 'rb')
    else:
        cmpr_total_size = app_base_bin_size
        f_compress = open(app_base_bin_file, 'rb')

    f_compress_content = f_compress.read()
    f_compress.close()
    # try:
    #     os.remove(compress_file)
    # except:
    #     nop = 0
    bytes_write_int(f_app_bin, 92, cmpr_total_size)
    cmprs_crc32 = binascii.crc32(f_compress_content)
    bytes_write_int(f_app_bin, 96, cmprs_crc32)
    print("cmprs crc32 = 0x%08x, size = %dbytes." % (cmprs_crc32, cmpr_total_size))

    f_app_bin.seek(104)
    f_app_bin.write(f_compress_content)
    f_app_bin.close()

    f_app_bin = open(app_bin_file, 'rb+')
    f_app_head_content = f_app_bin.read(100)
    hdr_crc32 = binascii.crc32(f_app_head_content)
    bytes_write_int(f_app_bin, 100, hdr_crc32)
    f_app_bin.close()
    print("hdr   crc32 = 0x%08x, size = %dbytes." % (hdr_crc32, 92))

    app_bin_size = os.path.getsize(app_bin_file)
    print("create %s(size=%d) seccessfully!" % (app_bin_file, app_bin_size))

    exit()


if __name__ == '__main__':
    ota_package()


