// Copyright 2024 Benjamin Lutz.
// 
// This file is part of QRGen. QRGen is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.

#ifndef QR_H
#define QR_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include "data.h"
#include "qrgen.h"
#include "symbol.h"


class QR
{
public:
    
    QR() = delete;
    
    static Symbol encode(std::u16string_view data,
                         QRGen_ErrorCorrection ec = QRGen_EC_M,
                         uint8_t version = 0,
                         uint8_t mask = 255);
    
private:
    enum class Mode : uint8_t { automatic = 16, eci = 7, numeric = 1, alphanumeric = 2,
                                eightbit = 4, kanji = 8, structuredAppend = 3,
                                fnc1_first = 5, fnc1_second = 9, terminator = 0 };
    struct EncodeResult {
        bool success;
        Data bits;
        Mode mode;
        uint16_t characterCount;
    };
    
    static EncodeResult encodeSegment(std::u16string_view data, QRGen_ErrorCorrection ec);
    static EncodeResult encodeContent(std::u16string_view data);
    /** Add error correction codewords and put everything into the final sequence order. */
    static std::vector<uint8_t> finalSequence(Data &bits, uint8_t version, QRGen_ErrorCorrection ec);
    
    static bool isNumeric(char16_t c);
    static bool isNumeric(std::u16string_view s);
    static bool isAlphaNumeric(char16_t c);
    static bool isAlphaNumeric(std::u16string_view s);
    
    static uint32_t characterCountBits(uint8_t version, Mode encodeMode);
    
    static EncodeResult encodeNumeric(std::u16string_view data);
    static EncodeResult encodeAlphanumeric(std::u16string_view data);
    static EncodeResult encodeEightbit(std::u16string_view data);
    
    static uint8_t minimumVersion(uint32_t numContentData, QRGen_ErrorCorrection ec);
    static uint8_t minimumVersion(EncodeResult encodeResult, QRGen_ErrorCorrection ec);
    
    static std::string toString(Mode mode);
    
    /** The number of data bits a QR code can hold, per version, per error correction type */
    static const std::array<std::array<uint16_t, 4>, 40> dataBitsCounts;
    
    /** The number of error correction codewords required, per version, per error correction type. */
    static const std::array<std::array<uint16_t, 4>, 40> ecCodewordsCounts;
    
    /**
     * The number of error correction codes per block. Array indices have the
     * following meanings, from the outside in:
     *   - version (array index 0-39 correspond to version 1-40)
     *   - error correction level (0: L, 1: M, 2: Q, 3: H)
     *   - type of error correction block (0: first, 1: second)
     *   - error correction block specification:
     *     * number of such blocks (can be 0)
     *     * total number of codewords
     *     * number of data codewords
     */
    static const std::array<std::array<std::array<std::array<uint16_t, 3>, 2>, 4>, 40> ecBlocks;
};

#endif // QR_H
