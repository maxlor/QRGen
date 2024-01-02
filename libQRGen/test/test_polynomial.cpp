#include <gtest/gtest.h>
#include "../src/polynomial.h"


TEST(Polynomial, testAdditionSubtraction) {
    const Polynomial a(0b1010);
    const Polynomial b(0b0110);
    const Polynomial r = a + b;
    const Polynomial s = a - b;
    EXPECT_EQ(r, Polynomial(0b1100));
    EXPECT_EQ(s, Polynomial(0b1100));
}


TEST(Polynomial, testMultiplication) {
    const Polynomial a(0b0010);
    const Polynomial b(0b1100);
    const Polynomial r = a * b;
    EXPECT_EQ(r, Polynomial(0b11000));
}


TEST(Polynomial, testModulo) {
    // This is the example from Annex C of ISO/IEC 18004:2015
    const Polynomial a(0b1'0100'0000'0000);
    const Polynomial b(0b101'0011'0111);
    const Polynomial r = a % b;
    EXPECT_EQ(r, Polynomial(0b11011100));
}


TEST(Polynomial, testComparisonOperator) {
    const Polynomial a(0);
    const Polynomial b(0);
    const Polynomial c(1);
    
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
}
