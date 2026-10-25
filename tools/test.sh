#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build
${CXX:-c++} -std=c++17 -Wall -Wextra -Werror -fsanitize=address,undefined -Iinclude src/Core.cpp tests/core_tests.cpp -o build/core-tests
./build/core-tests
${CXX:-c++} -std=c++17 -Wall -Wextra -Werror -fsanitize=address,undefined -Itests/ui_stubs -Iinclude src/Core.cpp src/Vesc.cpp tests/vesc_tests.cpp -o build/vesc-tests
./build/vesc-tests
${CXX:-c++} -std=c++17 -Wall -Wextra -Werror -fsanitize=address,undefined -DARDUINOJSON_ENABLE_ARDUINO_STRING=1 -Itests/ui_stubs -Iinclude -I.pio/libdeps/jc3248w535/ArduinoJson/src src/Core.cpp src/Storage.cpp tests/storage_tests.cpp -o build/storage-tests
./build/storage-tests
${CXX:-c++} -std=c++17 -Wall -Wextra -Werror -fsanitize=address,undefined -Iinclude tests/resistive_tests.cpp -o build/resistive-tests
./build/resistive-tests
