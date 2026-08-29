#include "CPU.hpp"

namespace aris {

    CPU::CPU(RAM& ram) : m_ram(ram) {}

    void CPU::set_program_counter(word address) {
        m_register_file[RegisterFile::pc] = address;
    }

    word CPU::program_counter() const {
        return m_register_file[RegisterFile::pc];
    }

    bool CPU::step() {
        auto decoded_instruction = m_decoder(m_ram.read(m_register_file[RegisterFile::pc]));
        if (!decoded_instruction.has_value()) {
            return false;
        }

        m_register_file[RegisterFile::pc] += 4;
        dispatch(decoded_instruction.value());
        return true;
    }

    void CPU::dispatch(DecodedInstruction& instruction) {
        switch (instruction.category) {
        case InstructionCategory::integer_register:
            IntegerRegisterStage::instance().execute(m_register_file, instruction);
            break;
        case InstructionCategory::memory_access:
            MemoryAccessStage::instance().execute(m_register_file, instruction);
            break;
        case InstructionCategory::control_flow:
            ControlFlowStage::instance().execute(m_register_file, instruction);
            break;
        case InstructionCategory::system:
            SystemStage::instance().execute(m_register_file, instruction);
            break;
        }
    }
}
