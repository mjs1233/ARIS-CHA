//
// Created by Codex on 8/21/26.
//

#ifndef ARIS_RV32I_EXECUTION_STAGE_HPP
#define ARIS_RV32I_EXECUTION_STAGE_HPP
#include <array>
#include <cstddef>
#include "RegisterFile.hpp"
#include "instruction.hpp"

namespace aris {

    inline constexpr std::size_t op_count = static_cast<std::size_t>(Op::ebreak) + 1;

    constexpr std::size_t op_index(Op operation) {
        return static_cast<std::size_t>(operation);
    }

    class IntegerRegisterStage {
    public:
        static IntegerRegisterStage& instance();
        void execute(RegisterFile& registers, DecodedInstruction& instruction) const;

    private:
        using Handler = void (*)(RegisterFile&, DecodedInstruction&);

        IntegerRegisterStage();

        static void add(RegisterFile& registers, DecodedInstruction& instruction);
        static void sub(RegisterFile& registers, DecodedInstruction& instruction);
        static void sll(RegisterFile& registers, DecodedInstruction& instruction);
        static void slt(RegisterFile& registers, DecodedInstruction& instruction);
        static void sltu(RegisterFile& registers, DecodedInstruction& instruction);
        static void xor_(RegisterFile& registers, DecodedInstruction& instruction);
        static void srl(RegisterFile& registers, DecodedInstruction& instruction);
        static void sra(RegisterFile& registers, DecodedInstruction& instruction);
        static void or_(RegisterFile& registers, DecodedInstruction& instruction);
        static void and_(RegisterFile& registers, DecodedInstruction& instruction);
        static void addi(RegisterFile& registers, DecodedInstruction& instruction);
        static void slti(RegisterFile& registers, DecodedInstruction& instruction);
        static void sltiu(RegisterFile& registers, DecodedInstruction& instruction);
        static void xori(RegisterFile& registers, DecodedInstruction& instruction);
        static void ori(RegisterFile& registers, DecodedInstruction& instruction);
        static void andi(RegisterFile& registers, DecodedInstruction& instruction);
        static void slli(RegisterFile& registers, DecodedInstruction& instruction);
        static void srli(RegisterFile& registers, DecodedInstruction& instruction);
        static void srai(RegisterFile& registers, DecodedInstruction& instruction);
        static void lui(RegisterFile& registers, DecodedInstruction& instruction);
        static void auipc(RegisterFile& registers, DecodedInstruction& instruction);

        std::array<Handler, op_count> m_handlers = {};
    };

    class MemoryAccessStage {
    public:
        static MemoryAccessStage& instance();
        void execute(RegisterFile& registers, DecodedInstruction& instruction) const;

    private:
        using Handler = void (*)(RegisterFile&, DecodedInstruction&);

        MemoryAccessStage();

        static void lb(RegisterFile& registers, DecodedInstruction& instruction);
        static void lh(RegisterFile& registers, DecodedInstruction& instruction);
        static void lw(RegisterFile& registers, DecodedInstruction& instruction);
        static void lbu(RegisterFile& registers, DecodedInstruction& instruction);
        static void lhu(RegisterFile& registers, DecodedInstruction& instruction);
        static void sb(RegisterFile& registers, DecodedInstruction& instruction);
        static void sh(RegisterFile& registers, DecodedInstruction& instruction);
        static void sw(RegisterFile& registers, DecodedInstruction& instruction);

        std::array<Handler, op_count> m_handlers = {};
    };

    class ControlFlowStage {
    public:
        static ControlFlowStage& instance();
        void execute(RegisterFile& registers, DecodedInstruction& instruction) const;

    private:
        using Handler = void (*)(RegisterFile&, DecodedInstruction&);

        ControlFlowStage();

        static void beq(RegisterFile& registers, DecodedInstruction& instruction);
        static void bne(RegisterFile& registers, DecodedInstruction& instruction);
        static void blt(RegisterFile& registers, DecodedInstruction& instruction);
        static void bge(RegisterFile& registers, DecodedInstruction& instruction);
        static void bltu(RegisterFile& registers, DecodedInstruction& instruction);
        static void bgeu(RegisterFile& registers, DecodedInstruction& instruction);
        static void jal(RegisterFile& registers, DecodedInstruction& instruction);
        static void jalr(RegisterFile& registers, DecodedInstruction& instruction);

        std::array<Handler, op_count> m_handlers = {};
    };

    class SystemStage {
    public:
        static SystemStage& instance();
        void execute(RegisterFile& registers, DecodedInstruction& instruction) const;

    private:
        using Handler = void (*)(RegisterFile&, DecodedInstruction&);

        SystemStage();

        static void fence(RegisterFile& registers, DecodedInstruction& instruction);
        static void ecall(RegisterFile& registers, DecodedInstruction& instruction);
        static void ebreak(RegisterFile& registers, DecodedInstruction& instruction);

        std::array<Handler, op_count> m_handlers = {};
    };
}
#endif // ARIS_RV32I_EXECUTION_STAGE_HPP
