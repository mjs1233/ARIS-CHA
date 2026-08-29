//
// Created by Codex on 8/20/26.
//

#ifndef ARIS_RV32I_EXECUTION_UNIT_HPP
#define ARIS_RV32I_EXECUTION_UNIT_HPP
#include <array>
#include "Global.hpp"
#include "instruction.hpp"
#include "RegisterFile.hpp"

namespace aris {

    class ExecutionUnit {
    public:
        void integer_register(RegisterFile& reg, DecodedInstruction instruction) {

        }
        // DecodedInstruction is lowered into micro-operations here in a later stage.
    };
}
#endif // ARIS_RV32I_EXECUTION_UNIT_HPP
