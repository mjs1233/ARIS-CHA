//
// Created by takakura on 8/20/26.
//

#ifndef ARIS_RV32I_DECODER_HPP
#define ARIS_RV32I_DECODER_HPP
#include <cinttypes>
#include <expected>
#include "instruction.hpp"
#include "Global.hpp"
#include "bit_utils.hpp"

namespace aris {

    enum class DecodeError { illegal_opcode };

    class Decoder {
    public:
        std::expected<DecodedInstruction, DecodeError> operator()(word instruction) {
            switch (instruction & 0b1111111) {
            case Opcode::op:
                return op(instruction);
            case Opcode::op_imm:
                return op_imm(instruction);
            case Opcode::lui:
                return lui(instruction);
            case Opcode::auipc:
                return auipc(instruction);
            case Opcode::jal:
                return jal(instruction);
            case Opcode::jalr:
                return jalr(instruction);
            case Opcode::branch:
                return branch(instruction);
            case Opcode::load:
                return load(instruction);
            case Opcode::store:
                return store(instruction);
            case Opcode::misc_mem:
                return misc_mem(instruction);
            case Opcode::system:
                return system(instruction);
            default:
                return std::unexpected{DecodeError::illegal_opcode};
            }
        }

    private:
        std::expected<DecodedInstruction, DecodeError> op(word instruction) {
            byte rd = utils::take_rd(instruction);
            byte rs1 = utils::take_rs1(instruction);
            byte rs2 = utils::take_rs2(instruction);
            uint32_t funct3 = utils::take_funct3(instruction);
            uint32_t funct7 = utils::take_bits<25, 31>(instruction);

            switch (funct3) {
            case 0b000:
                if (funct7 == 0b0000000) {
                    return decoded(Op::add, rd, rs1, rs2, 0, instruction);
                }
                if (funct7 == 0b0100000) {
                    return decoded(Op::sub, rd, rs1, rs2, 0, instruction);
                }
                break;
            case 0b001:
                if (funct7 == 0b0000000) {
                    return decoded(Op::sll, rd, rs1, rs2, 0, instruction);
                }
                break;
            case 0b010:
                if (funct7 == 0b0000000) {
                    return decoded(Op::slt, rd, rs1, rs2, 0, instruction);
                }
                break;
            case 0b011:
                if (funct7 == 0b0000000) {
                    return decoded(Op::sltu, rd, rs1, rs2, 0, instruction);
                }
                break;
            case 0b100:
                if (funct7 == 0b0000000) {
                    return decoded(Op::xor_, rd, rs1, rs2, 0, instruction);
                }
                break;
            case 0b101:
                if (funct7 == 0b0000000) {
                    return decoded(Op::srl, rd, rs1, rs2, 0, instruction);
                }
                if (funct7 == 0b0100000) {
                    return decoded(Op::sra, rd, rs1, rs2, 0, instruction);
                }
                break;
            case 0b110:
                if (funct7 == 0b0000000) {
                    return decoded(Op::or_, rd, rs1, rs2, 0, instruction);
                }
                break;
            case 0b111:
                if (funct7 == 0b0000000) {
                    return decoded(Op::and_, rd, rs1, rs2, 0, instruction);
                }
                break;
            }
            return std::unexpected{DecodeError::illegal_opcode};
        }

        std::expected<DecodedInstruction, DecodeError> op_imm(word instruction) {
            byte rd = utils::take_rd(instruction);
            byte rs1 = utils::take_rs1(instruction);
            uint32_t funct3 = utils::take_funct3(instruction);
            word imm_type_0 = sign_extend<12>(utils::take_bits<20, 31>(instruction));
            word imm_shamt = utils::take_bits<20, 24>(instruction);
            uint32_t imm_type1 = utils::take_bits<25, 31>(instruction);

            switch (funct3) {
            case 0b000:
                return decoded(Op::addi, rd, rs1, 0, imm_type_0, instruction);
            case 0b010:
                return decoded(Op::slti, rd, rs1, 0, imm_type_0, instruction);
            case 0b011:
                return decoded(Op::sltiu, rd, rs1, 0, imm_type_0, instruction);
            case 0b100:
                return decoded(Op::xori, rd, rs1, 0, imm_type_0, instruction);
            case 0b110:
                return decoded(Op::ori, rd, rs1, 0, imm_type_0, instruction);
            case 0b111:
                return decoded(Op::andi, rd, rs1, 0, imm_type_0, instruction);
            case 0b001:
                if (imm_type1 == 0b0000000) {
                    return decoded(Op::slli, rd, rs1, 0, imm_shamt, instruction);
                }
                break;
            case 0b101:
                if (imm_type1 == 0b0000000) {
                    return decoded(Op::srli, rd, rs1, 0, imm_shamt, instruction);
                }
                if (imm_type1 == 0b0100000) {
                    return decoded(Op::srai, rd, rs1, 0, imm_shamt, instruction);
                }
                break;
            }
            return std::unexpected{DecodeError::illegal_opcode};
        }

        std::expected<DecodedInstruction, DecodeError> lui(word instruction) {
            return decoded(Op::lui, utils::take_rd(instruction), 0, 0, instruction & 0xfffff000, instruction);
        }

        std::expected<DecodedInstruction, DecodeError> auipc(word instruction) {
            return decoded(Op::auipc, utils::take_rd(instruction), 0, 0, instruction & 0xfffff000, instruction);
        }

