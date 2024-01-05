// Copyright 2024 Benjamin Lutz.
// 
// This file is part of QRGen. QRGen is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.

#include "symbol.h"
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <vector>
#include "polynomial.h"
#include "util.h"

using namespace std;


static const vector<uint8_t> positions[40] {
    {},
    {6, 18},
    {6, 22},
    {6, 26},
    {6, 30},
    {6, 34},
    {6, 22, 38},
    {6, 24, 42},
    {6, 26, 46},
    {6, 28, 50},
    {6, 30, 54},
    {6, 32, 58},
    {6, 34, 62},
    {6, 26, 46, 66},
    {6, 26, 48, 70},
    {6, 26, 50, 74},
    {6, 30, 54, 78},
    {6, 30, 56, 82},
    {6, 30, 58, 86},
    {6, 34, 62, 90},
    {6, 28, 50, 72, 94},
    {6, 26, 50, 74, 98},
    {6, 30, 54, 78, 102},
    {6, 28, 54, 80, 106},
    {6, 32, 58, 84, 110},
    {6, 30, 58, 86, 114},
    {6, 34, 62, 90, 118},
    {6, 26, 50, 74, 98, 122},
    {6, 30, 54, 78, 102, 126},
    {6, 26, 52, 78, 104, 130},
    {6, 30, 56, 82, 108, 134},
    {6, 34, 60, 86, 112, 138},
    {6, 30, 58, 86, 114, 142},
    {6, 34, 62, 90, 118, 146},
    {6, 30, 54, 78, 102, 126, 150},
    {6, 24, 50, 76, 102, 128, 154},
    {6, 28, 54, 80, 106, 132, 158},
    {6, 32, 58, 84, 110, 136, 162},
    {6, 26, 54, 82, 110, 138, 166},
    {6, 30, 58, 86, 114, 142, 170},
};


Symbol::Symbol(uint8_t version)
    : _version(version), _size(1 <= version && version <= 40 ? 17 + version * 4 : 0) {
    if (_size == 0) { return; }
    
    _pixels.resize(_size * _size);
    _pixelType.resize(_size * _size, PixelType::Unset);
    _highlight.resize(_size * _size);
    
    drawFinderPatterns();
    drawTimingPatterns();
    drawAlignmentPatterns();
    drawDarkModule();
    drawVersionInformation();
}


size_t Symbol::size() const {
    return _size;
}


const std::vector<bool> Symbol::pixels() const {
    return _pixels;
}


const std::vector<Symbol::PixelType> Symbol::pixelType() const {
    return _pixelType;
}


bool Symbol::pixel(int x, int y) const {
    if (!valid(x, y)) { return false; }
    return _pixels[toIndex(x, y)];
}


uint32_t Symbol::highlight(int x, int y) const {
    return _highlight[toIndex(x, y)];
}


void Symbol::highlightCodeword(size_t codewordNo, uint32_t highlight) {
    for (const Position &position : position(codewordNo)) {
        if (position.valid()) {
            _highlight[toIndex(position)] = highlight;
        }
    }
}


void Symbol::setData(const std::vector<uint8_t> &data, QR::ErrorCorrection ec, uint8_t mask) {
    unsigned int lowestPenalty = numeric_limits<unsigned int>::max();
    uint8_t bestMask = mask;
    if (bestMask == 255) {
        for (uint8_t mask = 0; mask < 8; ++mask) {
            drawFormatInformation(mask, ec);
            drawCodewords(data, mask);
            const unsigned int penalty = evaluate();
            if (penalty < lowestPenalty) {
                lowestPenalty = penalty;
                bestMask = mask;
            }
        }
    }
    drawFormatInformation(bestMask, ec);
    drawCodewords(data, bestMask);
}


#ifndef NDEBUG
bool Symbol::test() {
    bool result = true;
    result |= testFormatInformation();
    result |= testEvaluation();

    return result;
}
#endif


