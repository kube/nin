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

#ifndef LIBNIN_H
#define LIBNIN_H 1

#include <stdlib.h>
#include <nin/nin.h>
#include <libnin/APU.h>
#include <libnin/Audio.h>
#include <libnin/Cart.h>
#include <libnin/DiskSystem.h>
#include <libnin/HardwareInfo.h>
#include <libnin/IRQ.h>
#include <libnin/Mapper.h>
#include <libnin/Memory.h>
#include <libnin/Util.h>

using namespace libnin;

#define RAM_SIZE    0x800
#define VRAM_SIZE   0x800

#define PFLAG_C     0x01
#define PFLAG_Z     0x02
#define PFLAG_I     0x04
#define PFLAG_D     0x08
#define PFLAG_B     0x10
#define PFLAG_1     0x20
#define PFLAG_V     0x40
#define PFLAG_N     0x80

#define PFLAG_MASK  (~(PFLAG_B | PFLAG_1))

#define REG_A   0x00
#define REG_X   0x01
#define REG_Y   0x02
#define REG_S   0x03

#define NMI_OCCURED 0x01
#define NMI_OUTPUT  0x02

#define BITMAP_X    256
#define BITMAP_Y    240

#define DEBUG_LEVEL 0

typedef void    (*NinPrgWriteHandler)(NinState* state, uint16_t addr, uint8_t value);
typedef void    (*NinPpuMonitorHandler)(NinState* state, uint16_t addr);
typedef uint8_t (*NinMemoryReadHandler)(NinState* state, uint16_t addr);
typedef void    (*NinMemoryWriteHandler)(NinState* state, uint16_t addr, uint8_t value);

#define MIRRORING_HORIZONTAL    0
#define MIRRORING_VERTICAL      1
#define MIRRORING_QUAD          2

#define CLOCK_RATE_NTSC     1789773

typedef struct {
    uint8_t reserved[8];
} NinRomHeaderLegacy;

typedef struct {
    uint8_t     mapperEx:4;
    uint8_t     submapper:4;
    uint8_t     prgRomSizeHi:4;
    uint8_t     chrRomSizeHi:4;
    uint8_t     prgRamSizeShift:4;
    uint8_t     prgNvramSizeShift:4;
    uint8_t     chrRamSizeShift:4;
    uint8_t     chrNvramSizeShift:4;
    uint8_t     region:2;
    uint8_t     reserved0:6;
    uint8_t     reserved[3];

} NinRomHeaderNes2;

typedef struct {
    char        magic[4];
    uint8_t     prgRomSize;
    uint8_t     chrRomSize;
    uint8_t     mirroring:1;
    uint8_t     battery:1;
    uint8_t     trainer:1;
    uint8_t     quadScreen:1;
    uint8_t     mapperLo:4;
    uint8_t     vs:1;
    uint8_t     playChoice:1;
    uint8_t     magicNes2:2;
    uint8_t     mapperHi:4;
    union {
        NinRomHeaderLegacy  ines;
        NinRomHeaderNes2    nes2;
    };
} NinRomHeader;

typedef struct {
    uint16_t    pc;
    uint8_t     regs[4];
    uint8_t     p;
    uint8_t     p2;
} NinCPU;

typedef struct {
    uint8_t shift;
    uint8_t count;
    uint8_t mirroring:2;
    uint8_t prgBankMode:2;
    uint8_t chrBankMode:1;
    uint8_t chrBank0:5;
    uint8_t chrBank1:5;
    uint8_t prgBank:4;
} NinMapperRegsMMC1;

typedef struct {
    uint8_t bankLatch0Lo;
    uint8_t bankLatch0Hi;
    uint8_t bankLatch1Lo;
    uint8_t bankLatch1Hi;
    uint8_t latch0:1;
    uint8_t latch1:1;
} NinMapperRegsMMC2;

typedef struct {
    uint8_t bankSelect:3;
    uint8_t bank[8];
    uint8_t bankModePrgRom:1;
    uint8_t bankModeChrRom:1;
} NinMapperRegsMMC3;

typedef union {
    NinMapperRegsMMC1 mmc1;
    NinMapperRegsMMC2 mmc2;
    NinMapperRegsMMC3 mmc3;
} NinMapperRegs;

typedef union {
    uint8_t raw[4];
    struct {
        uint8_t y;
        uint8_t tile;
        uint8_t palette:2;
        uint8_t reserved:3;
        uint8_t back:1;
        uint8_t xFlip:1;
        uint8_t yFlip:1;
        uint8_t x;
    };
} NinSprite;

