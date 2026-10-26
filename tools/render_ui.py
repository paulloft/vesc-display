#!/usr/bin/env python3
"""Build the actual LVGL widgets for host layout verification; no MCU emulation."""
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor
import subprocess
import os

root = Path(__file__).resolve().parents[1]
os.chdir(root)
lvgl = root / '.pio/libdeps/jc3248w535/lvgl'
assert lvgl.exists(), 'Run pio pkg install first'
dest = root / 'build/host-ui-v9'
dest.mkdir(parents=True, exist_ok=True)
common = ['-O1', '-DLV_CONF_INCLUDE_SIMPLE', '-DLV_LVGL_H_INCLUDE_SIMPLE',
          '-Itests/ui_stubs', '-Iinclude', '-I' + str(lvgl), '-I' + str(lvgl/'include')]
sources = list((lvgl/'src').rglob('*.c')) + list((root/'src').glob('font_*.c')) + [root/'src/dashboard_background.c', root/'src/icon_assets.c', root/'src/UiAllocator.c', root/'src/Ui.cpp', root/'tests/ui_render.cpp']

def compile_one(pair):
    i, source = pair
    obj = dest / (str(i) + '.o')
    cpp = source.suffix == '.cpp'
    compiler = 'c++' if cpp else 'cc'
    # Recompile changed project files; the third-party library can stay cached.
    if not obj.exists() or source.stat().st_mtime > obj.stat().st_mtime or (root/'include/lv_conf.h').stat().st_mtime > obj.stat().st_mtime or cpp:
        compiled = subprocess.run([compiler, '-std=c++17' if cpp else '-std=c99', *common, '-c', str(source), '-o', str(obj)], capture_output=True, text=True)
        if compiled.returncode:
            raise RuntimeError(compiled.stderr)
    return str(obj)

with ThreadPoolExecutor(max_workers=8) as pool:
    objects = list(pool.map(compile_one, enumerate(sources)))
subprocess.run(['c++', *objects, '-o', str(dest/'render')], check=True)
subprocess.run([str(dest/'render')], check=True)
for name in ('main', 'settings', 'diagnostics', 'alerts', 'stats'):
    subprocess.run(['sips', '-s', 'format', 'png', f'build/ui-{name}.ppm', '--out', f'build/ui-{name}.png'], check=True, capture_output=True)
print('Rendered build/ui-{main,settings,diagnostics,alerts,stats}.png')
