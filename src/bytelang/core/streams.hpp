#pragma once

#include <Stream.h>
#include <rs/aliases.hpp>
#include <rs/Option.hpp>


namespace bytelang::core {

/// Входной поток (чтение данных)
struct InputStream {

private:
    Stream &stream;

public:
    explicit InputStream(Stream &s) :
        stream{s} {}

    /// Сколько байт доступно для чтения
    [[nodiscard]] rs::size available() {
        return stream.available();
    }

    /// Прочитать один байт
    [[nodiscard]] rs::Option<rs::u8> readByte() {
        const auto result = stream.read();

        if (result == -1) {
            return {};
        } else {
            return {static_cast<rs::u8>(result)};
        }
    }

    /// Прочитать объект типа T
    template<typename T> [[nodiscard]] rs::Option<T> read() {
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
struct OutputStream {

private:
    Stream &stream;

public:
    explicit OutputStream(Stream &s) :
        stream{s} {}

    /// Записать один байт
    [[nodiscard]] bool writeByte(rs::u8 byte) {
        return stream.write(byte) == 1;
    }

    /// Записать буфер
    [[nodiscard]] bool write(const void *data, rs::size length) {
        return length == stream.write(static_cast<const rs::u8 *>(data), length);
    }

    /// Записать объект типа T
    template<typename T> [[nodiscard]] inline bool write(const T &value) {
        return write(static_cast<const void *>(&value), sizeof(T));
    }
};

} // namespace bytelang::core
