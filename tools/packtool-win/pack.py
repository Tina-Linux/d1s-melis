import os
import sys
import glob
import shutil
import re
import subprocess
import argparse

parser = argparse.ArgumentParser()
parser.add_argument(
    "--top", help="Specify the top dir(defualt topdir=..\\..\\)")

args = parser.parse_args()

top = os.getcwd()
platform = {}
img_name = None
work_dir = None
pack_tools_dir = None
udisk_size = 0

phoenixplugin_file_list = [
    "tools\\phoenixplugin\\*.fex"
]

common_config_file_list = [
    "arisc.fex", "config.fex", "split_xxxx.fex", "sunxi.fex", "sys_config.fex",
    "package_boot0.cfg", "rootfs.ini"
]

special_fex_config_file = {
    'nor': ["sys_config_nor.fex", "sys_partition_nor.fex", "sys_partition_nor_dump.fex", "cardscript.fex"],
    'nand': ["sys_config_nand.fex", "sys_partition_nand.fex", "sys_partition_nand_dump.fex"],
    'card': ["cardscript.fex", "sys_config_card.fex", "sys_partition_card.fex", "sys_partition_card_dump.fex"],
    'card_product': ["cardscript_product.fex", "sys_config_card_product.fex", "sys_partition_card_product.fex"]
}

special_cfg_config_file = {
    'nor': ["image_nor.cfg", "package_uboot_nor.cfg"],
    'nand': ["image_nand.cfg", "package_uboot_nand.cfg"],
    'card': ["image_card.cfg", "package_uboot_card.cfg"],
    'card_product': ["image_card_product.cfg", "package_uboot_card_product.cfg"]
}

vesion_file_list = [
    "version_base.mk"
]


def check_file_exists(file):
    if not os.path.exists(file):
        print('%s not exists!' % file)
        os.system("pause")
        exit(1)


def cp(src, dst):
    check_file_exists(src)
    print("%-50s \t->\t %s" % (src, dst))
    shutil.copy(src, dst)


# 读取平台信息
def read_platform_info():
    global platform
    global work_dir
    global img_name
    global pack_tools_dir
    global top

    if args.top:
        top = args.top
    else:
        top += "\\..\\.."

    top = os.path.abspath(top)

    work_dir = top
    plat_file = top + "\\platform.txt"
    if not os.path.exists(plat_file):
        print("platform file not exists!")
        os.system('pause')
        exit(1)
    print("plat_file = %s" % (plat_file))
    with open(plat_file, 'r') as f:
        for line in f:
            line = line.rstrip()
            info = line.split('=')
            print("info = %s" % info)
            if len(info) == 2:
                platform[info[0]] = info[1]
    work_dir += "\\out\\"
    work_dir += platform['board']

    print("work_dir = %s" % work_dir)

    img_name = platform['platform'] + "_" + \
        platform['board'] + "_" + platform['debug']

    if not platform['sigmode'] == "none":
        img_name += "_" + platform['sigmode']
    if platform['mode'] == "dump":
        img_name += "_" + platform['mode']
    if platform['securemode'] == "secure":
        img_name += "_" + platform['securemode']
    if platform['torage_type'] == 'nor':
        img_name += "_" + platform['nor_volume'] + "Mnor"

    img_name += ".img"
    print("img_name = ", img_name)

    pack_tools_dir = top + "\\tools\\packtool-win\\"
    sys.path.append(pack_tools_dir)


def copy_pack_tools():
    global pack_tools_dir
    global work_dir

    image_dir = work_dir + "\\image\\"

    for file in glob.glob(pack_tools_dir + "\\*"):
        file_name = image_dir + os.path.basename(file)
        file = os.path.relpath(file)
        file_name = os.path.relpath(file_name)
        cp(file, file_name)


def remove_pack_tools():
    global pack_tools_dir
    global work_dir

    image_dir = work_dir + "\\image\\"
    for file in glob.glob(pack_tools_dir + "\\*"):
        file_name = image_dir + os.path.basename(file)
        os.remove(file_name)


def copy_commond_files(files, src_dir, dst_dir):
    for item in files:
        src = src_dir + item
        dst = dst_dir + item

        src = os.path.relpath(src)
        dst = os.path.relpath(dst)

        cp(src, dst)


