Import("env")
import os
import subprocess

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
        subprocess.run(
            [
                openocd_exe,
                "-s", variant_dir,
                "-s", scripts_dir,
                "-f", "openocd.cfg",
                "-c", "cmsis_dap_vid_pid 0x2341 0x1002",
                "-c", "init; halt; mwh 0x4001E3FE 0xA502; mww 0x4001E500 0x07738135; mwh 0x4001E3FE 0xA500; reset run; shutdown"
            ],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )

env.AddPreAction("upload", enter_r4_bootloader)
