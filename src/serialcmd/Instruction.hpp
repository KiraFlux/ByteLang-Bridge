#pragma once

#include "serialcmd/Serializer.hpp"


namespace serialcmd {

/// Инструкция

template<typename T> struct Instruction {

private:

    /// Код инструкции
    const T code;
    /// Сериализатор
    Serializer &serializer;

public:

    Instruction(T code, Serializer &s) :
        code{code}, serializer{s} {}

    /// Отправить заголовок инструкции и вернуть сериализатор
    Serializer &begin() {
        serializer.write(code);
        return serializer;
    }

    Instruction() = delete;
};


}