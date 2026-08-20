//
// Created by tgian on 2026-08-19.
//

#ifndef ARIS_RV32I_CPU_HPP
#define ARIS_RV32I_CPU_HPP
#include <iostream>
#include "Global.hpp"
#include "RAM.hpp"
#include "Decoder.hpp"
#include "ExecutionUnit.hpp"

namespace aris {
    class CPU {
    public:
        CPU(RAM& ram, std::ostream& output = std::cout) : m_ram(ram), m_execution_unit(output) {}

        void set_program_counter(word address) {
            m_register_file[program_counter_register] = address;
        }

        [[nodiscard]] word program_counter() const {
            return m_register_file[program_counter_register];
        }

        [[nodiscard]] bool step() {
            auto decode_inst = m_decoder(m_ram.read(m_register_file[program_counter_register]));
            if (!decode_inst.has_value()) {
                return false;
            }

            execute(decode_inst.value());
            m_register_file[program_counter_register] += 4;
            return true;
        }

    private:
        void execute(const DecodedInstruction& instruction) {
            switch (instruction.execution_target) {
            case ExecutionTarget::alu:
                execute_alu(instruction);
                break;
            case ExecutionTarget::io:
                execute_io(instruction);
                break;
            case ExecutionTarget::control:
                execute_control(instruction);
                break;
            case ExecutionTarget::system:
                execute_system(instruction);
                break;
            }
        }

        void execute_alu(const DecodedInstruction& instruction) {
            m_execution_unit.execute(instruction, m_register_file, m_ram);
        }

        void execute_io(const DecodedInstruction& instruction) {
            m_execution_unit.execute(instruction, m_register_file, m_ram);
        }

        void execute_control(const DecodedInstruction& instruction) {
            m_execution_unit.execute(instruction, m_register_file, m_ram);
        }

        void execute_system(const DecodedInstruction& instruction) {
            m_execution_unit.execute(instruction, m_register_file, m_ram);
        }

        static constexpr byte program_counter_register = 31;

        ExecutionUnit::RegisterFile m_register_file = {{}};
        Decoder m_decoder;
        RAM& m_ram;
        ExecutionUnit m_execution_unit;
    };
}

#endif // ARIS_RV32I_CPU_HPP
