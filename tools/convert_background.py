#!/usr/bin/env python3
"""Convert the generated background to the LCD's native RGB565 asset."""
from pathlib import Path
from PIL import Image
root = Path(__file__).resolve().parents[1]
image = Image.open(root/'assets/dashboard-background.png').convert('RGB').resize((480,320), Image.Resampling.LANCZOS)
# These PNGs are the LVGL Pro source of truth; export code after conversion.
image.save(root/'ebike_display/images/background.png')
from shutil import copyfile
copyfile(root/'assets/dashboard-background.png', root/'ebike_display/images/background-source.png')
data = bytearray()
for r,g,b in image.get_flattened_data():
    value = ((r>>3)<<11)|((g>>2)<<5)|(b>>3)
    data += value.to_bytes(2, 'little')
lines = [', '.join(f'0x{x:02x}' for x in data[i:i+24]) for i in range(0,len(data),24)]
source = '#include <lvgl.h>\n\nstatic const uint8_t pixels[] = {\n' + ',\n'.join(lines) + '\n};\n'
source += '''const lv_image_dsc_t dashboard_background = {
 .header = {.magic = LV_IMAGE_HEADER_MAGIC, .cf = LV_COLOR_FORMAT_RGB565, .w = 480, .h = 320, .stride = 960},
 .data_size = sizeof(pixels), .data = pixels
};
'''
(root/'src/dashboard_background.c').write_text(source)

print("Background updated in ebike_display/images. Run Export code only in LVGL Pro before building.")
