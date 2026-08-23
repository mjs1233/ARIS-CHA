//
// Created by Codex on 8/21/26.
//

#ifndef ARIS_RV32I_REGISTER_FILE_HPP
#define ARIS_RV32I_REGISTER_FILE_HPP
#include <array>
#include "Global.hpp"

namespace aris {

    class RegisterFile {
    public:
        using index_type = byte;

        static constexpr index_type zero = 0;
        static constexpr index_type link = 1;
        static constexpr index_type return_address = link;
        static constexpr index_type pc = 31;

        class Reference {
        public:
            Reference(RegisterFile& registers, index_type index);
            Reference& operator=(word value);
            Reference& operator=(const Reference& other);
            Reference& operator+=(word value);
            [[nodiscard]] operator word() const;

        private:
            RegisterFile& m_registers;
            index_type m_index;
        };

        [[nodiscard]] Reference operator[](index_type index);
        [[nodiscard]] word operator[](index_type index) const;

    private:
        [[nodiscard]] word read(index_type index) const;
        void write(index_type index, word value);

        std::array<word, 32> m_registers = {};
    };
}
#endif // ARIS_RV32I_REGISTER_FILE_HPP
