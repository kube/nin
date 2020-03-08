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

#include <cstring>
#include <cmath>
#include <libnin/PPU.h>
#include <libnin/BusVideo.h>
#include <libnin/HardwareInfo.h>
#include <libnin/Mapper.h>
#include <libnin/Memory.h>
#include <libnin/NMI.h>
#include <libnin/Video.h>

using namespace libnin;

static constexpr const std::uint8_t bitrev8(std::uint8_t v)
{
    alignas(64) constexpr const std::uint8_t kLookup[] =
    {
        0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0, 0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
        0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8, 0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
        0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4, 0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
        0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec, 0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
        0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2, 0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
        0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea, 0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
        0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6, 0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
        0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee, 0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
        0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1, 0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
        0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9, 0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
        0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5, 0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
        0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed, 0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
        0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3, 0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
        0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb, 0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
        0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7, 0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
        0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
    };

    return kLookup[v];
}

static const uint16_t kHMask = 0x041f;

static std::uint16_t incX(std::uint16_t v)
{
    if ((v & 0x001f) == 31)
    {
        v &= ~0x001f;
        v ^= 0x0400;
    }
    else
        v++;
    return v;
}

static std::uint16_t incY(std::uint16_t v)
{
    std::uint16_t y;

    if ((v & 0x7000) != 0x7000)
        v += 0x1000;
    else
    {
        v &= ~0x7000;
        y = (v & 0x03E0) >> 5;
        if (y == 29)
        {
            y = 0;
            v ^= 0x0800;
        }
        else if (y == 31)
            y = 0;
        else
            y++;
        v = (v & ~0x03E0) | (y << 5);
    }

    return v;
}

PPU::PPU(HardwareInfo& info, Memory& memory, NMI& nmi, BusVideo& busVideo, Mapper& mapper, Video& video)
: _info{info}
, _memory{memory}
, _nmi{nmi}
, _busVideo{busVideo}
, _mapper{mapper}
, _video{video}
, _handler{(Handler)&PPU::handlePreScan}
, _handler2{}
, _v{}
, _t{}
, _x{}
, _w{}
, _flags{}
, _readBuf{}
, _latchNT{}
, _latchAT{}
, _latchLoBG{}
, _latchHiBG{}
, _shiftPatternLo{}
, _shiftPatternHi{}
, _shiftPaletteLo{}
, _shiftPaletteHi{}
, _clock{}
, _clockVideo{}
, _scanline{}
, _step{}
, _oamAddr{}
{

}

std::uint8_t PPU::regRead(std::uint16_t reg)
{
    std::uint8_t value;

    value = 0;

    switch (reg & 0x07)
    {
    case 0x00:
        break;
    case 0x01:
        break;
    case 0x02: // PPUSTATUS
        if (_nmi.check(NMI_OCCURED))
            value |= 0x80;
        _nmi.unset(NMI_OCCURED);
        _w = false;
        break;
    case 0x03:
        break;
    case 0x04: // OAMDATA
        value = _memory.oam[_oamAddr];
        break;
    case 0x05:
        break;
    case 0x06:
        break;
    case 0x07: // PPUDATA
        if ((_v & 0x3f00) == 0x3f00)
        {
            value = _busVideo.read(_v);
            _readBuf = _busVideo.read(_v & 0x2fff);
        }
        else
        {
            value = _readBuf;
            _readBuf = _busVideo.read(_v);
        }
        _v += _flags.incrementY ? 32 : 1;
        break;
    }

    return value;
}

void PPU::regWrite(std::uint16_t reg, std::uint8_t value)
{
    switch (reg & 0x07)
    {
    case 0x00: // PPUCTRL
        // Set base nametable
        _t &= ~0x0c00;
        _t |= ((value & 0x03) << 10);

        _flags.incrementY = !!(value & 0x04);
        _flags.altSpritePattern = !!(value & 0x08);
        _flags.altBackgroundPattern = !!(value & 0x10);
        _flags.largeSprites = !!(value & 0x20);

        if (value & 0x80)
            _nmi.set(NMI_OUTPUT);
        else
            _nmi.unset(NMI_OUTPUT);
        break;
    case 0x01:
        break;
    case 0x02:
        break;
    case 0x03: // OAMADDR
        _oamAddr = value;
        break;
    case 0x04: // OAMDATA
        oamWrite(value);
        break;
    case 0x05: // PPUSCROLL
        if (!_w)
        {
            // Fine X
            _x = value & 0x07;

            // Coarse X
            _t &= ~(0x001f);
            _t |= ((value >> 3) & 0x1f);
        }
        else
        {
            // Fine Y
            _t &= ~(0x7000);
            _t |= (((uint16_t)value & 0x07) << 12);

            // Coarse Y
            _t &= ~(0x03e0);
            _t |= (((uint16_t)value >> 3) & 0x1f) << 5;
        }
        _w = !_w;
        break;
    case 0x06: // PPUADDR
        if (!_w)
        {
            _t &= 0x00ff;
            _t |= (((uint16_t)value << 8) & 0x3fff);
        }
        else
        {
            _t &= 0xff00;
            _t |= (value & 0x00ff);
            _v = _t;
            _mapper.videoRead(_v);
        }
        _w = !_w;
        break;
    case 0x07: // PPUDATA
        _busVideo.write(_v, value);
        _v += _flags.incrementY ? 32 : 1;
        break;
    }
}

