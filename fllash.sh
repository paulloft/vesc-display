#!/usr/bin/env bash
# Build and upload the complete PlatformIO image (including partition table).
set -euo pipefail

project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$project_dir"

usage() {
  cat <<'HELP'
Прошивка дисплея JC3248W535 по USB-C.

  ./fllash.sh                       Найти порт, собрать и прошить
  ./fllash.sh --port /dev/cu.usbmodemXXXX
  ./fllash.sh --monitor             После прошивки открыть монитор 115200 бод
  ./fllash.sh --help

Если USB-порт не появляется: удерживайте BOOT, нажмите RST, отпустите BOOT.
Выход из монитора: Ctrl+C.
HELP
}

port=""
monitor=false
while (($#)); do
  case "$1" in
    --port|-p)
      if (($# < 2)) || [[ -z "$2" || "$2" == -* ]]; then
        echo "После --port нужно указать порт." >&2; exit 2
      fi
      port="$2"; shift 2 ;;
    --monitor|-m) monitor=true; shift ;;
    --help|-h) usage; exit 0 ;;
    *) echo "Неизвестный аргумент: $1" >&2; usage >&2; exit 2 ;;
  esac
done

python_bin="$(command -v python3 || true)"
if [[ -z "$python_bin" ]]; then
  echo "Нужен Python 3 для PlatformIO. Установите Python 3 и запустите скрипт снова." >&2
  exit 1
fi

pio_bin=""
if [[ -x "$project_dir/.venv/bin/pio" ]]; then
  pio_bin="$project_dir/.venv/bin/pio"
elif command -v pio >/dev/null 2>&1; then
  pio_bin="$(command -v pio)"
elif command -v platformio >/dev/null 2>&1; then
  pio_bin="$(command -v platformio)"
elif [[ -x "${HOME}/.platformio/penv/bin/pio" ]]; then
  pio_bin="${HOME}/.platformio/penv/bin/pio"
elif [[ -x /tmp/ebike-build-venv/bin/pio ]]; then
  # Reuse the toolchain installed while developing this project, if available.
  pio_bin=/tmp/ebike-build-venv/bin/pio
  if [[ -z "${PLATFORMIO_CORE_DIR:-}" && -d /tmp/ebike-platformio ]]; then
    export PLATFORMIO_CORE_DIR=/tmp/ebike-platformio
  fi
else
  echo "Устанавливаю PlatformIO в .venv проекта (нужен доступ к интернету)…"
  "$python_bin" -m venv "$project_dir/.venv"
  "$project_dir/.venv/bin/python" -m pip install platformio==6.2.0
  pio_bin="$project_dir/.venv/bin/pio"
fi

if [[ -z "$port" ]]; then
  device_json="$("$pio_bin" device list --json-output)"
  candidates="$(printf '%s' "$device_json" | "$python_bin" -c '
import json,sys
for item in json.load(sys.stdin):
    device=item.get("port", "")
    hwid=item.get("hwid", "").upper()
    if device.startswith(("/dev/cu.usbmodem", "/dev/cu.usbserial", "/dev/ttyACM", "/dev/ttyUSB", "/dev/cu.SLAB_USBtoUART")) or "VID:PID=303A:" in hwid:
        print(device)
')"
  ports=()
  while IFS= read -r candidate; do
    [[ -z "$candidate" ]] || ports+=("$candidate")
  done <<< "$candidates"
  if ((${#ports[@]} == 0)); then
    echo "USB-порт ESP32 не найден. Подключите дисплей USB-C кабелем с передачей данных." >&2
    echo "Если порт не появился: удерживайте BOOT, нажмите RST, отпустите BOOT." >&2
    echo "Можно указать порт вручную: ./fllash.sh --port /dev/cu.usbmodemXXXX" >&2
    exit 1
  elif ((${#ports[@]} == 1)); then
    port="${ports[0]}"
  else
    if [[ ! -t 0 ]]; then
      printf 'Найдено несколько портов; укажите --port:\n' >&2
      printf '  %s\n' "${ports[@]}" >&2
      exit 1
    fi
    echo "Выберите USB-порт дисплея:"
    select selected_port in "${ports[@]}"; do
      if [[ -n "$selected_port" ]]; then port="$selected_port"; break; fi
      echo "Введите номер из списка."
    done
    [[ -n "$port" ]] || exit 1
  fi
fi

echo "Порт: $port"
echo "Собираю и прошиваю JC3248W535…"
"$pio_bin" run --environment jc3248w535 --target upload --upload-port "$port"
echo "Прошивка записана."
if "$monitor"; then
  exec "$pio_bin" device monitor --port "$port" --baud 115200
fi
