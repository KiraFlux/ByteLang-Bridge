#pragma once

#include "rs/Result.hpp"
#include "bytelang/core/streams.hpp"


namespace bytelang {
namespace bridge {

/// Ошибки обработки инструкций
enum class InstructionError : rs::u32 {
    /// Полученный код инструкции не соответствует ни одной инструкции.
    UnknownInstruction,
    CodeReadError,
};


/// Слушатель инструкций (Принимает код и аргументы)
template<
    /// Тип кода принимаемой инструкции
    typename T,
    /// Количество инструкций
    rs::size instructions_count
> struct Receiver {

public:

    /// Сериализатор
    core::InputStream in;

    /// Обработчики на приём
    const std::function<void(core::InputStream &)> handlers[instructions_count];

    /// Обновление (Пул проверки)
    rs::Result<void, InstructionError> pull() {
        if (in.available() < sizeof(T)) {
            return {};
        }

        auto code_option = in.read<T>();

        if (code_option.none()) {
            return {InstructionError::CodeReadError};
        }

        if (code_option.value >= instructions_count) {
            return {InstructionError::UnknownInstruction};
        }

        handlers[code_option.value](in);

        return {};
    }

public:

    Receiver() = delete;

};

/// Инструкция отправки
template<
    /// Примитив кода инструкции
    typename T
> struct Instruction {

private:

    /// Код инструкции
    const T code;

    /// Сериализатор
    core::OutputStream &out;

public:

    Instruction(T code, core::OutputStream &output_stream) :
        code{code}, out{output_stream} {}

    /// Отправить заголовок инструкции и вернуть сериализатор
    rs::Option<core::OutputStream *> begin() const {
        if (out.write(code)) {
            return {&out};
        } else {
            return {};
        }
    }

    Instruction() = delete;
};

/// Протокол отправки
template<
    /// Тип кода отправляемой инструкции
    typename T
> struct Sender {

private:

    /// Сериализатор
    core::OutputStream out;

    /// Счётчик инструкций
    T next_code{0};

public:

    explicit Sender(core::OutputStream &&output_stream) :
        out{output_stream} {}

    /// Создать инструкцию отправки
    Instruction<T> createInstruction() {
        return Instruction<T>(next_code++, out);
    }
};


}
}

