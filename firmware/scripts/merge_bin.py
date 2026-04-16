"""
Post-build script: merges bootloader + partition table + app into a single
binary for esp-web-tools flashing from the browser.
Output: .pio/build/<env>/firmware-merged.bin
"""
Import("env")
import subprocess, os

def merge_bin(source, target, env):
    firmware_path = str(target[0])
    merged_path = firmware_path.replace(".elf", "-merged.bin").replace(
        os.path.basename(firmware_path), "firmware-merged.bin"
    )
    bootloader = os.path.join(env.subst("$BUILD_DIR"), "bootloader.bin")
    partitions = os.path.join(env.subst("$BUILD_DIR"), "partitions.bin")
    boot_app  = os.path.join(
        env.subst("$PACKAGE_DIR"),
        "tools", "partitions", "boot_app0.bin"
    )
    app = os.path.join(env.subst("$BUILD_DIR"), "firmware.bin")

    flash_args = env.subst("$UPLOADERFLAGS").split()
    # Find flash mode / freq from upload flags
    flash_mode  = "dio"
    flash_freq  = "80m"
    flash_size  = "4MB"
    for i, arg in enumerate(flash_args):
        if arg == "--flash_mode" and i+1 < len(flash_args):
            flash_mode = flash_args[i+1]
        if arg == "--flash_freq" and i+1 < len(flash_args):
            flash_freq = flash_args[i+1]

    cmd = [
        "python", "-m", "esptool",
        "--chip", "esp32s3",
        "merge_bin",
        "--flash_mode", flash_mode,
        "--flash_freq", flash_freq,
        "--flash_size", flash_size,
        "-o", merged_path,
        "0x0000",   bootloader,
        "0x8000",   partitions,
        "0xe000",   boot_app,
        "0x10000",  app,
    ]
    print("Merging firmware -> " + merged_path)
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print("merge_bin failed:", result.stderr)
    else:
        print("Merged binary created:", merged_path)
        # Copy to web-installer folder
        import shutil
        web_out = os.path.join(
            env.subst("$PROJECT_DIR"), "..", "web-installer", "firmware-merged.bin"
        )
        shutil.copy2(merged_path, web_out)
        print("Copied to web-installer/firmware-merged.bin")

env.AddPostAction("$BUILD_DIR/firmware.bin", merge_bin)
