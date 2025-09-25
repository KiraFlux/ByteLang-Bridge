#pragma once

#include <array>
#include <functional>

#include <rs/Result.hpp>

#include "bytelang/core/streams.hpp"

namespace bytelang {
namespace bridge {

/// Ошибки обработки инструкций
enum class Error : rs::u32 {

    /// Полученный код инструкции не соответствует ни одной инструкции.
    UnknownInstruction,

    /// Не удалось считать код инструкции
    InstructionCodeReadFail,

    /// Не удалось отправить код инструкции
    InstructionCodeWriteFail,

    /// Обработчик исполнения инструкции отправки не указан
    InstructionSendHandlerIsNull,

    // для применения в пользовательских инструкциях

    /// Не удалось прочесть аргумент
    InstructionArgumentReadFail,

    /// Не удалось записать аргумент
    InstructionArgumentWriteFail
};

/// Слушатель инструкций (Принимает код и аргументы)
template<
    /// Тип кода принимаемой инструкции
    typename T,
    /// Количество инструкций
    rs::size N>
struct Receiver {

    /// Код инструкции на приём
    using Code = T;
    /// Количество инструкций
    static constexpr auto instruction_count = N;
    /// Контейнер для таблицы инструкций
    using InstructionTable = std::array<std::function<rs::Result<void, Error>(core::InputStream &)>, instruction_count>;

    /// Сериализатор
    core::InputStream in;
    /// Обработчики на приём
    InstructionTable handlers;

    /// Обновление (Пул проверки)
    rs::Result<void, Error> poll() {
        if (in.available() < sizeof(Code)) { return {}; }

        auto code_option = in.read<Code>();

        if (code_option.none()) { return {Error::InstructionCodeReadFail}; }
        if (code_option.value >= instruction_count) { return {Error::UnknownInstruction}; }

        return handlers[code_option.value](in);
    }

    // Запрет неявного создания
    Receiver() = delete;
};

/// Инструкция отправки
template<
    /// Примитив кода инструкции
    typename T>
// todo Args... / передаваемые аргументы
struct Instruction {

    /// Код инструкции на отправку
    using Code = T;
    /// Обработчик вызова инструкции
    using Handler = std::function<rs::Result<void, Error>(core::OutputStream &)>;// + Args ...

private:
    /// Сериализатор
    core::OutputStream &out;
    /// Обработчик вызова
    const Handler handler;
    /// Код инструкции
    const Code code;

public:
    Instruction(core::OutputStream &output_stream, Code code, Handler call_handler) :
        out{output_stream}, handler{std::move(call_handler)}, code{code} {}

    /// Вызвать инструкцию
    rs::Result<void, Error> operator()() {
        const bool write_ok = out.write(code);

        if (not write_ok) {
            return {Error::InstructionCodeWriteFail};
        }

        if (handler == nullptr) {
            return {Error::InstructionSendHandlerIsNull};
        }

        return handler(out);// handler(out, args...) Надо передать аргументы.....
    }

    Instruction() = delete;
};

/// Протокол отправки
template<
    /// Тип кода отправляемой инструкции
    typename T>
struct Sender {

    /// Тип кода отправляемой инструкции
    using Code = T;

private:
    /// Сериализатор
    core::OutputStream out;
    /// Счётчик инструкций
    Code next_code{0};

public:
    explicit Sender(core::OutputStream &&output_stream) :
        out{output_stream} {}

    /// Создать инструкцию отправки
    // todo + Args
    Instruction<Code> createInstruction(typename Instruction<Code>::Handler handler) {
        return Instruction<Code>{out, next_code++, std::move(handler)};
    }
};

}// namespace bridge
}// namespace bytelang