void Symbol::drawAlignmentPatterns() {
    assert(_size != 0);
    
    auto drawPattern = [&](int x, int y) {
        drawRect(x - 2, y - 2, 5, 5, true, PixelType::AlignmentPattern);
        drawRect(x - 1, y - 1, 3, 3, false, PixelType::AlignmentPattern);
        drawPixel(x, y, true, PixelType::AlignmentPattern);
    };
    
    for (size_t y = 0; y < positions[_version - 1].size(); ++y) {
        for (size_t x = 0; x < positions[_version - 1].size(); ++x) {
            if (x == 0 && y == 0) { continue; }
            if (x == 0 && y == positions[_version - 1].size() - 1) { continue; }
            if (x == positions[_version - 1].size() - 1 && y == 0) { continue; }
            drawPattern(positions[_version - 1][x], positions[_version - 1][y]);
        }
    }
}


void Symbol::drawCodewords(const std::vector<uint8_t> &data, uint8_t mask) {
    static const array<function<bool(size_t, size_t)>, 8> maskFun = {
        [](size_t j, size_t i) -> bool { return (i + j) % 2 == 0; },
        [](size_t j, size_t i) -> bool { (void)j; return i % 2 == 0; },
        [](size_t j, size_t i) -> bool { (void)i; return j % 3 == 0; },
        [](size_t j, size_t i) -> bool { return (i + j) % 3 == 0; },
        [](size_t j, size_t i) -> bool { return (i / 2 + j / 3) % 2 == 0; },
        [](size_t j, size_t i) -> bool { return i * j % 2 + i * j % 3 == 0; },
        [](size_t j, size_t i) -> bool { return (i * j % 2 + i * j % 3) % 2 == 0; },
        [](size_t j, size_t i) -> bool { return ((i + j) % 2 + i * j % 3) % 2 == 0; }
    };

    Position position(startPosition());
    for (uint8_t codeword : data) {
        for (int bit = 7; bit >= 0; --bit) {
            const size_t i = toIndex(position);
            const bool value = (codeword & (1 << bit)) != 0;
            const bool maskValue = maskFun[mask](position.x, position.y);
            _pixels[i] = value ^ maskValue;
            _pixelType[i] = PixelType::Data;
            position = nextPosition(position);
            if (!position.valid()) { return; }
        }
    }

    for (;position.valid(); position = nextPosition(position)) {
        const size_t i = toIndex(position);
        const bool maskValue = maskFun[mask](position.x, position.y);
        _pixels[i] = maskValue;
        _pixelType[i] = PixelType::Blank;
    }
}


void Symbol::drawDarkModule() {
    drawPixel(8, _size - 8, true, PixelType::VersionInformation);
}


void Symbol::drawFinderPatterns() {
    assert(_size != 0);
    
    auto drawPattern = [&](int x, int y) {
        drawRect(x - 1, y - 1, 9, 9, false, PixelType::FinderPattern);
        drawRect(x, y, 7, 7, true, PixelType::FinderPattern);
        drawRect(x + 1, y + 1, 5, 5, false, PixelType::FinderPattern);
        drawRect(x + 2, y + 2, 3, 3, true, PixelType::FinderPattern);
        drawPixel(x + 3, y + 3, true, PixelType::FinderPattern);
    };
    
    int t = _size - 7;
    
    drawPattern(0, 0);
    drawPattern(t, 0);
    drawPattern(0, t);
}


