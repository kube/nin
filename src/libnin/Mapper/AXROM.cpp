/*
 * BSD 2 - Clause License
 *
 * Copyright(c) 2019, Maxime Bacoux
 * All rights reserved.
 *
 * Redistributionand use in sourceand binary forms, with or without
 * modification, are permitted provided that the following conditions are met :
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditionsand the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditionsand the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <libnin/Cart.h>
#include <libnin/Mapper.h>
#include <libnin/Util.h>

using namespace libnin;

template <>
void Mapper::handleWrite<MapperID::AxROM>(std::uint16_t addr, std::uint8_t value)
{
    if (addr >= 0x8000)
    {
        mirror(((value >> 4) & 1) ? NIN_MIRROR_B : NIN_MIRROR_A);
        bankPrg32k(2, CART_PRG_ROM, value & 0xf);
    }
}

template <>
void Mapper::handleWrite<MapperID::AxROM_Conflicts>(std::uint16_t addr, std::uint8_t value)
{
    if (addr >= 0x8000)
    {
        value &= _prg[(addr - 0x8000) / 0x2000 + 2][addr & 0x1fff];
        mirror(((value >> 4) & 1) ? NIN_MIRROR_B : NIN_MIRROR_A);
        bankPrg32k(2, CART_PRG_ROM, value & 0xf);
    }
}

template <>
void Mapper::init<MapperID::AxROM>()
{
    _handleWrite = &Mapper::handleWrite<MapperID::AxROM>;
}

template <>
void Mapper::init<MapperID::AxROM_Conflicts>()
{
    _handleWrite = &Mapper::handleWrite<MapperID::AxROM_Conflicts>;
}
