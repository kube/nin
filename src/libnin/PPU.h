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

#ifndef LIBNIN_PPU_H
#define LIBNIN_PPU_H 1

#include <cstddef>
#include <cstdint>
#include <libnin/NonCopyable.h>

namespace libnin
{

class HardwareInfo;
class Memory;
class NMI;
class BusVideo;
class Mapper;
class Video;
class PPU : private NonCopyable
{
public:
    PPU(HardwareInfo& info, Memory& memory, NMI& nmi, BusVideo& busVideo, Mapper& mapper, Video& video);

    std::uint8_t    regRead(std::uint16_t reg);
    void            regWrite(std::uint16_t reg, std::uint8_t value);
    void            oamWrite(std::uint8_t value);

    void            tick(std::size_t cycles);

private:
    struct Flags
    {
        bool incrementY:1;
        bool altSpritePattern:1;
        bool altBackgroundPattern:1;
        bool largeSprites:1;
    };

    using AnyFuncPtr = void* (PPU::*)(void);
    using Handler = AnyFuncPtr(PPU::*)(void);

    Handler handleWait();
    Handler handleVBlank();
    Handler handlePreScan();
    Handler handleScan();
    Handler handleScanNT0();
    Handler handleScanNT1();
    Handler handleScanAT0();
    Handler handleScanAT1();
    Handler handleScanLoBG0();
    Handler handleScanLoBG1();
    Handler handleScanHiBG0();
    Handler handleScanHiBG1();

    Handler wait(std::uint32_t cycles, Handler next);

    void            emitPixel();
    std::uint8_t    pixelBackground();

    void shiftReload();

    HardwareInfo&   _info;
    Memory&         _memory;
    NMI&            _nmi;
    BusVideo&       _busVideo;
    Mapper&         _mapper;
    Video&          _video;

    Handler         _handler;
    Handler         _handler2;

    std::uint16_t   _v;
    std::uint16_t   _t;
    std::uint8_t    _x;
    bool            _w:1;

    Flags           _flags;

    std::uint8_t    _readBuf;
    std::uint8_t    _latchNT;
    std::uint8_t    _latchAT;
    std::uint8_t    _latchLoBG;
    std::uint8_t    _latchHiBG;

    std::uint16_t   _shiftPatternLo;
    std::uint16_t   _shiftPatternHi;
    std::uint16_t   _shiftPaletteLo;
    std::uint16_t   _shiftPaletteHi;

    std::uint32_t   _clock;
    std::uint32_t   _clockVideo;
    std::uint8_t    _scanline;
    std::uint8_t    _step;
    std::uint8_t    _oamAddr;
};

}

#endif
