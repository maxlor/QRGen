// Copyright 2024 Benjamin Lutz.
// 
// This file is part of QRGen. QRGen is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.

#include <gtest/gtest.h>
#include "qrgen.h"
#define private public
#include "../src/qr.h"


TEST(QR, encodeSegment) {
    // This is the example from Annex I of ISO 18004:2015.
    QR::EncodeResult result = QR::encodeSegment(u"01234567", QRGen::ErrorCorrection::M);
    Data expected;
    expected.append(4, 0b0001);
    expected.append(10, 0b0000001000);
    expected.append(10, 0b0000001100);
    expected.append(10, 0b0101011001);
    expected.append(7, 0b1000011);
    expected.append(4, 0b0000);
    
    EXPECT_EQ(result.bits.bitCount(), expected.bitCount());
    EXPECT_EQ(result.bits, expected);
}


TEST(QR, tables) {
    // Check that multidimensional array initialization is correct
    EXPECT_EQ(QR::ecCodewordsCounts[3][1], 36);
    
    EXPECT_EQ(QR::ecBlocks[0][0][0][2], 19);
    EXPECT_EQ(QR::ecBlocks[0][0][1][2], 0);
}
