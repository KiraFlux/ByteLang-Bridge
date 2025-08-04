#pragma once

#include <Stream.h>
#include "rs/primitives.hpp"

#include "rs/Option.hpp"


namespace bytelang {
namespace core {


/// Входной поток (чтение данных)
struct InputStream {

private:

    Stream &stream;

public:

    explicit InputStream(Stream &s) :
        stream(s) {}

    /// Сколько байт доступно для чтения
    rs::size available() {
        return stream.available();
    }

    /// Прочитать один байт
    rs::Option<rs::u8> readByte() {
        const auto result = stream.read();

        if (result == -1) {
            return {};
        } else {
            return {static_cast<rs::u8>(result)};
        }
    }

    /// Прочитать объект типа T
    template<typename T> rs::Option<T> read() {
        T value;

        rs::size bytes_read = stream.readBytes(
            reinterpret_cast<rs::u8 *>(&value),
            sizeof(T)
        );

        if (bytes_read == sizeof(T)) {
            return {value};
        } else {
            return {};
        }
    }
};

/// Выходной поток (запись данных)
class OutputStream {
    Stream &stream;

public:
    explicit OutputStream(Stream &s) :
        stream(s) {}

    /// Записать один байт
    bool writeByte(rs::u8 byte) {
        return stream.write(byte) == 1;
    }

    /// Записать объект типа T
    template<typename T> bool write(const T &value) {
        return sizeof(T) != stream.write(
            reinterpret_cast<const rs::u8 *>(&value),
            sizeof(T)
        );
    }
};

} // namespace core
} // namespace bytelang
