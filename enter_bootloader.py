Import("env")
import os
import subprocess
import time

def enter_r4_bootloader(source, target, env):
    platform = env.PioPlatform()
    openocd_dir = platform.get_package_dir("tool-openocd")
    framework_dir = platform.get_package_dir("framework-arduinorenesas-uno")
    if not openocd_dir or not framework_dir:
        return

    openocd_exe = os.path.join(openocd_dir, "bin", "openocd.exe")
    variant_dir = os.path.join(framework_dir, "variants", "UNOWIFIR4")
    scripts_dir = os.path.join(openocd_dir, "openocd", "scripts")

    if os.path.isfile(openocd_exe) and os.path.isdir(variant_dir):
        print("Putting UNO R4 WiFi into bootloader mode via onboard CMSIS-DAP...")
        for _ in range(2):
            res = subprocess.run(
                [
                    openocd_exe,
                    "-s", variant_dir,
                    "-s", scripts_dir,
                    "-f", "openocd.cfg",
                    "-c", "cmsis_dap_vid_pid 0x2341 0x1002",
                    "-c", (
                        "init; reset halt; "
                        "mwh 0x4001E3FE 0xA502; "
                        "mwb 0x4001E500 0x35; mwb 0x4001E501 0x81; mwb 0x4001E502 0x73; mwb 0x4001E503 0x07; "
                        "mwh 0x4001E3FE 0xA500; "
                        "reset run; shutdown"
                    ),
                ],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
            )
            if res.returncode == 0:
                break
            time.sleep(0.2)

env.AddPreAction("upload", enter_r4_bootloader)
