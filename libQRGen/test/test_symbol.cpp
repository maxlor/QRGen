// Copyright 2024 Benjamin Lutz.
// 
// This file is part of QRGen. QRGen is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.

#include <gtest/gtest.h>
#include <array>
#include <vector>
#include "../src/qr.h"
#include "../src/symbol.h"


TEST(Symbol, invalidSize) {
    for (uint8_t i = 41; i != 1; ++i) { // intentional wrap-around to test size 0
        EXPECT_EQ(Symbol(i).size(), 0);
    }
}


TEST(Symbol, structure) {
    static auto copySquare = [](const Symbol &symbol, int x, int y, int size) -> std::vector<bool> {
        std::vector<bool> result(size * size);
        for (int j = 0; j < size; ++j) {
            for (int i = 0; i < size; ++i) {
                result[i + j * size] = symbol.pixel(x + i, y + j);
            }
        }
        return result;
    };
    
    static const std::vector<bool> finderPattern = {
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 0, 0, 0, 0, 0, 1, 0,
        0, 1, 0, 1, 1, 1, 0, 1, 0,
        0, 1, 0, 1, 1, 1, 0, 1, 0,
        0, 1, 0, 1, 1, 1, 0, 1, 0,
        0, 1, 0, 0, 0, 0, 0, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    
    static const std::vector<bool> alignmentPattern = {
        1, 1, 1, 1, 1,
        1, 0, 0, 0, 1,
        1, 0, 1, 0, 1,
        1, 0, 0, 0, 1,
        1, 1, 1, 1, 1,
    };
    
    for (uint8_t version = 1; version <= 40; ++version) {
        const Symbol symbol(version);
        
        // Test symbol size
        EXPECT_EQ(symbol.size(), 17 + 4 * version);
        
        // Test finder patterns
        EXPECT_EQ(copySquare(symbol, -1, -1, 9), finderPattern);
        const int k = symbol.size() - 8;
        EXPECT_EQ(copySquare(symbol, k, -1, 9), finderPattern);
        EXPECT_EQ(copySquare(symbol, -1, k, 9), finderPattern);
        
        // Test timing patterns
        for (int i = 8; i < int(symbol.size()) - 8; ++i) {
            EXPECT_EQ(symbol.pixel(i, 6), i & 1 ? false : true);
            EXPECT_EQ(symbol.pixel(6, i), i & 1 ? false : true);
        }
        
        // Test bottom right alignment pattern
        if (version > 2) {
            const int k = symbol.size() - 9;
            EXPECT_EQ(copySquare(symbol, k, k, 5), alignmentPattern);
        }
        
        // Test version information
        if (version >= 7) {
            unsigned int v1 = 0;
            unsigned int v2 = 0;
            for (int i = 12; i < 18; ++i) {
                const int x = i / 3;
                const int y = symbol.size() - 11 + i % 3;
                v1 |= symbol.pixel(x, y) ? (1 << (i - 12)) : 0;
                v2 |= symbol.pixel(y, x) ? (1 << (i - 12)) : 0;
            }
            EXPECT_EQ(version, v1);
            EXPECT_EQ(version, v2);
        }
    }
}


TEST(Symbol, formatInformation) {
    // These values are from Table C.1 in ISO/IEC 18004:2015
    constexpr std::array<uint16_t, 32> expectedFormats = {
        0x5412, 0x5125, 0x5E7C, 0x5B4B, 0x45F9, 0x40CE, 0x4F97, 0x4AA0,
        0x77C4, 0x72F3, 0x7DAA, 0x789D, 0x662F, 0x6318, 0x6C41, 0x6976,
        0x1689, 0x13BE, 0x1CE7, 0x19D0, 0x0762, 0x0255, 0x0D0C, 0x083B,
        0x355F, 0x3068, 0x3F31, 0x3A06, 0x24B4, 0x2183, 0x2EDA, 0x2BED,
    };
    
    using EC = QR::ErrorCorrection;
    
    size_t counter = 0;
    // the order is intentionally M-L-H-Q, see ISO/IEC 18004:2015 table 12
    for (EC ec : { EC::M, EC::L, EC::H, EC::Q }) {
        for (uint8_t mask = 0; mask < 8; ++mask, ++counter) {
            Symbol symbol(1);
            symbol.setData({0}, ec, mask);
            
            uint32_t f1 = 0;
            for (int i = 0; i < 8; ++i) {
                f1 |= symbol.pixel(symbol.size() - 1 - i, 8) ? (1 << i) : 0;
            }
            for (int i = 8; i < 15; ++i) {
                f1 |= symbol.pixel(8, symbol.size() - 15 + i) ? (1 << i) : 0;
            }
            
            uint32_t f2 = 0;
            for (int i = 0; i < 6; ++i) {
                f2 |= symbol.pixel(8, i) ? (1 << i) : 0;
            }
            f2 |= symbol.pixel(8, 7) ? (1 << 6) : 0;
            f2 |= symbol.pixel(8, 8) ? (1 << 7) : 0;
            f2 |= symbol.pixel(7, 8) ? (1 << 8) : 0;
            for (int i = 9; i < 15; ++i) {
                f2 |= symbol.pixel(14 - i, 8) ? (1 << i) : 0;
            }
            
            EXPECT_EQ(f1, expectedFormats[counter]);
            EXPECT_EQ(f2, expectedFormats[counter]);
        }
    }
}