void Symbol::drawFormatInformation(uint8_t mask, QR::ErrorCorrection ec) {
    // ISO/IEC 18004:2004: see sections 8.9 and appendix C
    // ISO/IEC 18004:2015: see sections 7.9 and appendix C
    // EC information goes into bits 14:13
    // Mask information goes into 12:10
    // Bits 9:0 contain the remainder of EC:Mask:0000000000 / 0b10100110111
    // The 15 bit value is XOR-ed with 0b101010000010010
    assert(mask < 8);
    
    static const Polynomial generatorPolynomial(0b101'0011'0111u);
    uint32_t formatBits;
    switch (ec) {
    case QR::ErrorCorrection::L: formatBits = 0b010'0000'0000'0000u; break;
    case QR::ErrorCorrection::M: formatBits = 0b000'0000'0000'0000u; break;
    case QR::ErrorCorrection::Q: formatBits = 0b110'0000'0000'0000u; break;
    case QR::ErrorCorrection::H: formatBits = 0b100'0000'0000'0000u; break;
    }
    
    // calculate format information bits
    formatBits |= uint32_t(mask) << 10;
    const uint32_t remainder = (Polynomial(formatBits) % generatorPolynomial).value();
    assert(remainder < (1u << 10));
    formatBits |= remainder;
    formatBits ^= 0b101010000010010u;

    // Place format information around 11311 finder patterns
    for (int i = 0; i < 6; ++i) {
        drawPixel(8, i, formatBits & (1 << i), PixelType::FormatInformation);
        drawPixel(_size - 1 - i, 8, formatBits & (1 << i), PixelType::FormatInformation);
    }

    drawPixel(8, 7, formatBits & (1 << 6), PixelType::FormatInformation);
    drawPixel(_size - 7, 8, formatBits & (1 << 6), PixelType::FormatInformation);
    drawPixel(8, 8, formatBits & (1 << 7), PixelType::FormatInformation);
    drawPixel(_size - 8, 8, formatBits & (1 << 7), PixelType::FormatInformation);
    drawPixel(7, 8, formatBits & (1 << 8), PixelType::FormatInformation);
    drawPixel(8, _size - 7, formatBits & (1 << 8), PixelType::FormatInformation);

    drawPixel(8, _size - 8, true, PixelType::FormatInformation); // single dark module at lower left finder pattern

    for (int i = 9; i < 15; ++i) {
        drawPixel(14 - i, 8, formatBits & (1 << i), PixelType::FormatInformation);
        drawPixel(8, _size - 15 + i, formatBits & (1 << i), PixelType::FormatInformation);
    }
}


void Symbol::drawTimingPatterns() {
    assert(_size != 0);
    
    for (int t = 8; t < _size - 8; ++t) {
        drawPixel(t, 6, (t & 1) == 0, PixelType::TimingPattern);
        drawPixel(6, t, (t & 1) == 0, PixelType::TimingPattern);
    }
}