        std::expected<DecodedInstruction, DecodeError> jal(word instruction) {
            word imm = (utils::take_bits<31, 31>(instruction) << 20)
                     | (utils::take_bits<12, 19>(instruction) << 12)
                     | (utils::take_bits<20, 20>(instruction) << 11)
                     | (utils::take_bits<21, 30>(instruction) << 1);
            return decoded(Op::jal, utils::take_rd(instruction), 0, 0, sign_extend<21>(imm), instruction);
        }

        std::expected<DecodedInstruction, DecodeError> jalr(word instruction) {
            if (utils::take_funct3(instruction) != 0b000) {
                return std::unexpected{DecodeError::illegal_opcode};
            }
            return decoded(Op::jalr, utils::take_rd(instruction), utils::take_rs1(instruction), 0,
                           sign_extend<12>(utils::take_bits<20, 31>(instruction)), instruction);
        }

        std::expected<DecodedInstruction, DecodeError> branch(word instruction) {
            word imm = (utils::take_bits<31, 31>(instruction) << 12)
                     | (utils::take_bits<7, 7>(instruction) << 11)
                     | (utils::take_bits<25, 30>(instruction) << 5)
                     | (utils::take_bits<8, 11>(instruction) << 1);
            Op operation;
            switch (utils::take_funct3(instruction)) {
            case 0b000:
                operation = Op::beq;
                break;
            case 0b001:
                operation = Op::bne;
                break;
            case 0b100:
                operation = Op::blt;
                break;
            case 0b101:
                operation = Op::bge;
                break;
            case 0b110:
                operation = Op::bltu;
                break;
            case 0b111:
                operation = Op::bgeu;
                break;
            default:
                return std::unexpected{DecodeError::illegal_opcode};
            }
            return decoded(operation, 0, utils::take_rs1(instruction), utils::take_rs2(instruction), sign_extend<13>(imm), instruction);
        }

        std::expected<DecodedInstruction, DecodeError> load(word instruction) {
            Op operation;
            switch (utils::take_funct3(instruction)) {
            case 0b000:
                operation = Op::lb;
                break;
            case 0b001:
                operation = Op::lh;
                break;
            case 0b010:
                operation = Op::lw;
                break;
            case 0b100:
                operation = Op::lbu;
                break;
            case 0b101:
                operation = Op::lhu;
                break;
            default:
                return std::unexpected{DecodeError::illegal_opcode};
            }
            return decoded(operation, utils::take_rd(instruction), utils::take_rs1(instruction), 0,
                           sign_extend<12>(utils::take_bits<20, 31>(instruction)), instruction);
        }

        std::expected<DecodedInstruction, DecodeError> store(word instruction) {
            Op operation;
            switch (utils::take_funct3(instruction)) {
            case 0b000:
                operation = Op::sb;
                break;
            case 0b001:
                operation = Op::sh;
                break;
            case 0b010:
                operation = Op::sw;
                break;
            default:
                return std::unexpected{DecodeError::illegal_opcode};
            }
            word imm = utils::take_bits<7, 11>(instruction) | (utils::take_bits<25, 31>(instruction) << 5);
            return decoded(operation, 0, utils::take_rs1(instruction), utils::take_rs2(instruction), sign_extend<12>(imm), instruction);
        }

        std::expected<DecodedInstruction, DecodeError> misc_mem(word instruction) {
            if (utils::take_funct3(instruction) != 0b000) {
                return std::unexpected{DecodeError::illegal_opcode};
            }
            return decoded(Op::fence, 0, 0, 0, 0, instruction);
        }

        std::expected<DecodedInstruction, DecodeError> system(word instruction) {
            if (instruction == 0x00000073) {
                return decoded(Op::ecall, 0, 0, 0, 0, instruction);
            }
            if (instruction == 0x00100073) {
                return decoded(Op::ebreak, 0, 0, 0, 0, instruction);
            }
            return std::unexpected{DecodeError::illegal_opcode};
        }

        template <byte Bits>
        static constexpr word sign_extend(word value) noexcept {
            static_assert(Bits > 0 && Bits < 32);
            constexpr word sign_bit = word{1} << (Bits - 1);
            return (value ^ sign_bit) - sign_bit;
        }

        static constexpr DecodedInstruction decoded(Op operation, byte rd, byte rs1, byte rs2, word imm, word raw) noexcept {
            return {.op = operation, .execution_target = execution_target(operation), .rd = rd, .rs1 = rs1, .rs2 = rs2, .imm = imm, .raw = raw};
        }

        static constexpr ExecutionTarget execution_target(Op operation) noexcept {
            switch (operation) {
            case Op::add:
            case Op::sub:
            case Op::sll:
            case Op::slt:
            case Op::sltu:
            case Op::xor_:
            case Op::srl:
            case Op::sra:
            case Op::or_:
            case Op::and_:
            case Op::addi:
            case Op::slti:
            case Op::sltiu:
            case Op::xori:
            case Op::ori:
            case Op::andi:
            case Op::slli:
            case Op::srli:
            case Op::srai:
            case Op::lui:
            case Op::auipc:
                return ExecutionTarget::alu;
            case Op::lb:
            case Op::lh:
            case Op::lw:
            case Op::lbu:
            case Op::lhu:
            case Op::sb:
            case Op::sh:
            case Op::sw:
                return ExecutionTarget::io;
            case Op::beq:
            case Op::bne:
            case Op::blt:
            case Op::bge:
            case Op::bltu:
            case Op::bgeu:
            case Op::jal:
            case Op::jalr:
                return ExecutionTarget::control;
            case Op::fence:
            case Op::ecall:
            case Op::ebreak:
                return ExecutionTarget::system;
            }
            return ExecutionTarget::system;
        }
    };

}
#endif //ARIS_RV32I_DECODER_HPP
