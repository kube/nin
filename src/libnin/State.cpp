#include <libnin/libnin.h>

NinState::NinState()
: memory{}
, info{}
, cart{}
, input{}
, irq{}
, nmi{}
, mapper{memory, cart, irq}
, busVideo{memory, cart, mapper}
, audio{info}
, apu{info, irq, mapper, audio}
, diskSystem{info, irq}
, video{}
, ppu{info, memory, nmi, busVideo, mapper, video}
, cpu{}
, saveFile{}
, battery{}
, mirroring{}
, readHandler{}
, nmi2{}
, cyc{}
, frame{}
{
}
