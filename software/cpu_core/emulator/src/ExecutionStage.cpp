#include "ExecutionStage.hpp"
#include <cassert>

namespace aris {

    IntegerRegisterStage& IntegerRegisterStage::instance() {
        static IntegerRegisterStage stage;
        return stage;
    }

    void IntegerRegisterStage::execute(RegisterFile& registers, DecodedInstruction& instruction) const {
        const Handler handler = m_handlers[op_index(instruction.op)];
        assert(handler != nullptr);
        handler(registers, instruction);
    }

    IntegerRegisterStage::IntegerRegisterStage() {
        m_handlers[op_index(Op::add)] = &add;
        m_handlers[op_index(Op::sub)] = &sub;
        m_handlers[op_index(Op::sll)] = &sll;
        m_handlers[op_index(Op::slt)] = &slt;
        m_handlers[op_index(Op::sltu)] = &sltu;
        m_handlers[op_index(Op::xor_)] = &xor_;
        m_handlers[op_index(Op::srl)] = &srl;
        m_handlers[op_index(Op::sra)] = &sra;
        m_handlers[op_index(Op::or_)] = &or_;
        m_handlers[op_index(Op::and_)] = &and_;
        m_handlers[op_index(Op::addi)] = &addi;
        m_handlers[op_index(Op::slti)] = &slti;
        m_handlers[op_index(Op::sltiu)] = &sltiu;
        m_handlers[op_index(Op::xori)] = &xori;
        m_handlers[op_index(Op::ori)] = &ori;
        m_handlers[op_index(Op::andi)] = &andi;
        m_handlers[op_index(Op::slli)] = &slli;
        m_handlers[op_index(Op::srli)] = &srli;
        m_handlers[op_index(Op::srai)] = &srai;
        m_handlers[op_index(Op::lui)] = &lui;
        m_handlers[op_index(Op::auipc)] = &auipc;
    }

    void IntegerRegisterStage::add(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[instruction.rs1] + registers[instruction.rs2];
    }

    void IntegerRegisterStage::sub(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[instruction.rs1] - registers[instruction.rs2];
    }

    void IntegerRegisterStage::sll(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[instruction.rs1] << registers[instruction.rs2];
    }