def copy_specital_files(id, files, src_dir, dst_dir):
    if id not in files.keys():
        print(id + "Not support!!!")
        exit(1)
    for item in files[id]:
        src = src_dir + item
        dst = dst_dir + item

        src = os.path.relpath(src)
        dst = os.path.relpath(dst)

        cp(src, dst)


def copy_files():
    global top
    image_dir = work_dir + "\\image\\"

    config_file_dir = top + "\\projects\\" + platform["board"] + "\\configs\\"
    version_file_dir = top + "\\projects\\" + platform["board"] + "\\version\\"
    boot_file_dir = top + "\\projects\\" + platform["board"] + "\\bin\\"
    epos_file = top + "\\projects\\" + platform["board"] + "\\epos.img"

    if not os.path.exists(image_dir):
        os.makedirs(image_dir)
    else:
        for file in glob.glob(image_dir + "*"):
            os.remove(file)
    os.chdir(image_dir)
    print("\n\n----------------------------copy phoenixplugin fex file------------------------------")
    for item in phoenixplugin_file_list:
        for file in glob.glob(top + "\\" + item):
            name = os.path.basename(file)

            dst = image_dir + name

            src = os.path.relpath(file)
            dst = os.path.relpath(dst)

            cp(src, dst)

    print("\n\n----------------------------copy config fex file------------------------------")
    copy_commond_files(common_config_file_list, config_file_dir, image_dir)
    copy_specital_files(
        platform['torage_type'], special_fex_config_file, config_file_dir, image_dir)

    print("\n\n----------------------------copy config cfg file------------------------------")
    copy_specital_files(
        platform['torage_type'], special_cfg_config_file, config_file_dir, image_dir)

    print("\n\n----------------------------copy version mk file------------------------------")
    copy_commond_files(vesion_file_list, version_file_dir, image_dir)
    print("\n\n----------------------------copy boot bin file------------------------------")

    copy_file = boot_file_dir + "fes1_" + platform["chip"] + ".bin"
    dst = image_dir + "fes1.fex"
    src = os.path.relpath(copy_file)
    dst = os.path.relpath(dst)
    cp(src, dst)

    dst = image_dir + "epos.img"
    src = os.path.relpath(epos_file)
    dst = os.path.relpath(dst)
    cp(src, dst)

    if platform["torage_type"] == "card_product":
        copy_file = boot_file_dir + "boot0_" + platform["chip"] + "_card.bin"
        dst = image_dir + "boot0_card_product.fex"
        src = os.path.relpath(copy_file)
        dst = os.path.relpath(dst)
        cp(src, dst)

    elif platform["torage_type"] == "nor":
        copy_file = boot_file_dir + "boot0_" + platform["chip"] + "_nor.bin"
        dst = image_dir + "boot0_nor.fex"
        src = os.path.relpath(copy_file)
        dst = os.path.relpath(dst)
        cp(src, dst)

        copy_file = boot_file_dir + "u-boot_" + platform["chip"] + "_nor"
        copy_file += "_" + platform["debug"] + ".bin"
        dst = image_dir + "u-boot_nor.fex"
        src = os.path.relpath(copy_file)
        dst = os.path.relpath(dst)
        cp(src, dst)
    else:
        copy_file = boot_file_dir + "boot0_" + platform["chip"] + "_nand.bin"
        dst = image_dir + "boot0_nand.fex"
        src = os.path.relpath(copy_file)
        dst = os.path.relpath(dst)
        cp(src, dst)

        copy_file = boot_file_dir + "u-boot_" + platform["chip"] + "_nand.bin"
        dst = image_dir + "u-boot_nand.fex"
        src = os.path.relpath(copy_file)
        dst = os.path.relpath(dst)
        cp(src, dst)
    # boot0_card.fex must copy in order to support card burn
    copy_file = boot_file_dir + "boot0_" + platform["chip"] + "_card.bin"
    dst = image_dir + "boot0_card.fex"
    src = os.path.relpath(copy_file)
    dst = os.path.relpath(dst)
    cp(src, dst)

    if platform["securemode"] == "secure":
        print("\n\n----------------------------copy secure boot file------------------------------")
        copy_file = boot_file_dir + "sboot_" + platform["chip"] + ".bin"
        dst = image_dir + "sboot.bin"
        src = os.path.relpath(copy_file)
        dst = os.path.relpath(dst)
        cp(src, dst)


def del_match(target, match):
    lines = None

    with open(target, 'r', encoding='utf8') as f:
        lines = f.readlines()
    with open(target, 'w') as f:
        for l in lines:
            if re.match(match, l):
                continue
            f.write(l)


