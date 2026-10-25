#pragma once
#include <stdint.h>

namespace board {
// JC3248W535 manufacturer's schematic. P1 numbering must be checked on PCB.
constexpr int vescTx = 43, vescRx = 44;
constexpr int ioSda = 17, ioScl = 18;
constexpr int keyAdc = 6, keyPullup = 9, keyPower = 7, keyMode = 15;
constexpr int touchSda = 4, touchScl = 8;
constexpr int sdCs = 10, sdMosi = 11, sdSck = 12, sdMiso = 13;
constexpr int backlight = 1;
constexpr uint8_t touchAddress = 0x3b;
// Join IO6 and IO9 externally. IO9 stays INPUT_PULLUP; never drive this net.
constexpr float keySupplyMv = 3300.0f;
constexpr uint32_t uartBaud = 115200;
constexpr uint32_t telemetryTimeoutMs = 1200;
}

