//
// Created by Codex on 8/20/26.
//

#ifndef ARIS_RV32I_EXECUTION_UNIT_HPP
#define ARIS_RV32I_EXECUTION_UNIT_HPP
#include <array>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include "RAM.hpp"
#include "instruction.hpp"

namespace aris {

    class ExecutionUnit {
    public:
        using RegisterFile = std::array<word, 32>;

        explicit ExecutionUnit(std::ostream& output = std::cout) : m_output(output) {}

        void execute(const DecodedInstruction& instruction, RegisterFile& registers, RAM& memory) {
            const auto index = static_cast<size_t>(instruction.op);
            (this->*handler_table[index])(instruction, registers, memory);
        }

    private:
        using Handler = void (ExecutionUnit::*)(const DecodedInstruction&, RegisterFile&, RAM&);

        void add(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("add", instruction);
        }

        void sub(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("sub", instruction);
        }

        void sll(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("sll", instruction);
        }

        void slt(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("slt", instruction);
        }

        void sltu(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("sltu", instruction);
        }

        void xor_(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("xor", instruction);
        }

        void srl(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("srl", instruction);
        }

        void sra(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("sra", instruction);
        }

        void or_(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("or", instruction);
        }

        void and_(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("and", instruction);
        }

        void addi(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("addi", instruction);
        }

        void slti(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("slti", instruction);
        }

        void sltiu(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("sltiu", instruction);
        }

        void xori(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("xori", instruction);
        }

        void ori(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("ori", instruction);
        }

        void andi(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("andi", instruction);
        }

        void slli(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("slli", instruction);
        }

        void srli(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("srli", instruction);
        }

        void srai(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("srai", instruction);
        }

        void lb(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("lb", instruction);
        }

        void lh(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("lh", instruction);
        }

        void lw(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("lw", instruction);
        }

        void lbu(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("lbu", instruction);
        }

        void lhu(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("lhu", instruction);
        }

        void sb(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("sb", instruction);
        }

        void sh(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("sh", instruction);
        }

        void sw(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("sw", instruction);
        }

        void beq(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("beq", instruction);
        }

        void bne(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("bne", instruction);
        }

        void blt(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("blt", instruction);
        }

        void bge(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("bge", instruction);
        }

        void bltu(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("bltu", instruction);
        }

        void bgeu(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("bgeu", instruction);
        }

        void jal(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("jal", instruction);
        }

        void jalr(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("jalr", instruction);
        }

        void lui(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("lui", instruction);
        }

        void auipc(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("auipc", instruction);
        }

        void fence(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("fence", instruction);
        }

        void ecall(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("ecall", instruction);
        }

        void ebreak(const DecodedInstruction& instruction, RegisterFile&, RAM&) {
            print("ebreak", instruction);
        }

        void print(const char* name, const DecodedInstruction& instruction) {
            m_output << name
                     << "(rd=" << static_cast<unsigned int>(instruction.rd)
                     << ", rs1=" << static_cast<unsigned int>(instruction.rs1)
                     << ", rs2=" << static_cast<unsigned int>(instruction.rs2)
                     << ", imm=0x" << std::hex << std::setw(8) << std::setfill('0') << instruction.imm
                     << ", raw=0x" << std::setw(8) << instruction.raw
                     << std::dec << std::setfill(' ') << ")\n";
        }

        inline static constexpr std::array<Handler, static_cast<size_t>(Op::ebreak) + 1> handler_table = {
            &ExecutionUnit::add,
            &ExecutionUnit::sub,
            &ExecutionUnit::sll,
            &ExecutionUnit::slt,
            &ExecutionUnit::sltu,
            &ExecutionUnit::xor_,
            &ExecutionUnit::srl,
            &ExecutionUnit::sra,
            &ExecutionUnit::or_,
            &ExecutionUnit::and_,
            &ExecutionUnit::addi,
            &ExecutionUnit::slti,
            &ExecutionUnit::sltiu,
            &ExecutionUnit::xori,
            &ExecutionUnit::ori,
            &ExecutionUnit::andi,
            &ExecutionUnit::slli,
            &ExecutionUnit::srli,
            &ExecutionUnit::srai,
            &ExecutionUnit::lb,
            &ExecutionUnit::lh,
            &ExecutionUnit::lw,
            &ExecutionUnit::lbu,
            &ExecutionUnit::lhu,
            &ExecutionUnit::sb,
            &ExecutionUnit::sh,
            &ExecutionUnit::sw,
            &ExecutionUnit::beq,
            &ExecutionUnit::bne,
            &ExecutionUnit::blt,
            &ExecutionUnit::bge,
            &ExecutionUnit::bltu,
            &ExecutionUnit::bgeu,
            &ExecutionUnit::jal,
            &ExecutionUnit::jalr,
            &ExecutionUnit::lui,
            &ExecutionUnit::auipc,
            &ExecutionUnit::fence,
            &ExecutionUnit::ecall,
            &ExecutionUnit::ebreak
        };

        std::ostream& m_output;
    };
}
#endif // ARIS_RV32I_EXECUTION_UNIT_HPP
