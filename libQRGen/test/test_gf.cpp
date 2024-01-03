// Copyright 2024 Benjamin Lutz.
// 
// This file is part of QRGen. QRGen is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.

#include <gtest/gtest.h>
#include <unordered_set>
#define private public
#include "../src/gf.h"


TEST(GF256, identityElementsQR) {
    using GF = GF256<GF256_RP::QR>;
    auto zero = GF::zero();
    auto one = GF::one();
    auto alpha = GF::alpha();
    auto alpha2 = GF::alpha(2);
    
    EXPECT_EQ(zero + zero, zero);
    EXPECT_EQ(one + zero, one);
    EXPECT_EQ(alpha + zero, alpha);
    
    EXPECT_EQ(zero - zero, zero);
    EXPECT_EQ(one - zero, one);
    EXPECT_EQ(alpha - zero, alpha);
    
    EXPECT_EQ(one * one, one);
    EXPECT_EQ(alpha * one, alpha);
    EXPECT_EQ(alpha2 * one, alpha2);
}


TEST(GF256, identityElementsRijndael) {
    using GF = GF256<GF256_RP::Rijndael>;
    auto zero = GF::zero();
    auto one = GF::one();
    auto alpha = GF::alpha();
    auto alpha2 = GF::alpha(2);
    
    EXPECT_EQ(zero + zero, zero);
    EXPECT_EQ(one + zero, one);
    EXPECT_EQ(alpha + zero, alpha);
    
    EXPECT_EQ(zero - zero, zero);
    EXPECT_EQ(one - zero, one);
    EXPECT_EQ(alpha - zero, alpha);
    
    EXPECT_EQ(one * one, one);
    EXPECT_EQ(alpha * one, alpha);
    EXPECT_EQ(alpha2 * one, alpha2);
}


TEST(GF256, identityElements19F) {
    using GF = GF256<GF256_RP::P19F>;
    auto zero = GF::zero();
    auto one = GF::one();
    auto alpha = GF::alpha();
    auto alpha2 = GF::alpha(2);
    
    EXPECT_EQ(zero + zero, zero);
    EXPECT_EQ(one + zero, one);
    EXPECT_EQ(alpha + zero, alpha);
    
    EXPECT_EQ(zero - zero, zero);
    EXPECT_EQ(one - zero, one);
    EXPECT_EQ(alpha - zero, alpha);
    
    EXPECT_EQ(one * one, one);
    EXPECT_EQ(alpha * one, alpha);
    EXPECT_EQ(alpha2 * one, alpha2);
}


TEST(GF256, primitiveElementQR) {
    using GF = GF256<GF256_RP::QR>;
    auto alpha = GF::alpha;
    std::unordered_set<uint8_t> elements;
    
    for (int i = 0; i < 255; ++i) {
        EXPECT_EQ(elements.find(alpha(i)), elements.end());
        elements.insert(alpha(i));
    }
    EXPECT_EQ(alpha(0), GF::one());
    EXPECT_EQ(alpha(256), alpha(1));
}


TEST(GF256, primitiveElementRijndael) {
    using GF = GF256<GF256_RP::Rijndael>;
    auto alpha = GF::alpha;
    std::unordered_set<uint8_t> elements;
    
    for (int i = 0; i < 255; ++i) {
        EXPECT_EQ(elements.find(alpha(i)), elements.end());
        elements.insert(alpha(i));
    }
    EXPECT_EQ(alpha(0), GF::one());
    EXPECT_EQ(alpha(256), alpha(1));
}


TEST(GF256, primitiveElement19F) {
    using GF = GF256<GF256_RP::P19F>;
    auto alpha = GF::alpha;
    std::unordered_set<uint8_t> elements;
    
    for (int i = 0; i < 255; ++i) {
        EXPECT_EQ(elements.find(alpha(i)), elements.end());
        elements.insert(alpha(i));
    }
    EXPECT_EQ(alpha(0), GF::one());
    EXPECT_EQ(alpha(256), alpha(1));
}


TEST(GF256, logAlphaQR) {
    using GF = GF256<GF256_RP::QR>;
    
    for (uint8_t i = 0; i < 255; ++i) {
        EXPECT_EQ(GF::logAlpha(GF::alpha(i)), i);
    }
}


TEST(GF256, logAlphaRijndael) {
    using GF = GF256<GF256_RP::Rijndael>;
    
    for (uint8_t i = 0; i < 255; ++i) {
        EXPECT_EQ(GF::logAlpha(GF::alpha(i)), i);
    }
}


TEST(GF256, logAlphaP19F) {
    using GF = GF256<GF256_RP::P19F>;
    
    for (uint8_t i = 0; i < 255; ++i) {
        EXPECT_EQ(GF::logAlpha(GF::alpha(i)), i);
    }
}


TEST(GF256, multiplicationQR) {
    using GF = GF256<GF256_RP::QR>;
    
    for (unsigned i = 1; i < 256; ++i) {
        for (unsigned j = 1; j < 256; ++j) {
            unsigned longResult = GF::mulLong(i, j);
            unsigned peasantResult = GF::mulPeasant(i, j);
            unsigned lookupResult = GF::mulLookup(i, j);
            EXPECT_EQ(longResult, peasantResult);
            EXPECT_EQ(longResult, lookupResult);
        }
    }
}
