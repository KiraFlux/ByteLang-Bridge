#pragma once

#include <functional>
#include <vector>

#include "serialcmd/Serializer.hpp"
#include "serialcmd/Instruction.hpp"

#include "rs/primitives.hpp"


namespace serialcmd {
namespace abc {

/// Протокол P2P общения
template<
    typename SendInstructionCode, ///< Тип кода отправляемой инструкции
    typename ReceiveInstructionCode ///< Тип кода принимаемой инструкции
> struct Protocol {

public:

    using onReceiveFunction = std::function<void(Serializer &)>;
    using ProtocolInstruction = Instruction<SendInstructionCode>;

private:

    /// Сериализатор
    Serializer serializer;
    /// Обработчики приёма данных
    std::vector<onReceiveFunction> receive_handlers{};
    /// Обработчики отправки данных
    SendInstructionCode senders{0};

public:

    explicit Protocol(Stream &stream) :
        serializer{stream} {}

    /// Обновление
    void pull() {
        if (serializer.stream.available() < sizeof(ReceiveInstructionCode)) { return; }

        ReceiveInstructionCode code;
        serializer.read(code);

        if (code >= receive_handlers.size()) { return; }

        receive_handlers[code](serializer);
    }

    /// Зарегистрировать обработчик приёма
    void registerReceiver(onReceiveFunction &&handler) {
        receive_handlers.push_back(std::move(handler));
    }

protected:

    /// Создать инструкцию отправки
    ProtocolInstruction createSender() {
        auto instruction = ProtocolInstruction(SendInstructionCode(senders), serializer);
        senders += 1;
        return instruction;
    }

public:

    Protocol() = delete;

};


}
}