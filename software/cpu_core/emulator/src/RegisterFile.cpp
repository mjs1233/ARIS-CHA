#include "RegisterFile.hpp"
#include <cassert>

namespace aris {

    RegisterFile::Reference::Reference(RegisterFile& registers, index_type index) : m_registers(registers), m_index(index) {}

    RegisterFile::Reference& RegisterFile::Reference::operator=(word value) {
        m_registers.write(m_index, value);
        return *this;
    }

    RegisterFile::Reference& RegisterFile::Reference::operator=(const Reference& other) {
        return *this = static_cast<word>(other);
    }

    RegisterFile::Reference& RegisterFile::Reference::operator+=(word value) {
        return *this = static_cast<word>(*this) + value;
    }

    RegisterFile::Reference::operator word() const {
        return m_registers.read(m_index);
    }

    RegisterFile::Reference RegisterFile::operator[](index_type index) {
        assert(index < m_registers.size());
        return Reference(*this, index);
    }

    word RegisterFile::operator[](index_type index) const {
        assert(index < m_registers.size());
        return read(index);
    }

    word RegisterFile::read(index_type index) const {
        return m_registers[index];
    }

    void RegisterFile::write(index_type index, word value) {
        if (index != zero) {
            m_registers[index] = value;
        }
    }
}