def set_img_name():
    global img_name
    image_cfg_file = work_dir + "\\image\\image_" + \
        platform["torage_type"] + ".cfg"

    lines = None

    with open(image_cfg_file, 'r') as f:
        lines = f.readlines()
    with open(image_cfg_file, 'w') as f:
        for l in lines:
            if re.match("(^imagename.*)|(^;.*)", l):
                continue
            f.write(l)
        f.write("imagename = " + img_name)
        f.write("\n\n")


def format_file(file, toformat='unix2dos'):
    if toformat == 'unix2dos':
        src = b'\r'
        dst = b'\r\n'
    else:
        src = b'\r\n'
        dst = b'\n'

    with open(file, 'rb+') as f1:
        byt = f1.read()

        byt = byt.replace(src, dst)
        tempfile = open(file + toformat, 'wb+')
        tempfile.write(byt)
        tempfile.close()

    os.remove(file)
    os.rename(file + toformat, file)


def copy_image_dir_file(src, dst):
    global work_dir

    image_dir = work_dir + "\\image\\"

    src = image_dir + src
    dst = image_dir + dst

    src = os.path.relpath(src)
    dst = os.path.relpath(dst)
    if src != dst:
        cp(src, dst)


def mv_image_dir_file(src, dst):
    global work_dir

    image_dir = work_dir + "\\image\\"

    if src != dst:
        print("%s -mv-> %s" % (src, dst))
        shutil.move(src, dst)


def exec_cmd(cmd, *args):
    global pack_tools_dir
    global work_dir

    image_dir = work_dir + "\\image\\"
    arg = " "
    exec_file = cmd
    #exec_file = os.path.relpath(exec_file)

    for a in args:
        if type(a) is str:
            a = os.path.relpath(a)
            arg += a + " "
        else:
            arg += str(a) + " "

    cmd_line = exec_file + arg

    print(cmd_line + "\n")

    if not os.path.exists(exec_file):
        print("!!!exec file %s not exits!!!" % (cmd))
        print("cmd line = %s" % cmd_line)
        os.system("pause")

    proc = subprocess.Popen(cmd_line, stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE, shell=True)
    outinfo, errinfo = proc.communicate()
    info = str(outinfo, encoding="utf-8")
    if 'error' in info or 'failed' in info:
        print("exec %s error!" % cmd_line)
        print(info)
        os.system("pause")
        exit(1)


# convert the fex file to bin file
def script_file(storage):
    global pack_tools_dir
    global work_dir

    image_dir = work_dir + "\\image\\"

    # prepare source files
    config_file_src = "sys_config_" + storage + ".fex"
    config_file_dst = "sys_config_" + storage + ".bin"

    del_match(image_dir + config_file_src, '(^;.*)|(^ +;.*)|(^$)')

    check_file_exists(config_file_src)
    exec_cmd("mode_script.exe", config_file_dst, config_file_src)

    cp(image_dir + config_file_dst, image_dir + "config_nor.fex")
    cp(image_dir + config_file_dst, image_dir + "sys_config.bin")

    sys_partition__src = "sys_partition_" + storage + ".fex"
    sys_partition_dst = "sys_partition_" + storage + ".bin"
    check_file_exists(sys_partition__src)
    exec_cmd("mode_script.exe", sys_partition_dst, sys_partition__src)


def update_boot0(storage='nor'):
    global work_dir

    image_dir = work_dir + "\\image\\"
    config_file = "sys_config_" + storage + ".bin"

    output_file = image_dir
    storage_type = "SPINOR_FLASH"

    if storage == "nor":
        output_file += "boot0_nor.fex "
        storage_type = "SPINOR_FLASH"
    elif storage == "nand":
        output_file += "boot0_nand.fex "
        storage_type = "NAND "
    elif storage == "card":
        output_file += "boot0_card.fex "
        storage_type = "SDMMC_CARD"
    else:
        output_file += "boot0_card_product.fex "
        storage_type = "SDMMC_CARD"

    input_file = image_dir + config_file

    check_file_exists(input_file)
    exec_cmd("update_boot0.exe", output_file, input_file, storage_type)


