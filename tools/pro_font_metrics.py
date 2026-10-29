"""Preserve the TTF line metrics used by LVGL Pro's FreeType preview.

lv_font_conv otherwise derives metrics from the exported glyph subset.
The numeric ui_value and ui_speed fonts use their original TTF metrics.
Uses the standard library so PlatformIO does not need Pillow or fontTools.
"""
from pathlib import Path
import struct
import xml.etree.ElementTree as ET


def write_value_metrics(root: Path) -> None:
    project = root / 'ebike_display'
    xml = ET.parse(project / 'globals.xml')
    result = '/* Generated from globals.xml and its TTF; do not edit. */\n#pragma once\n'
    for name, prefix in [('ui_value', 'EBIKE_VALUE'), ('ui_speed', 'EBIKE_SPEED')]:
        font = xml.find(f"fonts/bin[@name='{name}']")
        if font is None:
            raise RuntimeError(f'Missing {name} font in globals.xml')
        blob = (project / font.attrib['src_path']).read_bytes()
        size = int(font.attrib['size'])
        count = struct.unpack_from('>H', blob, 4)[0]
        tables = {}
        for index in range(count):
            tag, _, offset, _ = struct.unpack_from('>4sIII', blob, 12 + index * 16)
            tables[tag] = offset
        units = struct.unpack_from('>H', blob, tables[b'head'] + 18)[0]
        ascent, descent, gap = struct.unpack_from('>hhh', blob, tables[b'hhea'] + 4)
        # FreeType rounds the face height to pixels and the descender downwards.
        height = ((ascent - descent + gap) * size + units // 2) // units
        baseline = (-descent * size + units - 1) // units
        result += f'#define {prefix}_LINE_HEIGHT {height}\n#define {prefix}_BASE_LINE {baseline}\n'
    target = project / 'fonts/ui_value_metrics.h'
    if not target.exists() or target.read_text() != result:
        target.write_text(result)


if __name__ == '__main__':
    write_value_metrics(Path(__file__).resolve().parents[1])