void PPU::oamWrite(std::uint8_t value)
{
    _memory.oam[_oamAddr++] = value;
}

void PPU::tick(std::size_t cycles)
{
    while (cycles--)
    {
        _handler = (Handler)(this->*_handler)();
    }
}

PPU::Handler PPU::handleWait()
{
    if (--_clock)
        return (Handler)&PPU::handleWait;
    return _handler2;
}

PPU::Handler PPU::handleVBlank()
{
    _video.swap();
    _clockVideo = 0;
    _nmi.set(NMI_OCCURED);
    return wait(341 * 100, (Handler)&PPU::handlePreScan);
}

PPU::Handler PPU::handlePreScan()
{
    _nmi.unset(NMI_OCCURED);
    _v = _t;
    return wait(341, (Handler)&PPU::handleScan);
}

PPU::Handler PPU::handleScan()
{
    _step = 0;
    return (Handler)&PPU::handleScanNT0;
}

PPU::Handler PPU::handleScanNT0()
{
    emitPixel();
    return (Handler)&PPU::handleScanNT1;
}

PPU::Handler PPU::handleScanNT1()
{
    emitPixel();
    _latchNT = _busVideo.read(0x2000 | (_v & 0xfff));
    return (Handler)&PPU::handleScanAT0;
}

PPU::Handler PPU::handleScanAT0()
{
    emitPixel();
    return (Handler)&PPU::handleScanAT1;
}

PPU::Handler PPU::handleScanAT1()
{
    emitPixel();
    _latchAT = _busVideo.read(0x23c0 | (_v & 0x0c00) | ((_v >> 4) & 0x38) | ((_v >> 2) & 0x07));
    return (Handler)&PPU::handleScanLoBG0;
}

PPU::Handler PPU::handleScanLoBG0()
{
    emitPixel();
    return (Handler)&PPU::handleScanLoBG1;
}

PPU::Handler PPU::handleScanLoBG1()
{
    emitPixel();
    _latchLoBG = _busVideo.read((_flags.altBackgroundPattern ? 0x1000 : 0x0000) | _latchNT << 4 | ((_v >> 12) & 0x07));
    return (Handler)&PPU::handleScanHiBG0;
}

PPU::Handler PPU::handleScanHiBG0()
{
    emitPixel();
    return (Handler)&PPU::handleScanHiBG1;
}

PPU::Handler PPU::handleScanHiBG1()
{
    emitPixel();
    _latchHiBG = _busVideo.read((_flags.altBackgroundPattern ? 0x1000 : 0x0000) | _latchNT << 4 | 0x08 | ((_v >> 12) & 0x07));
    shiftReload();

    /* Increment X */
    _v = incX(_v);

    _step++;
    if (_step < 32)
    {
        return (Handler)&PPU::handleScanNT0;
    }
    _scanline++;
    if (_scanline < 240)
    {
        _v = (_v & ~kHMask) | (_t & kHMask);
        _v = incY(_v);

        return wait(84, (Handler)&PPU::handleScan);
    }
    _scanline = 0;
    return wait(84, (Handler)&PPU::handleVBlank);
}

PPU::Handler PPU::wait(std::uint32_t cycles, Handler next)
{
    _clock = cycles;
    _handler2 = next;

    return (Handler)&PPU::handleWait;
}

void PPU::emitPixel()
{
    _video.write(_clockVideo, pixelBackground());
    _clockVideo++;
}

std::uint8_t PPU::pixelBackground()
{
    std::uint8_t pattern{};
    std::uint8_t palette{};
    std::uint8_t paletteIdx{};

    pattern |= (_shiftPatternLo & 0x01);
    pattern |= ((_shiftPatternHi & 0x01) << 1);
    palette |= (_shiftPaletteLo & 0x01);
    palette |= ((_shiftPaletteHi & 0x01) << 1);

    _shiftPatternLo >>= 1;
    _shiftPatternHi >>= 1;
    _shiftPaletteLo >>= 1;
    _shiftPaletteHi >>= 1;

    if (pattern)
    {
        paletteIdx = (palette << 2) | pattern;
    }

    return _busVideo.read(0x3f00 | paletteIdx) & 0x3f;
}

void PPU::shiftReload()
{
    _shiftPatternLo |= (std::uint16_t)bitrev8(_latchLoBG) << 8;
    _shiftPatternHi |= (std::uint16_t)bitrev8(_latchHiBG) << 8;
    _shiftPaletteLo |= (_latchAT & 0x01) ? 0xff00 : 0x0000;
    _shiftPaletteHi |= (_latchAT & 0x02) ? 0xff00 : 0x0000;
}
