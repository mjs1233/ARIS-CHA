//
// Created by takakura on 8/20/26.
//

#ifndef ARIS_RV32I_DECODER_HPP
#define ARIS_RV32I_DECODER_HPP
#include <expected>
#include "Global.hpp"
#include "instruction.hpp"

namespace aris {

    enum class DecodeError { illegal_opcode };

    class Decoder {
    public:
        std::expected<DecodedInstruction, DecodeError> operator()(word instruction);

    private:
        std::expected<DecodedInstruction, DecodeError> op(word instruction, InstructionCategory category);
        std::expected<DecodedInstruction, DecodeError> op_imm(word instruction, InstructionCategory category);
        std::expected<DecodedInstruction, DecodeError> lui(word instruction, InstructionCategory category);
        std::expected<DecodedInstruction, DecodeError> auipc(word instruction, InstructionCategory category);
        std::expected<DecodedInstruction, DecodeError> jal(word instruction, InstructionCategory category);
        std::expected<DecodedInstruction, DecodeError> jalr(word instruction, InstructionCategory category);
        std::expected<DecodedInstruction, DecodeError> branch(word instruction, InstructionCategory category);
        std::expected<DecodedInstruction, DecodeError> load(word instruction, InstructionCategory category);
        std::expected<DecodedInstruction, DecodeError> store(word instruction, InstructionCategory category);
        std::expected<DecodedInstruction, DecodeError> misc_mem(word instruction, InstructionCategory category);
        std::expected<DecodedInstruction, DecodeError> system(word instruction, InstructionCategory category);

        template <byte Bits>
        static constexpr word sign_extend(word value) noexcept;

        static constexpr DecodedInstruction decoded(InstructionCategory category, Op operation, byte rd, byte rs1, byte rs2, word imm, word raw) noexcept;
    };
}
#endif // ARIS_RV32I_DECODER_HPP
