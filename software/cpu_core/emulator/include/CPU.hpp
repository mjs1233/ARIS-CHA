//
// Created by tgian on 2026-08-19.
//

#ifndef ARIS_RV32I_CPU_HPP
#define ARIS_RV32I_CPU_HPP
#include "Decoder.hpp"
#include "ExecutionStage.hpp"
#include "RAM.hpp"
#include "RegisterFile.hpp"

namespace aris {

    class CPU {
    public:
        explicit CPU(RAM& ram);

        void set_program_counter(word address);
        [[nodiscard]] word program_counter() const;
        [[nodiscard]] bool step();

    private:
        void dispatch(DecodedInstruction& instruction);

        RegisterFile m_register_file;
        Decoder m_decoder;
        RAM& m_ram;
    };
}
#endif // ARIS_RV32I_CPU_HPP
