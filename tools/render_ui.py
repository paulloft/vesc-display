#!/usr/bin/env python3
"""Build the actual LVGL widgets for host layout verification; no MCU emulation."""
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor
import subprocess
import os
import re
from pro_font_metrics import write_value_metrics

root = Path(__file__).resolve().parents[1]
os.chdir(root)
write_value_metrics(root)
lvgl = root / '.pio/libdeps/jc3248w535/lvgl'
assert lvgl.exists(), 'Run pio pkg install first'
dest = root / 'build/host-ui-pro'
dest.mkdir(parents=True, exist_ok=True)
common = ['-O1', '-DLV_CONF_INCLUDE_SIMPLE', '-DLV_LVGL_H_INCLUDE_SIMPLE',
          '-Itests/ui_stubs', '-Iinclude', '-Iebike_display', '-I' + str(lvgl), '-I' + str(lvgl/'include')]
export_list=(root/'ebike_display/file_list_gen.cmake').read_text()
export_sources=[root/'ebike_display'/name for name in re.findall(r"\$\{CMAKE_CURRENT_LIST_DIR\}/([^\s)]+\.c)",export_list)]
sources = sorted((lvgl/'src').rglob('*.c')) + export_sources + [root/'src/UiAllocator.c', root/'src/Ui.cpp', root/'tests/ui_render.cpp']

def compile_one(pair):
    i, source = pair
    obj = dest / (str(i) + '.o')
    cpp = source.suffix == '.cpp'
    compiler = 'c++' if cpp else 'cc'
    # Recompile changed project files; the third-party library can stay cached.
    if not obj.exists() or source.stat().st_mtime > obj.stat().st_mtime or (root/'include/lv_conf.h').stat().st_mtime > obj.stat().st_mtime or cpp or (source == root/'ebike_display/ebike_display.c' and (root/'ebike_display/fonts/ui_value_metrics.h').stat().st_mtime > obj.stat().st_mtime):
        compiled = subprocess.run([compiler, '-std=c++17' if cpp else '-std=c99', *common, '-c', str(source), '-o', str(obj)], capture_output=True, text=True)
        if compiled.returncode:
            raise RuntimeError(compiled.stderr)
    return str(obj)

with ThreadPoolExecutor(max_workers=8) as pool:
    objects = list(pool.map(compile_one, enumerate(sources)))
subprocess.run(['c++', *objects, '-o', str(dest/'render')], check=True)
subprocess.run([str(dest/'render')], check=True)
for restored_page in range(3):
    subprocess.run([str(dest/'render'), str(restored_page)], check=True)
for name in ('main', 'settings', 'diagnostics', 'alerts', 'stats', 'switches', 'voltage', 'power', 'safe'):
    subprocess.run(['sips', '-s', 'format', 'png', f'build/ui-{name}.ppm', '--out', f'build/ui-{name}.png'], check=True, capture_output=True)
print('Rendered build/ui-{main,settings,diagnostics,alerts,stats,switches,voltage,power}.png')
