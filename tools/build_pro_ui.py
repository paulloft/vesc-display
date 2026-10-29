"""Compile the native LVGL Pro export alongside the ESP32 backend."""
from pathlib import Path
import re
import sys
Import("env")
project = Path(env.subst("$PROJECT_DIR"))
ui = project / "ebike_display"
sys.path.insert(0, str(project / "tools"))
from pro_font_metrics import write_value_metrics
write_value_metrics(project)
# Use the editor's source list; old export artifacts are never globbed in.
source_list = (ui / "file_list_gen.cmake").read_text()
files = re.findall(r"\$\{CMAKE_CURRENT_LIST_DIR\}/([^\s)]+\.c)", source_list)
if not files or not (ui / "screens/app_gen.c").exists():
    raise RuntimeError("Export the UI in LVGL Pro first (hammer / Generate code)")
missing = [name for name in files if not (ui / name).is_file()]
if missing:
    raise RuntimeError("Incomplete LVGL Pro export: " + ", ".join(missing))
env.Append(CPPPATH=[str(ui), str(project / ".pio/libdeps" / env.subst("$PIOENV") / "lvgl")])
env.BuildSources(env.subst("$BUILD_DIR") + "/pro-ui", str(ui),
                 src_filter=["-<*>"] + ["+<" + name + ">" for name in files])
