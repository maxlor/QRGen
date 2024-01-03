// Copyright 2024 Benjamin Lutz.
// 
// This file is part of QRGen. QRGen is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.

#include <gtest/gtest.h>
#include <vector>
#define private public
#include "../src/ecccalculator.h"

using namespace std;


TEST(ECCCalculator, eccCalculation) {
    // The values are from the example given in Annex I of ISO/IEC 18004:2015.
    const vector<uint8_t> codewords {
        0b0001'0000, 0b0010'0000, 0b0000'1100, 0b0101'0110, 0b0110'0001, 0b1000'0000,
        0b1110'1100, 0b0001'0001, 0b1110'1100, 0b0001'0001, 0b1110'1100, 0b0001'0001,
        0b1110'1100, 0b0001'0001, 0b1110'1100, 0b0001'0001
    };
    const vector<uint8_t> expectedEcCodewords {
        0b1010'0101, 0b0010'0100, 0b1101'0100, 0b1100'0001, 0b1110'1101, 0b0011'0110,
        0b1100'0111, 0b1000'0111, 0b0010'1100, 0b01010101
    };
    const vector<uint8_t> actualEcCodewords =
        ECCCalculator::feed(codewords.begin(), codewords.end(), 10);
    
    EXPECT_EQ(expectedEcCodewords, actualEcCodewords);
    
    ECCCalculator eccc{10};
    for (uint8_t cw : codewords) { eccc.feed(cw); }
    EXPECT_EQ(expectedEcCodewords, eccc.errorCodeWords());
    
    // test that reset works
    eccc.reset();
    for (uint8_t cw : codewords) { eccc.feed(cw); }
    EXPECT_EQ(expectedEcCodewords, eccc.errorCodeWords());
}


TEST(ECCCalculator, polynomialGeneration) {
    const vector<uint8_t> &p7 = ECCCalculator::generatorPolynomial(7);
    vector<uint8_t> expected{21, 102, 238, 149, 146, 229, 87};
    EXPECT_EQ(expected, p7);
}
