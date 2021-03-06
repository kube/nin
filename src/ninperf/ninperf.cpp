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

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <chrono>
#include <nin/nin.h>

#define COUNT 10000000
#define SLICE 4096

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
using Duration = std::chrono::duration<double>;

static void dummyAudio(void*, const float*) {

}

int main(int argc, char** argv)
{
    NinState* state;
    TimePoint before;
    TimePoint after;
    double duration;
    std::uint64_t cps;
    size_t cyc;
    size_t tmp;

    if (argc != 2)
        return 1;

    ninCreateState(&state, argv[1]);
    if (!state)
        return 1;

    srand((unsigned)time(NULL));
    ninAudioSetCallback(state, &dummyAudio, nullptr);

    cyc = 0;
    before = Clock::now();
    for (;;)
    {
        ninSetInput(state, (rand() & 0xff));
        ninRunCycles(state, SLICE, &tmp);
        cyc += SLICE;
        cyc += tmp;
        if (cyc >= COUNT)
            break;
    }
    after = Clock::now();
    duration = std::chrono::duration_cast<Duration>(after - before).count();
    cps = (std::uint64_t)((double)cyc / duration);
    printf("%llu cycles/s\n", (long long unsigned int)cps);
    return 0;
}
