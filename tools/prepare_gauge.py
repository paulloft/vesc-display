"""Generate the dashboard's flat angular arc textures and a softly lit outer bezel.

The native LVGL arc masks the two RGB565 textures to the live speed. The
bezel is a static transparent image; the ticks remain native LVGL geometry.
No speed numbers, ticks or progress value are baked into these assets.
"""
from pathlib import Path
import math
from PIL import Image

FOLDER = Path(__file__).resolve().parents[1] / 'ebike_display/images'
SIZE = 221
CENTER = SIZE // 2


def mix(a, b, t):
    t = max(0., min(1., t))
    return tuple(round(x + (y - x) * t) for x, y in zip(a, b))


for active in (False, True):
    image = Image.new('RGB', (SIZE, SIZE))
    pixels = image.load()
    for y in range(SIZE):
        for x in range(SIZE):
            dx, dy = x - CENTER, y - CENTER
            # Color is constant across the band's thickness. Only the angle
            # changes it, so the arc stays flat instead of looking bevelled.
            angle = (math.degrees(math.atan2(dy, dx)) - 150) % 360
            along = min(angle / 240, 1)
            if active:
                warm = math.exp(-((along - .43) / .25) ** 2)
                color = mix((255, 67, 60), (255, 151, 104), warm)
            else:
                upper_light = math.exp(-((along - .6) / .32) ** 2)
                color = mix((15, 29, 42), (38, 61, 79), upper_light)
            pixels[x, y] = color
    image.save(FOLDER / ('gauge_active.png' if active else 'gauge_idle.png'))

# Subpixel rendering keeps the thin outer red ring smooth at 221 px.
SCALE = 4
BEZEL_EXTENSION_DEG = 8
BEZEL_FADE_DEG = 8
bezel = Image.new('RGBA', (SIZE * SCALE, SIZE * SCALE))
pixels = bezel.load()
for y in range(SIZE * SCALE):
    for x in range(SIZE * SCALE):
        dx = (x + .5) / SCALE - .5 - CENTER
        dy = (y + .5) / SCALE - .5 - CENTER
        radius = math.hypot(dx, dy)
        # Keep the original 240-degree span uniformly lit. Fade only the
        # short extensions beyond each end of the speed band.
        span = 240 + 2 * BEZEL_EXTENSION_DEG
        along = (math.degrees(math.atan2(dy, dx)) - (150 - BEZEL_EXTENSION_DEG)) % 360
        if along > span:
            continue
        fade = max(0., min(1., along / BEZEL_FADE_DEG, (span - along) / BEZEL_FADE_DEG))
        end_fade = fade * fade * (3 - 2 * fade)
        rim = math.exp(-((radius - 108.4) / .56) ** 2)
        glow = math.exp(-((radius - 108.0) / 1.9) ** 2)
        alpha = round(min(255., 190 * rim + 32 * glow) * end_fade)
        if alpha:
            pixels[x, y] = (255, 71, 66, alpha)
bezel.resize((SIZE, SIZE), Image.Resampling.LANCZOS).save(FOLDER / 'gauge_bezel.png')
print('Generated gauge_active.png, gauge_idle.png, gauge_bezel.png')
