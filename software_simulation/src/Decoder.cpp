//
// Created by takakura on 8/20/26.
//

#include "Decoder.hpp"
#include "bit_utils.hpp"

namespace aris {

        std::expected<DecodedInstruction, DecodeError> Decoder::operator()(word instruction) {
            switch (instruction & 0b1111111) {
            case Opcode::op:
                return op(instruction, InstructionCategory::integer_register);
            case Opcode::op_imm:
                return op_imm(instruction, InstructionCategory::integer_register);
            case Opcode::lui:
                return lui(instruction, InstructionCategory::integer_register);
            case Opcode::auipc:
                return auipc(instruction, InstructionCategory::integer_register);
            case Opcode::jal:
                return jal(instruction, InstructionCategory::control_flow);
            case Opcode::jalr:
                return jalr(instruction, InstructionCategory::control_flow);
            case Opcode::branch:
                return branch(instruction, InstructionCategory::control_flow);
            case Opcode::load:
                return load(instruction, InstructionCategory::memory_access);
            case Opcode::store:
                return store(instruction, InstructionCategory::memory_access);
            case Opcode::misc_mem:
                return misc_mem(instruction, InstructionCategory::system);
            case Opcode::system:
                return system(instruction, InstructionCategory::system);
            default:
                return std::unexpected{DecodeError::illegal_opcode};
            }
        }

        std::expected<DecodedInstruction, DecodeError> Decoder::op(word instruction, InstructionCategory category) {
            byte rd = utils::take_rd(instruction);
            byte rs1 = utils::take_rs1(instruction);
            byte rs2 = utils::take_rs2(instruction);
            uint32_t funct3 = utils::take_funct3(instruction);
            uint32_t funct7 = utils::take_bits<25, 31>(instruction);

            switch (funct3) {
            case 0b000:
                if (funct7 == 0b0000000) {
                    return decoded(category, Op::add, rd, rs1, rs2, 0, instruction);
                }
                if (funct7 == 0b0100000) {
                    return decoded(category, Op::sub, rd, rs1, rs2, 0, instruction);
                }
                break;
            case 0b001:
                if (funct7 == 0b0000000) {
                    return decoded(category, Op::sll, rd, rs1, rs2, 0, instruction);
                }
                break;
            case 0b010:
                if (funct7 == 0b0000000) {
                    return decoded(category, Op::slt, rd, rs1, rs2, 0, instruction);
                }
                break;
            case 0b011:
                if (funct7 == 0b0000000) {
                    return decoded(category, Op::sltu, rd, rs1, rs2, 0, instruction);
                }
                break;
            case 0b100:
                if (funct7 == 0b0000000) {
                    return decoded(category, Op::xor_, rd, rs1, rs2, 0, instruction);
                }
                break;
            case 0b101:
                if (funct7 == 0b0000000) {
                    return decoded(category, Op::srl, rd, rs1, rs2, 0, instruction);
                }
                if (funct7 == 0b0100000) {
                    return decoded(category, Op::sra, rd, rs1, rs2, 0, instruction);
                }
                break;
            case 0b110:
                if (funct7 == 0b0000000) {
                    return decoded(category, Op::or_, rd, rs1, rs2, 0, instruction);
                }
                break;
            case 0b111:
                if (funct7 == 0b0000000) {
                    return decoded(category, Op::and_, rd, rs1, rs2, 0, instruction);
                }
                break;
            }
            return std::unexpected{DecodeError::illegal_opcode};
        }

        std::expected<DecodedInstruction, DecodeError> Decoder::op_imm(word instruction, InstructionCategory category) {
            byte rd = utils::take_rd(instruction);
            byte rs1 = utils::take_rs1(instruction);
            uint32_t funct3 = utils::take_funct3(instruction);
            word imm_type_0 = sign_extend<12>(utils::take_bits<20, 31>(instruction));
            word imm_shamt = utils::take_bits<20, 24>(instruction);
            uint32_t imm_type1 = utils::take_bits<25, 31>(instruction);

            switch (funct3) {
            case 0b000:
                return decoded(category, Op::addi, rd, rs1, 0, imm_type_0, instruction);
            case 0b010:
                return decoded(category, Op::slti, rd, rs1, 0, imm_type_0, instruction);
            case 0b011:
                return decoded(category, Op::sltiu, rd, rs1, 0, imm_type_0, instruction);
            case 0b100:
                return decoded(category, Op::xori, rd, rs1, 0, imm_type_0, instruction);
            case 0b110:
                return decoded(category, Op::ori, rd, rs1, 0, imm_type_0, instruction);
            case 0b111:
                return decoded(category, Op::andi, rd, rs1, 0, imm_type_0, instruction);
            case 0b001:
                if (imm_type1 == 0b0000000) {
                    return decoded(category, Op::slli, rd, rs1, 0, imm_shamt, instruction);
                }
                break;
            case 0b101:
                if (imm_type1 == 0b0000000) {
                    return decoded(category, Op::srli, rd, rs1, 0, imm_shamt, instruction);
                }
                if (imm_type1 == 0b0100000) {
                    return decoded(category, Op::srai, rd, rs1, 0, imm_shamt, instruction);
                }
                break;
            }
            return std::unexpected{DecodeError::illegal_opcode};
        }

