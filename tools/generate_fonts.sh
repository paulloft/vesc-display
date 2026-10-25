#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
for size in 14 20; do
  font=tools/fonts/NotoSans.ttf
  if [ "$size" = 20 ]; then font=tools/fonts/Dashboard-Bold.ttf; fi
  npx --yes --package=lv_font_conv@1.5.3 lv_font_conv --font "$font" \
    --size "$size" --bpp 4 --range '0x20-0x7f,0x401,0x410-0x44f,0x451,0xb0,0xb7,0x2013-0x2014,0x2026' \
    --format lvgl --no-compress --output "src/font_ru_$size.c"
done
npx --yes --package=lv_font_conv@1.5.3 lv_font_conv --font tools/fonts/Dashboard-Bold.ttf \
  --size 72 --bpp 4 --range '0x2d,0x30-0x39' --format lvgl --no-compress --output src/font_speed_72.c
npx --yes --package=lv_font_conv@1.5.3 lv_font_conv --font tools/fonts/Dashboard-Bold.ttf \
  --size 32 --bpp 4 --range '0x20-0x7f,0xb0,0x2014' --format lvgl --no-compress --output src/font_value_32.c