def package_boot0(storage):
    global work_dir

    image_dir = work_dir + "\\image\\"

    src = image_dir + "package_boot0.cfg"
    dst = "melis_pkg_" + storage + ".fex"
    if not os.path.exists(src):
        print("package_boot0.cfg file not exits")
        return
    check_file_exists("epos.img")
    exec_cmd("compress.exe", "-c", "epos.img", "epos.img.lzma")
    check_file_exists(src)
    exec_cmd("dragonsecboot.exe", "-pack", src)
    check_file_exists("boot_package.fex")
    mv_image_dir_file("boot_package.fex", dst)


def update_fes1(storage):
    global work_dir

    image_dir = work_dir + "\\image\\"
    config_file = "sys_config_" + storage + ".bin"

    src = image_dir + config_file
    dst = image_dir + "fes1.fex"
    if not os.path.exists(src):
        print("sys_config.fex file not exits")
        return

    exec_cmd("update_fes1.exe", dst, src)


def update_uboot(storage):
    global work_dir

    image_dir = work_dir + "\\image\\"
    config_file = "sys_config_" + storage + ".bin"

    src = image_dir + config_file
    dst = image_dir + "u-boot_" + storage + ".fex"
    if not os.path.exists(src):
        print("sys_config.fex file not exits")
        return

    check_file_exists(src)
    exec_cmd("update_uboot.exe", "-no_merge", dst, src)


def package_uboot(storage):
    global work_dir

    image_dir = work_dir + "\\image\\"
    file_name = "package_uboot_" + storage + ".cfg"
    src = image_dir + file_name
    dst = "boot_pkg_uboot_" + storage + ".fex"
    temp = image_dir + "boot_package.fex"
    config_file = image_dir + "sys_config_" + storage + ".bin"

    if not os.path.exists(src):
        print("package_boot0.cfg file not exits")
        return

    check_file_exists(src)
    exec_cmd("dragonsecboot.exe", "-pack", src)

    check_file_exists(temp)
    exec_cmd("update_toc1.exe", temp, config_file)
    mv_image_dir_file(temp, dst)


def update_env():
    global work_dir

    image_dir = work_dir + "\\image\\"

    src = image_dir + "env.cfg"
    dst = image_dir + "env.fex"

    if not os.path.exists(src):
        print("package_boot0.cfg file not exits")
        return

    check_file_exists(src)
    exec_cmd("mkenvimage.exe", "-r -p 0x00 -s 4096 -o", dst, src)


def get_part_info(file, total):
    start = False
    lines = None
    info = {}
    name = None
    print("get partition info from ", file)

    tempfile = file + '.tmp'
    with open(file, 'r', encoding='utf8') as f:
        lines = f.readlines()
    with open(tempfile, 'w') as f:
        for l in lines:
            if re.match("(^;.*)|(^ +;.*)|(^$)", l):
                continue
            if start:
                f.write(l)
            if "[partition_start]" in l:
                start = True
    with open(tempfile, 'r') as f:
        lines = f.readlines()
    part_str = ""
    for l in lines:
        if "[partition]" in l:
            part_str += '~'
        else:
            part_str += l
    for item in re.split('~', part_str):
        if item != "":
            part_info = re.split('=|\n+', item)
            del part_info[-1]
            for i in range(len(part_info)):
                part_info[i] = part_info[i].strip()

            if len(part_info) % 2 != 0:
                print("parse part info failed!")
                return None
            # find partition name
            for i in range(0, len(part_info), 2):
                if part_info[i].lower() == 'name':
                    name = part_info[i+1]
                    break
            iinfo = {}
            for i in range(0, len(part_info), 2):
                if part_info[i].lower() != 'name':
                    iinfo[part_info[i]] = part_info[i+1]
            info[name] = iinfo
    sum = 0
    for k, v in info.items():
        if k != 'UDISK':
            if 'size' in v.keys():
                s = int(v['size'])
                v['size'] = str(int(s / 2))
                sum += int(s / 2)
            else:
                print("parse part info failed!")
                return None
    for k, v in info.items():
        if k == 'UDISK':
            v['size'] = str(total - sum)
            udisk_size = (total - sum) * 1024 / 512

    return info, udisk_size


