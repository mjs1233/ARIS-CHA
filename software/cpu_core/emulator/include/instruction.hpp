//
// Created by takakura on 8/20/26.
//

#ifndef ARIS_RV32I_INSTRUCTION_HPP
#define ARIS_RV32I_INSTRUCTION_HPP
#include "Global.hpp"
namespace aris {

    enum class Op : std::uint8_t {
        // R-type
        add, sub, sll, slt, sltu, xor_, srl, sra, or_, and_,

        // I-type ALU
        addi, slti, sltiu, xori, ori, andi, slli, srli, srai,

        // Load / store
        lb, lh, lw, lbu, lhu,
        sb, sh, sw,

        // Control flow
        beq, bne, blt, bge, bltu, bgeu,
        jal, jalr,

        // U-type
        lui, auipc,

        // Base-system
        fence, ecall, ebreak
    };

    enum class InstructionCategory : std::uint8_t {
        integer_register,
        memory_access,
        control_flow,
        system
    };

    struct DecodedInstruction {
        Op op;
        InstructionCategory category;
        word  rd  = 0;
        byte  rs1 = 0;
        byte  rs2 = 0;
        word imm = 0; // 이미 sign-extension된 32-bit bit pattern
        word raw = 0; // trace/debug용
    };


    namespace Opcode {
        enum table : std::uint8_t {

            op_imm = 0b0010011,
            lui = 0b0110111,
            auipc = 0b0010111,
            op = 0b0110011,
            jal = 0b1101111,
            jalr = 0b1100111,
            branch = 0b1100011,
            load = 0b0000011,
            store = 0b0100011,
            misc_mem = 0b0001111,
            system = 0b1110011
        };
    }
}

#endif //ARIS_RV32I_INSTRUCTION_HPP
