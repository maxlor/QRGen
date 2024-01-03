#include <gtest/gtest.h>
#define private public
#include "../src/data.h"


TEST(Data, defaultConstructor) {
    Data data;
    EXPECT_EQ(data.size(), 0);
    EXPECT_EQ(data.bitCount(), 0);
}


TEST(Data, initializerListConstructor) {
    Data data{0x12, 0x34, 0x56, 0x78};
    EXPECT_EQ(data.size(), 4);
    EXPECT_EQ(data.bitCount(), 4 * Data::TBitSize);
    EXPECT_EQ(data.at(0), 0x12);
    EXPECT_EQ(data.at(1), 0x34);
    EXPECT_EQ(data.at(2), 0x56);
    EXPECT_EQ(data.at(3), 0x78);
}


TEST(Data, append) {
    Data data;    
    data.append(6, 255);
    EXPECT_EQ(data.size(), 1);
    EXPECT_EQ(data.bitCount(), 6);
    EXPECT_EQ(data.at(0), 252);
    
    data.append(14, 8192 + 3);
    EXPECT_EQ(data.size(), 3);
    EXPECT_EQ(data.bitCount(), 20);
    EXPECT_EQ(data.at(0), 254);
    EXPECT_EQ(data.at(1), 0);
    EXPECT_EQ(data.at(2), 48);
    
    data.append(data);
    EXPECT_EQ(data.size(), 5);
    EXPECT_EQ(data.bitCount(), 40);
    // 1111'1110 0000'0000 0011'1111 1110'0000 0000'0011
    EXPECT_EQ(data.at(0), 254);
    EXPECT_EQ(data.at(1), 0);
    EXPECT_EQ(data.at(2), 63);
    EXPECT_EQ(data.at(3), 224);
    EXPECT_EQ(data.at(4), 3);
}


TEST(Data, append2) {
    Data data;
    data.append(3, 7);
    data.append(3, 0);
    data.append(3, 7);
    EXPECT_EQ(data.size(), 2);
    EXPECT_EQ(data.bitCount(), 9);
    EXPECT_EQ(data.at(0), 0xE3);
    EXPECT_EQ(data.at(1), 0x80);
}


TEST(Data, append3) {
    Data data;
    data.append(40, 0x12345678);
    EXPECT_EQ(data.size(), 5);
    EXPECT_EQ(data.bitCount(), 40);
    EXPECT_EQ(data.at(0), 0x00);
    EXPECT_EQ(data.at(1), 0x12);
    EXPECT_EQ(data.at(2), 0x34);
    EXPECT_EQ(data.at(3), 0x56);
    EXPECT_EQ(data.at(4), 0x78);
}


TEST(Data, append4) {
    Data data;
    data.append(12, 0x123);
    data.append(data);
    EXPECT_EQ(data.size(), 3);
    EXPECT_EQ(data.bitCount(), 24);
    EXPECT_EQ(data.at(0), 0x12);
    EXPECT_EQ(data.at(1), 0x31);
    EXPECT_EQ(data.at(2), 0x23);
}


TEST(Data, clear) {
    Data data;
    data.append(8, 42);
    EXPECT_EQ(data.size(), 1);
    EXPECT_EQ(data.bitCount(), 8);
    
    data.clear();
    EXPECT_EQ(data.size(), 0);
    EXPECT_EQ(data.bitCount(), 0);
}


TEST(Data, padLastByte) {
    Data data;
    data.append(1, 1);
    data.padLastByte();
    EXPECT_EQ(data.size(), 1);
    EXPECT_EQ(data.bitCount(), 8);
    EXPECT_EQ(data.at(0), 0x80);
    
    data.append(2, 3);
    data.padLastByte();
    EXPECT_EQ(data.size(), 2);
    EXPECT_EQ(data.bitCount(), 16);
    EXPECT_EQ(data.at(0), 0x80);
    EXPECT_EQ(data.at(1), 0xC0);
}


TEST(Data, comparisonOperator) {
    Data a;
    a.append(8, 42);
    Data b;
    b.append(7, 42);
    Data c;
    c.append(7, 42 + 128);
    
    EXPECT_TRUE(a == a);
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(b == c);
}


TEST(Data, appendZeros) {
    Data data;
    data.appendZeros(9);
    EXPECT_EQ(data.size(), 2);
    EXPECT_EQ(data.bitCount(), 9);
    
    data.appendZeros(7);
    EXPECT_EQ(data.size(), 2);
    EXPECT_EQ(data.bitCount(), 16);
}