void Symbol::drawVersionInformation() {
    if (_version < 7) { return; }
    
    static const Polynomial generatorPolynomial(0b1'1111'0010'0101u);
    uint32_t versionBits = _version << 12;
    const uint32_t remainder = (Polynomial(versionBits) % generatorPolynomial).value();
    assert(remainder < (1u << 12));
    versionBits |= remainder;
    
    for (int i = 0; i < 18; ++i) {
        const int x = i / 3;
        const int y = _size - 11 + i % 3;
        const bool bit = (versionBits & (1u << i)) != 0;
        drawPixel(x, y, bit, PixelType::VersionInformation);
        drawPixel(y, x, bit, PixelType::VersionInformation);
    }
}


unsigned int Symbol::evaluate() const {
    const unsigned int a = evaluateAdjacentSameColor();
    const unsigned int b = evaluateSameColorBlocks();
    const unsigned int c = evaluate11311Pattern();
    const unsigned int d = evaluateDarkProportion();
    return a + b + c + d;
}


unsigned int Symbol::evaluateAdjacentSameColor() const {
    static constexpr unsigned int N1 = 3;

    unsigned int result = 0;

    // find horizontally adjacent pixels of the same color
    for (int row = 0; row < _size; ++row) {
        bool runColor = _pixels[toIndex(0, row)];
        unsigned int run = 1;
        for (int col = 1; col < _size; ++col, ++run) {
            const bool pixelColor = _pixels[toIndex(col, row)];
            if (runColor != pixelColor) {
                if (run >= 5) { result += N1 + run - 5u; }
                runColor = pixelColor;
                run = 0;
            }
        }
        if (run >= 5) { result += N1 + run - 5u; }
    }

    // find vertically adjacent pixels of the same color
    for (int col = 0; col < _size; ++col) {
        bool runColor = _pixels[toIndex(col, 0)];
        unsigned int run = 1;
        for (int row = 1; row < _size; ++row, ++run) {
            const bool pixelColor = _pixels[toIndex(col, row)];
            if (runColor != pixelColor) {
                if (run >= 5) { result += N1 + run - 5u; }
                runColor = pixelColor;
                run = 0;
            }
        }
        if (run >= 5) { result += N1 + run - 5u; }
    }

    return result;
}


unsigned int Symbol::evaluateSameColorBlocks() const {
    static constexpr unsigned int N2 = 3;

    unsigned int result = 0;

    for (int row = 0; row < _size - 1; ++row) {
        for (int col = 0; col < _size - 1; ++col) {
            const array<bool, 4> colors = {
                _pixels[toIndex(col, row)],
                _pixels[toIndex(col + 1, row)],
                _pixels[toIndex(col, row + 1)],
                _pixels[toIndex(col + 1, row + 1)],
            };

            unsigned int score = N2;
            for (size_t i = 1; i < colors.size(); ++i) {
                if (colors[0] != colors[i]) {
                    score = 0;
                    break;
                }
            }

            result += score;
        }
    }

    return result;
}


unsigned int Symbol::evaluate11311Pattern() const {
    static constexpr unsigned int N3 = 40;
    static constexpr size_t PatLen{15};
    static constexpr bool w = false;
    static constexpr bool b = true;
    static const array<bool, PatLen> Pattern = { w, w, w, w, b, w, b, b, b, w, b, w, w, w, w };

    auto getPixel = [&](int col, int row) -> bool {
        if (col < 0 || int(_size) <= col || row < 0 || int(_size) <= row) {
            return w;
        } else {
            return _pixels[toIndex(col, row)];
        }
    };

    size_t result = 0;
    
    for (size_t scale = 1; int(scale * PatLen) < (_size + 8); ++scale) {
        for (size_t i = 0; i <= _size - scale; ++i) {
            for (int j = -4; j <= int(_size) + 4 - int(scale * PatLen); ++j) {
                // match horizontally
                bool match = true;
                for (size_t patElem = 0; patElem < PatLen; ++patElem) {
                    for (size_t k = 0; k < scale; ++k) {
                        if (getPixel(j + scale * patElem + k, i) != Pattern[patElem]) {
                            match = false;
                            break;
                        }
                    }
                    if (!match) { break; }
                }
                if (match) {
                    result += N3;
                }

                // match vertically
                match = true;
                for (size_t patElem = 0; patElem < PatLen; ++patElem) {
                    for (size_t k = 0; k < scale; ++k) {
                        if (getPixel(i, j + scale * patElem + k) != Pattern[patElem]) {
                            match = false;
                            break;
                        }
                    }
                    if (!match) { break; }
                }
                if (match) {
                    result += N3;
                }
            }
        }
    }
    return result;
}


unsigned int Symbol::evaluateDarkProportion() const {
    static constexpr unsigned int N4 = 10;

    const size_t darkCount = count_if(_pixels.begin(), _pixels.end(), identityFun<bool>);
    int darkProportion = 20 * darkCount / (_size * _size) - 10;
    if (2 * darkCount < (_size * _size)) { darkProportion += 1; }
    return abs(darkProportion) * N4;
}


size_t Symbol::toIndex(int x, int y) const {
    return y * _size + x;
}


size_t Symbol::toIndex(const Position &position) const {
    return toIndex(position.x, position.y);
}


bool Symbol::valid(int x, int y) const {
    return 0 <= x && x < _size && 0 <= y && y < _size;
}


void Symbol::drawPixel(int x, int y, bool color, PixelType pixelType) {
    if (valid(x, y)) {
        size_t i = toIndex(x, y);
        _pixels[i] = color;
        _pixelType[i] = pixelType;
    }
}


void Symbol::drawRect(int x, int y, int w, int h, bool color, PixelType pixelType) {
    for (int i = 0; i < w ; ++i) {
        drawPixel(x + i, y, color, pixelType);
        if (h != 1) {
            drawPixel(x + i, y + h - 1, color, pixelType);
        }
    }
    for (int i = 1; i < h - 1; ++i) {
        drawPixel(x, y + i, color, pixelType);
        if (w != 1) {
            drawPixel(x + w - 1, y + i, color, pixelType);
        }
    }
}


array<Symbol::Position, 8> Symbol::position(int codeword) {
    array<Position, 8> result;
    result.fill({-1, -1, false});

    if (codeword < 0) { return result; }

    Position position(startPosition());

    for (int i = 0; i < codeword; ++i) {
        for (int j = 0; j < 8; ++j) {
            position = nextPosition(position);
            if (!position.valid()) { return result; }
        }
    }

    for (int i = 0; i < 8; ++i) {
        result[i] = position;
        position = nextPosition(position);
        if (!position.valid()) { return result; }
    }

    return result;
}


Symbol::Position Symbol::nextPosition(Position position) const {
    static auto isDataPosition = [](PixelType pixelType) -> bool {
        return pixelType == PixelType::Unset || pixelType == PixelType::Data;
    };
    
    do {
        if (((position.x & 1) == 0) == (position.x > 6)) {
            --position.x;
        } else {
            ++position.x;
            if (position.upwards) {
                if (position.y > 0) {
                    --position.y;
                } else {
                    position.x -= 2;
                    if (position.x == 6) { position.x = 5; }
                    position.upwards = false;
                }
            } else {
                if (position.y < int(_size) - 1) {
                    ++position.y;
                } else {
                    position.x -= 2;
                    if (position.x == 6) { position.x = 5; }
                    position.upwards = true;
                }
            }
        }
        
    } while (position.valid() && !isDataPosition(_pixelType[toIndex(position)]));

    return position;
}


Symbol::Position Symbol::startPosition() const {
    return Position{_size - 1, _size - 1, true};
}


uint_fast16_t Symbol::formatInformation(uint8_t mask, QR::ErrorCorrection ec) {
    static constexpr uint_fast16_t xorMask{0b10101'00000'10010};
    static const Polynomial divisor{0b1'01001'10111};

    uint_fast16_t result;
    switch (ec) {
    case QR::ErrorCorrection::L: result = 0b010'0000'0000'0000; break;
    case QR::ErrorCorrection::M: result = 0b000'0000'0000'0000; break;
    case QR::ErrorCorrection::Q: result = 0b110'0000'0000'0000; break;
    case QR::ErrorCorrection::H: result = 0b100'0000'0000'0000; break;
    }

    assert(mask < 8);
    result |= static_cast<unsigned int>(mask) << 10;
    unsigned int remainder = (Polynomial(result) % divisor).value();
    assert(remainder < 0b1'00000'00000);
    result |= remainder;
    result ^= xorMask;
    return result;
}


#ifndef NDEBUG
bool Symbol::testFormatInformation() {
    // These values are from Table C.1 in ISO/IEC 18004:2015
    constexpr array<uint16_t, 32> expectedFormats = {
        0x5412, 0x5125, 0x5E7C, 0x5B4B, 0x45F9, 0x40CE, 0x4F97, 0x4AA0,
        0x77C4, 0x72F3, 0x7DAA, 0x789D, 0x662F, 0x6318, 0x6C41, 0x6976,
        0x1689, 0x13BE, 0x1CE7, 0x19D0, 0x0762, 0x0255, 0x0D0C, 0x083B,
        0x355F, 0x3068, 0x3F31, 0x3A06, 0x24B4, 0x2183, 0x2EDA, 0x2BED,
    };

    using EC = QR::ErrorCorrection;

    bool result = true;

    size_t counter = 0;
    // the order is intentionally M-L-H-Q, see ISO/IEC 18004:2015 table 12
    for (EC ec : { EC::M, EC::L, EC::H, EC::Q }) {
        for (uint8_t mask = 0; mask < 8; ++mask, ++counter) {
            uint16_t actual = formatInformation(mask, ec);
            uint16_t expected = expectedFormats[counter];
            result |= testEqual(expected, actual);
        }
    }

    return result;
}


bool Symbol::testEvaluation() {
    Symbol symbol(1);

    unsigned int actualAdjacentSameColor = symbol.evaluateAdjacentSameColor();
    unsigned int actualSameColorBlocks = symbol.evaluateSameColorBlocks();
    unsigned int actual11311Pattern = symbol.evaluate11311Pattern();
    unsigned int actualDarkProportion = symbol.evaluateDarkProportion();
    unsigned int actualTotal = symbol.evaluate();

    bool result = true;

    result |= testEqual(actualTotal, actualAdjacentSameColor + actualSameColorBlocks
                                         + actual11311Pattern + actualDarkProportion);
    result |= testEqual(566u, actualAdjacentSameColor);
    result |= testEqual(711u, actualSameColorBlocks);
    result |= testEqual(720u, actual11311Pattern);
    result |= testEqual(50u, actualDarkProportion);

    return result;
}
#endif // NDEBUG


bool Symbol::Position::valid() const {
    return x >= 0 && y >= 0;
}
