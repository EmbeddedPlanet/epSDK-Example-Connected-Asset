#!/usr/bin/env python3

import os
import shutil
import argparse

#Get command line argument(s) and print help text as required
parser = argparse.ArgumentParser(description = "Compile application and merge with bootloader, soft device and settings hex")
parser.add_argument("-a", "--alt", help = "alternate directory offset (add prefix to '../bootloader/'", required = False, default = "")
parser.print_help()

argument = parser.parse_args()
status = False

if argument.alt:
    print("Looking for bootloader hex at: {0}".format(argument.alt))
    status = True
if not status:
    print("Run with a '-h' argument to see options")

print(argument.alt)

system_os = os.name
alt_path = argument.alt
bl_path = ""
app_path = "_build/nrf52840_xxaa.hex"
bl_settings_path = alt_path +  "../bootloader/settings_v0.0.1.hex"
sd_path = alt_path + "../nrf_sdk_17_1/components/softdevice/s140/hex/s140_nrf52_7.2.0_softdevice.hex"
output_path = "./_build/production_v00.00.00.hex"

bl_path = alt_path + "../bootloader/bootloader_v0.0.8_ConnectedAsset.hex"

# Name the build with the directory name
build_name = os.path.basename(os.getcwd())

# Get the current application version number out of the main.h file
with open('../source/main.h') as file:
    content = file.readlines()
    version_detected = False
    for line in content:
        if "VERSION_NUM" in line:
            version_detected = True
            app_ver = line.split()
            app_ver = app_ver[2].replace("\"", '')
            output_path = "_build/" + build_name + "_production_v" + app_ver + ".hex"
            build_file = build_name + "_build_" + app_ver + ".hex"
    if version_detected == False:
        print("Version number not detected! Defaulting to v00.00.00")
        print("Please set a #define for VERSION_NUM in main.h!")
        print("Ex: #define VERSION_NUM \"00.00.01\"")

# Remove the old output file.
try:
    os.remove(output_path)
except:
    pass

os.system("make")

os.system("mergehex -m  " + app_path + ' ' + bl_path + ' ' + bl_settings_path + ' ' + sd_path + " -o " + output_path)