def perpare_for_nor():
    global work_dir
    global udisk_size

    image_dir = work_dir + "\\image\\"

    partition_file = image_dir + "sys_partition_nor.fex"
    part_kb_for_nor = int(platform['nor_volume'])*1024 - 64

    part_info, udisk_size = get_part_info(partition_file, part_kb_for_nor)

    # make fs
    name = top + "\\projects\\" + platform['board'] + "\\data\\UDISK"
    down_file = re.sub(r'"', "", part_info['ROOTFS']['downloadfile'])
    if not os.path.exists(name):
        print('not found %s to creating %s' % (name, down_file))
        return
    # minfs make name down_file rootfs.ini
    rootfs_ini = image_dir + "rootfs.ini"

    check_file_exists(rootfs_ini)
    exec_cmd("minfs.exe", "make",  name, down_file, rootfs_ini)

    return part_info


def updarte_mbr(storage, udisk_size, full_size):
    global work_dir

    image_dir = work_dir + "\\image\\"

    sys_partition_file = image_dir + "sys_partition_" + storage + ".bin"
    dlinfo_file = image_dir + "dlinfo.fex"
    gpt_file = image_dir + "sunxi_gpt.fex"
    mbr_source_file = image_dir + "sunxi_mbr_" + storage + ".fex"
    boot0_file_name = image_dir + "boot0_.fex"

    # update mbr file
    if not os.path.exists(sys_partition_file):
        print('not exists partition.fex file')
        exit(1)

    check_file_exists(sys_partition_file)
    if storage == 'nor':
        exec_cmd("update_mbr.exe",  sys_partition_file,
                 mbr_source_file, dlinfo_file, 1)
    else:
        exec_cmd("update_mbr.exe",  sys_partition_file,
                 mbr_source_file, dlinfo_file, 4)
    # convert mbr to get
    print("----------------mbr convert to gpt start---------------------")
    check_file_exists(mbr_source_file)
    exec_cmd("convert_gpt.exe", "-s",  mbr_source_file,
             "-o", gpt_file, "-l 96 -u 8")


def create_img(part_info):
    global work_dir
    global platform
    global img_name

    print("\n\n----------------------------create image------------------------------")

    storage = platform['torage_type']
    image_dir = work_dir + "\\image\\"

    print("image name = %s" % img_name)
    image_dst = work_dir + "\\" + img_name
    image_src = image_dir + img_name

    img_cfg_file = image_dir + "image_" + storage + ".cfg"
    sys_partition_file = image_dir + "sys_partition_" + storage + ".fex"

    # check file exists
    check_file_exists(img_cfg_file)
    check_file_exists(sys_partition_file)

    format_file(img_cfg_file, 'dos2unix')
    with open(img_cfg_file, 'r') as f:
        for line in f:
            if re.search('filename[ ]+=[ ]".*"', line):
                filename = re.sub('"', '', line.split(',')[
                                  0].split('=')[1]).strip()
                filedir = os.path.relpath(image_dir) + "\\" + filename
                if not os.path.exists(filedir):
                    print("%-30s <Failed>" % filename)
                else:
                    print("%-30s <Success>" % filename)
    if part_info == None:
        print('partition error!')
        exit(1)
    for k, v in part_info.items():
        if 'downloadfile' in v.keys():
            filename = v['downloadfile']
            filename = re.sub('"', '', filename)
            filedir = os.path.relpath(image_dir) + "\\" + filename
            if not os.path.exists(filedir):
                print("%-30s <Failed>" % filename)
            else:
                print("%-30s <Success>" % filename)

    exec_cmd("dragon.exe",  img_cfg_file, sys_partition_file)

    if not os.path.exists(image_src):
        print("Create image failed!")
    else:
        if os.path.exists(image_dst):
            os.remove(image_dst)
        shutil.move(image_src, image_dst)
        print("\n\n***************************************************************")
        print('Create image Success!')
        print("out dir = %s\n" % image_dst)
        print("***************************************************************")


def do_update(storage):
    print("\n\n----------------------------do update------------------------------")
    set_img_name()
    script_file(storage)
    update_boot0(storage)
    package_boot0(storage)

    update_fes1(storage)

    update_uboot(storage)
    package_uboot(storage)

    # update_env()


def do_finish(storage, part_info):
    global udisk_size

    updarte_mbr(storage, udisk_size, platform['nor_volume'])
    create_img(part_info)


if __name__ == "__main__":
    part_info = None

    read_platform_info()

    copy_files()
    copy_pack_tools()

    do_update(platform['torage_type'])

    if platform['torage_type'] == "nor":
        part_info = perpare_for_nor()
    do_finish(platform['torage_type'], part_info)
    remove_pack_tools()
    os.system("pause")
