#!/usr/bin/env python3
"""Package the current successful PlatformIO build without flashing a device."""
from pathlib import Path
import hashlib
import json
import os
import shutil
import subprocess

root = Path(__file__).resolve().parents[1]
build = root / '.pio/build/jc3248w535'
core = Path(os.environ.get('PLATFORMIO_CORE_DIR', '~/.platformio')).expanduser()
out = root / 'dist'
out.mkdir(exist_ok=True)
shutil.copyfile(build/'firmware.bin', out/'firmware.bin')
parts = [('0x0', build/'bootloader.bin'), ('0x8000', build/'partitions.bin'),
         ('0xe000', core/'packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin'),
         ('0x10000', build/'firmware.bin')]
tool = core/'packages/tool-esptool'/('esptool.exe' if os.name=='nt' else 'esptool')
if not tool.exists():
    raise SystemExit('PlatformIO 8 esptool is missing; run pio pkg install first')
command = [str(tool), '--chip', 'esp32s3', 'merge-bin', '-o', str(out/'first-flash.bin')]
for offset, file in parts:
    command += [offset, str(file)]
subprocess.run(command, check=True)
manifest = {'board': 'JC3248W535 N16R8', 'hardware_tested': False,
            'motor_commands_default': False, 'files': {}}
for name in ('firmware.bin', 'first-flash.bin'):
    blob = (out/name).read_bytes()
    manifest['files'][name] = {'bytes': len(blob), 'sha256': hashlib.sha256(blob).hexdigest()}
(out/'manifest.json').write_text(json.dumps(manifest, indent=2)+'\n')
print('Packaged dist/firmware.bin (OTA), dist/first-flash.bin (USB at 0x0)')
