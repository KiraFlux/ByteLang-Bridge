#pragma once

#include <functional>
#include "rs/Result.hpp"
#include "bytelang/core/streams.hpp"


namespace bytelang {
namespace bridge {

/// Ошибки обработки инструкций
enum class Error : rs::u32 {
    /// Полученный код инструкции не соответствует ни одной инструкции.
    UnknownInstruction,
    /// Не удалось считать код инструкции
    InstructionCodeReadFail,

    // для применения в пользовательских инструкциях

    /// Не удалось прочесть аргумент
    InstructionArgumentReadFail,

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
    const std::function<rs::Result<void, Error>(core::InputStream &)> handlers[instructions_count];

    /// Обновление (Пул проверки)
    rs::Result<void, Error> poll() {
        if (in.available() < sizeof(T)) {
            return {};
        }

        auto code_option = in.read<T>();

        if (code_option.none()) {
            return {Error::InstructionCodeReadFail};
        }

        if (code_option.value >= instructions_count) {
            return {Error::UnknownInstruction};
        }

        return handlers[code_option.value](in);
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

