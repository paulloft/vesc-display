# Растровые иконки

Созданы встроенным imagegen по запросу пользователя. Финальные листы:
`source-sheet.png` (пиктограммы 3×3) и `source-arrows.png` (стрелки).
`source-brake.png` — отдельный круглый индикатор тормоза, заменивший надпись
«СТОП». Он красный при торможении, приглушённый в остальных состояниях;
центр совпадает с общей линией остальных индикаторов.
Два предварительных варианта с неудачной прозрачностью не использованы.

`tools/prepare_icons.py` нарезает листы, переносит чёрный фон в альфа-канал,
обрезает пустые поля, уменьшает символы и центрирует их в прозрачных ячейках.
Правая стрелка — зеркальная копия левой. Контуры программно не перерисовываются.
Результат: отдельные PNG и `src/icon_assets.c` для LVGL (RGB565 + alpha8).

Отключённые/включённые состояния окрашиваются LVGL без изменения прозрачности.
Деления заряда батареи остаются динамическими данными внутри растровой рамки.
Файлы включены в прошивку и не требуют SD.

Основные ячейки верхней панели 36×28, общая центральная линия y=14,
шаг между семью индикаторами 72 пикселя; стрелки находятся по краям.
Кнопка аварийки расположена в правой карточке вместо дублирующего лимита.
Её растр экспортируется из исходного листа в 64×56 (символ до 48×48),
отрисовывается в масштабе 1:1 без программного увеличения. Проверка видимых границ
с точностью до половины пикселя записана в `manifest.json`.

Промпт индикатора тормоза (встроенный imagegen):

Production raster automotive dashboard brake indicator icon. One perfectly centered symbol on a pure solid black square background. The standard brake pictogram: a thin-to-medium bold WHITE circular ring enclosing a centered WHITE exclamation mark, with two separated curved WHITE parenthesis-like brake-shoe arcs outside the ring, one left and one right. Symmetric and balanced. Same clean monoline style as premium car dashboard icons, smooth antialiased edges, uniform rounded stroke, strong legibility when reduced to 28 pixels high. Icon occupies central 65 percent of canvas with generous equal black margins on all sides. Pure white on pure black only. No glow, no shadow, no texture, no gray background, no stray pixels, no words, no letters, no STOP text, no labels, no border. Do not use a warning triangle; use a circular BRAKE symbol with exclamation and outer curved brackets.

Точный промпт финального основного листа:

Create a pristine raster sprite sheet: nine white automotive dashboard pictograms on a perfectly SOLID PURE BLACK background (#000000). Opaque image, no transparency. Square 3x3 equal grid, no drawn grid lines. Smooth clean white strokes, no texture, no distress, no speckles, no glow, no shadows, no gray backgrounds, absolutely no stray marks. The symbols must look like beautifully designed high quality vector icons rasterized. Uniform bold stroke width, rounded joins, precise optical centering in every cell, huge empty black gutters. Cell symbols occupy central 60% of each cell. Row one: left turn arrow outline; mirrored right turn arrow outline; symmetrical position lights automotive symbol (two opposed lamps and short outward rays). Row two: dipped beam headlamp with D shape and three diagonal rays leftward; high beam headlamp same shape and three horizontal rays leftward; hazard double nested triangle. Row three: warning triangle with exclamation mark; upright tall empty battery outline with centered terminal on top, no internal charge bars; upright thermometer with bulb and stem. Exactly these nine symbols, once each. No text, no labels, no numerals. Professional dashboard icon set, simple and legible at 28 pixels, consistent robust white silhouettes. Clean digitally drawn contours and pure black negative spaces.

Точный промпт финальных стрелок:

Production raster icon strip, 2 equal cells side by side, wide 2:1 canvas. Pure solid black background. In left cell a large simple straight LEFT-POINTING arrow outline. In right cell its exact horizontally mirrored RIGHT-POINTING arrow outline. Each arrow consists of a straight horizontal rectangular shaft exactly centered vertically, joined to a triangular arrowhead. Classic automotive dashboard turn-signal symbols. The shaft is a straight horizontal rectangle with no bends, hooks, downward extensions, vertical legs, or curves. Horizontal arrows like these Unicode symbols: ⇦ ⇨, but bold smooth white outlines with transparent-looking black interiors. Pure white thick uniform stroke, perfectly clean sharp geometry, smooth antialiased contours, rounded joins subtly. Perfect bilateral mirror pair. Center each arrow inside its own cell, equal large black margins. No glow, no texture, no stray pixels, no drop shadow, no text, no border, no other symbols. This is a raster asset, not a UI mockup.
