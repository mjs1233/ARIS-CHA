//
// Created by tgian on 2026-08-19.
//

#ifndef ARIS_RV32I_CONFIG_HPP
#define ARIS_RV32I_CONFIG_HPP
#include "Global.hpp"

namespace aris {
    struct Config {
        word instruction_section;
        word data_section;
    };

    extern Config g_config;
}
#endif // ARIS_RV32I_CONFIG_HPP
