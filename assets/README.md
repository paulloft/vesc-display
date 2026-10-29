# Ресурсы оформления

`dashboard-background.png` создан встроенным инструментом imagegen 25.10.2026
для этого проекта. Это исходник фона, не изображение готового интерфейса.
Иконки — отдельные сгенерированные растры с прозрачностью, см. `icons/README.md`.
Показания и текст не запечены в картинку.
Фон преобразуется в RGB565 480×320 скриптом `tools/convert_background.py`
и обновляет `ebike_display/images/background.png`. После этого выполните
**Export code only** в LVGL Pro: ресурс войдёт в прошивку без SD.
`src/dashboard_background.c` оставлен для совместимости старого инструмента.

Точный промпт генерации (built-in imagegen):

Use case: stylized-concept. Asset type: production background image for a 480x320 landscape electric bicycle dashboard; this is BACKGROUND ART ONLY, not a UI mockup. Create a premium cinematic very dark alpine mountain road at blue hour, viewed from road level with a winding asphalt road entering bottom center and disappearing toward center horizon, pine forest silhouettes and layered jagged mountain peaks, subtle red-orange reflected light along the road and cold blue edges on distant peaks. Landscape 3:2 composition. The center and top must remain extremely dark and visually quiet so large white speed digits and a circular gauge overlaid by software remain clearly legible. The bottom center can have a restrained warm road glow. Side thirds quiet and dark for translucent dashboard panels. Palette nearly black navy, steel blue, restrained ember red, no bright sky, no sun, no vehicle. Sophisticated photorealistic digital matte painting, clean forms readable at tiny embedded-screen resolution. No text, no numbers, no gauge, no dashboard, no border, no icons, no logos, no watermark.


### Шкала спидометра

`tools/prepare_gauge.py` создаёт текстуры `gauge_active.png`, `gauge_idle.png`
и прозрачный обод `gauge_bezel.png` в `ebike_display/images/` (нужен Pillow).
Цвет меняется плавно только вдоль дуги: красно-оранжевая активная полоса
и сине-серая незаполненная часть. Поперёк толщины цвет постоянный, без блика
и затемнения краёв, чтобы полоса выглядела плоской. Текстуры
подключены как `arc_image_src`; угол заполнения остаётся живым значением LVGL.
Деления и подписи не запечены в изображениях: вложенные `lv_scale`
задают наружные отметки через 10 км/ч и внутренние подписи через 20 км/ч.
Внешний красный обод равномерный на всей дуге. С обоих концов он продлён
на 8° за полосу и плавно исчезает на этих продолжениях; параметры задаются
в `prepare_gauge.py` как `BEZEL_EXTENSION_DEG` и `BEZEL_FADE_DEG`.
После изменения ресурсов выполните Export code only в LVGL Pro.
