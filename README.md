# ByteLang-Bridge-Lib

**Структурированный мост для embedded-коммуникации**

## Основные возможности

- **Приём команд**: `Receiver<CodeType, instructions_count>`
- **Отправка данных**: `Sender<CodeType>` + `Instruction`
- **Бинарные потоки**: `InputStream` / `OutputStream`

## Быстрый старт

Добавьте в `platformio.ini`:

```ini
lib_deps =
    https://github.com/JamahaW/ByteLang-Bridge-Lib.git
```

## Пример

```cpp
#include "Arduino.h"
#include "bytelang/bridge.hpp"


using bytelang::core::InputStream;
using bytelang::core::OutputStream;

using bytelang::bridge::Error;

// обработчик на приём инструкций
auto receiver = bytelang::bridge::Receiver<rs::u8, 3>{
    .in = InputStream(Serial),
    .handlers = {
        /// 0x00 : set_led(state: u8)
        [](InputStream &serializer) -> rs::Result<void, Error> {
            auto state = serializer.readByte();

            if (state.none()) {
                return {Error::InstructionArgumentReadFail};
            }

            // ...

            return {};
        },

        /// 0x01 : set_motors(left_pwm: u8, right_pwm: u8)
        [](InputStream &serializer) -> rs::Result<void, Error> {
            auto left = serializer.readByte();
            auto right = serializer.readByte();

            if (left.none() or right.none()) {
                return {Error::InstructionArgumentReadFail};
            }

            // ... логика включения моторов

            return {};
        },
    },
};

// отправитель инструкций
auto sender = bytelang::bridge::Sender<rs::u8>(OutputStream(Serial));

/// ir_sensors(distances_mm: [3]u16)
const auto ir_dist_sensors = sender.createInstruction();

void sendIrDistSensors() {
    auto ser = ir_dist_sensors.begin();
    if (ser.some()) {
        ser.value->write(rs::u16(100));
        ser.value->write(rs::u16(0));
        ser.value->write(rs::u16(50));
    }
}

void setup() {}

void loop() {
    auto result = receiver.pull();

    if (result.fail()) {
        // ... сообщение об ошибке
    }

    delay(10);
}
```

## Лицензия

MIT License - подробнее см. [LICENSE](LICENSE)