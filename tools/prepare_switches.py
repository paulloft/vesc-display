"""Make antialiased native-size masks for the LVGL settings switches."""
from pathlib import Path
from PIL import Image, ImageDraw

folder = Path(__file__).resolve().parents[1] / 'ebike_display/images'
scale = 8
for name, width, height in [('switch_track', 66, 34), ('switch_knob', 26, 26)]:
    mask = Image.new('L', (width * scale, height * scale), 0)
    draw = ImageDraw.Draw(mask)
    draw.rounded_rectangle((0, 0, width * scale - 1, height * scale - 1),
                           radius=height * scale // 2, fill=255)
    mask = mask.resize((width, height), Image.Resampling.LANCZOS)
    mask = mask.point(lambda a: 0 if a < 3 else 255 if a > 252 else a)
    rgba = Image.new('RGBA', mask.size, (255, 255, 255, 0))
    rgba.putalpha(mask)
    rgba.save(folder / (name + '.png'))
