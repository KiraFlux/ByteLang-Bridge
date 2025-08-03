#pragma once

#include <Stream.h>
#include "rs/primitives.hpp"


namespace serialcmd {

/// Вспомогательный класс для чтения структур и примитивных типов из потока
struct Serializer {

    /// Используемый поток
    Stream &stream;

    explicit Serializer(Stream &stream) :
        stream(stream) {}

    /// Побайтово считать тип
    template<typename T> void read(T &destination) {
        this->stream.readBytes(reinterpret_cast<rs::u8 *>(&destination), sizeof(T));
    }

    /// Побайтово записать тип
    template<typename T> void write(T &&source) {
        this->stream.write(reinterpret_cast<const rs::u8 *>(&source), sizeof(T));
    }
};

}