typedef struct {
    uint16_t    t;
    uint16_t    v;
    uint8_t     x;
    uint16_t    scanline;
    uint16_t    cycle;
    uint8_t     latchName;
    uint8_t     latchAttr;
    uint8_t     latchTileLo;
    uint8_t     latchTileHi;
    uint16_t    shiftPatternLo;
    uint16_t    shiftPatternHi;
    uint16_t    shiftPaletteHi;
    uint16_t    shiftPaletteLo;
    NinSprite   oam2[8];
    uint8_t     oam2Index;
    uint8_t     latchSpriteBitmapLo[8];
    uint8_t     latchSpriteBitmapHi[8];
    uint8_t     latchSpriteBitmapAttr[8];
    uint8_t     latchSpriteBitmapX[8];
    unsigned    zeroHit:1;
    unsigned    zeroHitNext:1;
    unsigned    maskEnableBackground:1;
    unsigned    maskEnableBackgroundLeft:1;
    unsigned    maskEnableSprites:1;
    unsigned    maskEnableSpritesLeft:1;
    unsigned    largeSprites:1;
    uint8_t     inhibitNmi:1;
} NinRuntimePPU;

typedef struct {
    NinRuntimePPU   rt;
    uint8_t         oamAddr;
    uint8_t         readBuf;
    uint8_t         latch;
    uint8_t         nmi;
    uint8_t         controller;
    uint8_t         scrollX;
    uint8_t         scrollY;
    uint8_t         w:1;
    uint8_t         zeroHitFlag:1;
    uint8_t         race0:1;
    uint8_t         race1:1;
} NinPPU;

struct NinState
{
    NinState();

    Memory              memory;
    HardwareInfo        info;
    Cart                cart;
    IRQ                 irq;
    Mapper              mapper;
    Audio               audio;
    APU                 apu;
    DiskSystem          diskSystem;
    NinCPU              cpu;
    NinPPU              ppu;
    FILE*               saveFile;
    uint8_t             battery;
    uint8_t             mirroring;
    uint8_t             controller;
    uint8_t             controllerLatch;
    uint32_t*           backBuffer;
    uint32_t*           frontBuffer;
    uint16_t            audioSamplesCount;
    uint32_t            audioCycles;
    uint8_t*            palettes;
    union {
        uint8_t*        oam;
        NinSprite*      oamSprites;
    };

    NinMapperRegs           mapperRegs;
    NinPpuMonitorHandler    ppuMonitorHandler;
    NinMemoryReadHandler    readHandler;
    NinMemoryWriteHandler   writeHandler;

    uint8_t             nmi:1;
    uint8_t             nmi2:1;
    uint64_t            cyc;
    uint8_t             frame:1;
    uint8_t             frameOdd:1;
    uint8_t             irqScanlineEnabled:1;
    uint8_t             irqScanlineReload:1;
    uint8_t             irqScanlineCounter;
    uint8_t             irqScanlineReloadValue;
    uint16_t            irqScanlineFilterShifter;
    uint16_t            oldVmemAddr;
};

uint8_t     ninMemoryRead8(NinState* state, uint16_t addr);
uint16_t    ninMemoryRead16(NinState* state, uint16_t addr);
void        ninMemoryWrite8(NinState* state, uint16_t addr, uint8_t value);
void        ninMemoryWrite16(NinState* state, uint16_t addr, uint16_t value);

uint8_t     ninMemoryReadNES(NinState* state, uint16_t addr);
uint8_t     ninMemoryReadFDS(NinState* state, uint16_t addr);
void        ninMemoryWriteNES(NinState* state, uint16_t addr, uint8_t value);
void        ninMemoryWriteFDS(NinState* state, uint16_t addr, uint8_t value);

uint8_t     ninVMemoryRead8(NinState* state, uint16_t addr);
void        ninVMemoryWrite8(NinState* state, uint16_t addr, uint8_t value);

void        ninSetFlagNMI(NinState* state, uint8_t flag);
void        ninUnsetFlagNMI(NinState* state, uint8_t flag);

uint8_t     ninPpuRegRead(NinState* state, uint16_t reg);
void        ninPpuRegWrite(NinState* state, uint16_t reg, uint8_t value);

int         ninPpuRunCycles(NinState* state, uint16_t cycles);

/* Mapper handlers */
void    ninPrgWriteHandlerMMC1(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerMMC2(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerMMC3(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerMMC4(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerUXROM(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerUXROM_180(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerCNROM(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerAXROM(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerGXROM(NinState* state, uint16_t addr, uint8_t value);
void    ninPrgWriteHandlerColorDreams(NinState* state, uint16_t addr, uint8_t value);

void    ninPrgWriteHandlerCommonMMC2_MMC4(NinState* state, uint16_t addr, uint8_t value);

void    ninPpuMonitorHandlerNull(NinState* state, uint16_t addr);
void    ninPpuMonitorHandlerMMC2(NinState* state, uint16_t addr);
void    ninPpuMonitorHandlerMMC3(NinState* state, uint16_t addr);

#endif
