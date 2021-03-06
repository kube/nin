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

#ifndef MEMORY_SEARCH_WINDOW_H
#define MEMORY_SEARCH_WINDOW_H 1

#include <vector>
#include <cstdint>
#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <nin/nin.h>

class MemorySearchWindow : public QWidget
{
    Q_OBJECT;

public:
    explicit MemorySearchWindow(QWidget* parent = nullptr);

public slots:
    void refresh(NinState* state);

private:
    enum class SearchFunc
    {
        Equal = 0,
        NotEqual,
        Lesser,
        Greater,
        LesserEqual,
        GreaterEqual
    };

    enum class SearchValue
    {
        Previous = 0,
        First,
        Custom
    };

    struct MemorySpan
    {
        std::uint16_t   base;
        std::uint16_t   size;
    };

    void setSearchFunc(int searchFunc);
    void setSearchValue(int searchValue);

    void updateTable();
    void updateResults();
    void reset();
    void search();
    bool searchPredicate(std::uint16_t addr) const;

    QLabel*         _labelResults;
    QTableWidget*   _table;

    SearchFunc              _searchFunc;
    SearchValue             _searchValue;
    std::vector<MemorySpan> _spans;
    uint8_t                 _buffer[0x10000];
    uint8_t                 _bufferPrev[0x10000];
    uint8_t                 _bufferFirst[0x10000];
};

#endif