    void IntegerRegisterStage::slt(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] =
            static_cast<int32_t>(registers[instruction.rs1]) < static_cast<int32_t>(registers[instruction.rs2]) ? 1 : 0;
    }

    void IntegerRegisterStage::sltu(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = (registers[instruction.rs1] < registers[instruction.rs2]) ? 1 : 0;
    }

    void IntegerRegisterStage::xor_(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[instruction.rs1] ^ registers[instruction.rs2];
    }

    void IntegerRegisterStage::srl(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[instruction.rs1] >> registers[instruction.rs2];
    }

    void IntegerRegisterStage::sra(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[instruction.rs1];
        for (uint32_t i = 0; i < registers[instruction.rs2]; i++) {

            word ext = registers[instruction.rd] & 0b1;
            registers[instruction.rd] = registers[instruction.rd] >> 1;
            registers[instruction.rd] = (ext << 31);
        }
    }

    void IntegerRegisterStage::or_(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[instruction.rs1] | registers[instruction.rs2];
    }

    void IntegerRegisterStage::and_(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[instruction.rs1] & registers[instruction.rs2];
    }

    void IntegerRegisterStage::addi(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[instruction.rs1] + instruction.imm;
    }

    void IntegerRegisterStage::slti(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] =
            static_cast<int32_t>(registers[instruction.rs1]) < static_cast<int32_t>(instruction.imm) ? 1 : 0;
    }

    void IntegerRegisterStage::sltiu(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] =
            registers[instruction.rs1] < instruction.imm ? 1 : 0;
    }

    void IntegerRegisterStage::xori(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[instruction.rs1] ^ instruction.imm;
    }

    void IntegerRegisterStage::ori(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[instruction.rs1] | instruction.imm;
    }

    void IntegerRegisterStage::andi(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[instruction.rs1] & instruction.imm;
    }

    void IntegerRegisterStage::slli(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[instruction.rs1] << instruction.imm;
    }

    void IntegerRegisterStage::srli(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[instruction.rs1] >> instruction.imm;
    }

    void IntegerRegisterStage::srai(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[instruction.rs1];
        for (uint32_t i = 0; i < instruction.imm; i++) {

            word ext = registers[instruction.rd] & 0b1;
            registers[instruction.rd] = registers[instruction.rd] >> 1;
            registers[instruction.rd] = (ext << 31);
        }
    }

    void IntegerRegisterStage::lui(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = instruction.imm << 12;
    }

    void IntegerRegisterStage::auipc(RegisterFile& registers, DecodedInstruction& instruction) {

        registers[instruction.rd] = registers[RegisterFile::pc] + (instruction.imm << 12);
    }

    MemoryAccessStage& MemoryAccessStage::instance() {
        static MemoryAccessStage stage;
        return stage;
    }

    void MemoryAccessStage::execute(RegisterFile& registers, DecodedInstruction& instruction) const {
        const Handler handler = m_handlers[op_index(instruction.op)];
        assert(handler != nullptr);
        handler(registers, instruction);
    }

    MemoryAccessStage::MemoryAccessStage() {
        m_handlers[op_index(Op::lb)] = &lb;
        m_handlers[op_index(Op::lh)] = &lh;
        m_handlers[op_index(Op::lw)] = &lw;
        m_handlers[op_index(Op::lbu)] = &lbu;
        m_handlers[op_index(Op::lhu)] = &lhu;
        m_handlers[op_index(Op::sb)] = &sb;
        m_handlers[op_index(Op::sh)] = &sh;
        m_handlers[op_index(Op::sw)] = &sw;
    }

    void MemoryAccessStage::lb(RegisterFile& registers, DecodedInstruction& instruction) {
        (void) registers;
        (void) instruction;
        // TODO: hand-code lb execution.
    }

    void MemoryAccessStage::lh(RegisterFile& registers, DecodedInstruction& instruction) {
        (void) registers;
        (void) instruction;
        // TODO: hand-code lh execution.
    }

    void MemoryAccessStage::lw(RegisterFile& registers, DecodedInstruction& instruction) {
        (void) registers;
        (void) instruction;
        // TODO: hand-code lw execution.
    }

    void MemoryAccessStage::lbu(RegisterFile& registers, DecodedInstruction& instruction) {
        (void) registers;
        (void) instruction;
        // TODO: hand-code lbu execution.
    }

    void MemoryAccessStage::lhu(RegisterFile& registers, DecodedInstruction& instruction) {
        (void) registers;
        (void) instruction;
        // TODO: hand-code lhu execution.
    }

    void MemoryAccessStage::sb(RegisterFile& registers, DecodedInstruction& instruction) {
        (void) registers;
        (void) instruction;
        // TODO: hand-code sb execution.
    }

    void MemoryAccessStage::sh(RegisterFile& registers, DecodedInstruction& instruction) {
        (void) registers;
        (void) instruction;
        // TODO: hand-code sh execution.
    }

    void MemoryAccessStage::sw(RegisterFile& registers, DecodedInstruction& instruction) {
        (void) registers;
        (void) instruction;
        // TODO: hand-code sw execution.
    }

    ControlFlowStage& ControlFlowStage::instance() {
        static ControlFlowStage stage;
        return stage;
    }

    void ControlFlowStage::execute(RegisterFile& registers, DecodedInstruction& instruction) const {
        const Handler handler = m_handlers[op_index(instruction.op)];
        assert(handler != nullptr);
        handler(registers, instruction);
    }

    ControlFlowStage::ControlFlowStage() {
        m_handlers[op_index(Op::beq)] = &beq;
        m_handlers[op_index(Op::bne)] = &bne;
        m_handlers[op_index(Op::blt)] = &blt;
        m_handlers[op_index(Op::bge)] = &bge;
        m_handlers[op_index(Op::bltu)] = &bltu;
        m_handlers[op_index(Op::bgeu)] = &bgeu;
        m_handlers[op_index(Op::jal)] = &jal;
        m_handlers[op_index(Op::jalr)] = &jalr;
    }

    void ControlFlowStage::beq(RegisterFile& registers, DecodedInstruction& instruction) {

        if (registers[instruction.rs1] == registers[instruction.rs2]) {
            registers[RegisterFile::pc] += instruction.imm;
        }
    }

    void ControlFlowStage::bne(RegisterFile& registers, DecodedInstruction& instruction) {

        if (registers[instruction.rs1] != registers[instruction.rs2]) {
            registers[RegisterFile::pc] += instruction.imm;
        }
    }

    void ControlFlowStage::blt(RegisterFile& registers, DecodedInstruction& instruction) {

        if (static_cast<int32_t>(registers[instruction.rs1]) < static_cast<int32_t>(registers[instruction.rs2])) {
            registers[RegisterFile::pc] += instruction.imm;
        }
    }

    void ControlFlowStage::bge(RegisterFile& registers, DecodedInstruction& instruction) {

        if (static_cast<int32_t>(registers[instruction.rs1]) >= static_cast<int32_t>(registers[instruction.rs2])) {
            registers[RegisterFile::pc] += instruction.imm;
        }
    }

    void ControlFlowStage::bltu(RegisterFile& registers, DecodedInstruction& instruction) {

        if (registers[instruction.rs1] < registers[instruction.rs2]) {
            registers[RegisterFile::pc] += instruction.imm;
        }
    }

    void ControlFlowStage::bgeu(RegisterFile& registers, DecodedInstruction& instruction) {

        if (registers[instruction.rs1] >= registers[instruction.rs2]) {
            registers[RegisterFile::pc] += instruction.imm;
        }
    }

    void ControlFlowStage::jal(RegisterFile& registers, DecodedInstruction& instruction) {


    }

    void ControlFlowStage::jalr(RegisterFile& registers, DecodedInstruction& instruction) {
        (void) registers;
        (void) instruction;
        // TODO: hand-code jalr execution.
    }

    SystemStage& SystemStage::instance() {
        static SystemStage stage;
        return stage;
    }

    void SystemStage::execute(RegisterFile& registers, DecodedInstruction& instruction) const {
        const Handler handler = m_handlers[op_index(instruction.op)];
        assert(handler != nullptr);
        handler(registers, instruction);
    }

    SystemStage::SystemStage() {
        m_handlers[op_index(Op::fence)] = &fence;
        m_handlers[op_index(Op::ecall)] = &ecall;
        m_handlers[op_index(Op::ebreak)] = &ebreak;
    }

    void SystemStage::fence(RegisterFile& registers, DecodedInstruction& instruction) {
        (void) registers;
        (void) instruction;
        // TODO: hand-code fence execution.
    }

    void SystemStage::ecall(RegisterFile& registers, DecodedInstruction& instruction) {
        (void) registers;
        (void) instruction;
        // TODO: hand-code ecall execution.
    }

    void SystemStage::ebreak(RegisterFile& registers, DecodedInstruction& instruction) {
        (void) registers;
        (void) instruction;
        // TODO: hand-code ebreak execution.
    }
}