        std::expected<DecodedInstruction, DecodeError> Decoder::lui(word instruction, InstructionCategory category) {
            return decoded(category, Op::lui, utils::take_rd(instruction), 0, 0, instruction & 0xfffff000, instruction);
        }

        std::expected<DecodedInstruction, DecodeError> Decoder::auipc(word instruction, InstructionCategory category) {
            return decoded(category, Op::auipc, utils::take_rd(instruction), 0, 0, instruction & 0xfffff000, instruction);
        }

        std::expected<DecodedInstruction, DecodeError> Decoder::jal(word instruction, InstructionCategory category) {
            word imm = (utils::take_bits<31, 31>(instruction) << 20)
                     | (utils::take_bits<12, 19>(instruction) << 12)
                     | (utils::take_bits<20, 20>(instruction) << 11)
                     | (utils::take_bits<21, 30>(instruction) << 1);
            return decoded(category, Op::jal, utils::take_rd(instruction), 0, 0, sign_extend<21>(imm), instruction);
        }

        std::expected<DecodedInstruction, DecodeError> Decoder::jalr(word instruction, InstructionCategory category) {
            if (utils::take_funct3(instruction) != 0b000) {
                return std::unexpected{DecodeError::illegal_opcode};
            }
            return decoded(category, Op::jalr, utils::take_rd(instruction), utils::take_rs1(instruction), 0,
                           sign_extend<12>(utils::take_bits<20, 31>(instruction)), instruction);
        }

        std::expected<DecodedInstruction, DecodeError> Decoder::branch(word instruction, InstructionCategory category) {
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
            return decoded(category, operation, 0, utils::take_rs1(instruction), utils::take_rs2(instruction), sign_extend<13>(imm), instruction);
        }

        std::expected<DecodedInstruction, DecodeError> Decoder::load(word instruction, InstructionCategory category) {
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
            return decoded(category, operation, utils::take_rd(instruction), utils::take_rs1(instruction), 0,
                           sign_extend<12>(utils::take_bits<20, 31>(instruction)), instruction);
        }

        std::expected<DecodedInstruction, DecodeError> Decoder::store(word instruction, InstructionCategory category) {
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
            return decoded(category, operation, 0, utils::take_rs1(instruction), utils::take_rs2(instruction), sign_extend<12>(imm), instruction);
        }

        std::expected<DecodedInstruction, DecodeError> Decoder::misc_mem(word instruction, InstructionCategory category) {
            if (utils::take_funct3(instruction) != 0b000) {
                return std::unexpected{DecodeError::illegal_opcode};
            }
            return decoded(category, Op::fence, 0, 0, 0, 0, instruction);
        }

        std::expected<DecodedInstruction, DecodeError> Decoder::system(word instruction, InstructionCategory category) {
            if (instruction == 0x00000073) {
                return decoded(category, Op::ecall, 0, 0, 0, 0, instruction);
            }
            if (instruction == 0x00100073) {
                return decoded(category, Op::ebreak, 0, 0, 0, 0, instruction);
            }
            return std::unexpected{DecodeError::illegal_opcode};
        }

    template <byte Bits>
    constexpr word Decoder::sign_extend(word value) noexcept {
            static_assert(Bits > 0 && Bits < 32);
            constexpr word sign_bit = word{1} << (Bits - 1);
            return (value ^ sign_bit) - sign_bit;
        }

        constexpr DecodedInstruction Decoder::decoded(InstructionCategory category, Op operation, byte rd, byte rs1, byte rs2, word imm, word raw) noexcept {
            return {.op = operation, .category = category, .rd = rd, .rs1 = rs1, .rs2 = rs2, .imm = imm, .raw = raw};
        }
}
