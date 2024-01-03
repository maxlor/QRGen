// Copyright 2024 Benjamin Lutz.
// 
// This file is part of QRGen. QRGen is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.

#ifndef GF_H
#define GF_H

#include <array>
#include <cassert>
#include <cstdint>
#include <map>
#include <utility>


/** All reducing polynomials in GF(256). The highest bit is implicit in the value. */
enum class GF256_RP : uint8_t {
    P11B = 0x1B,  P11D = 0x1D,  P12B = 0x2B,  P12D = 0x2D,  P139 = 0x39,  P13F = 0x3F, 
    P14D = 0x4D,  P15F = 0x5F,  P163 = 0x63,  P165 = 0x65,  P169 = 0x69,  P171 = 0x71, 
    P177 = 0x77,  P17B = 0x7B,  P187 = 0x87,  P18B = 0x8B,  P18D = 0x8D,  P19F = 0x9F, 
    P1A3 = 0xA3,  P1A9 = 0xA9,  P1B1 = 0xB1,  P1BD = 0xBD,  P1C3 = 0xC3,  P1CF = 0xCF, 
    P1D7 = 0xD7,  P1DD = 0xDD,  P1E7 = 0xE7,  P1F3 = 0xF3,  P1F5 = 0xF5,  P1F9 = 0xF9,
    Rijndael = P11B, QR = P11D
};


/**
 * Operations on GF(2^8).
 * 
 * Implements operations on the Galois field (finite field) with 2^8 elements.
 * 
 * The elements are: 0, 1, α, α², α³, ..., α^254
 * 
 * You may construct Elements directly from an uint8_t value, or use
 * GF256::zero(), GF256::one() and GF256::alpha().
 */
template <GF256_RP RP>
class GF256 {
public:
    /**
     * An element of GF(2^8).
     * 
     * It implements addition, subtraction, multiplication and division.
     * 
     * Element implements the conversion constructor and casting operator for
     * uint8_t.
     */
    class Element {
    public:
        Element(uint8_t value = 0);
        
        Element operator+(const Element &other) const;
        Element operator-(const Element &other) const;
        Element operator*(const Element &other) const;
        Element operator/(const Element &other) const;
        bool operator==(const Element &other) const;
        operator uint8_t() const;
        operator int() const;
        
    private:
        uint8_t _value;
    };
    
    GF256() = delete;
    
    static Element zero(); ///< Returns the identity element for addition.
    static Element one();  ///< Returns the identity element for multiplication.
    static Element alpha(int n = 1); ///< Returns α^n.
    
    static uint8_t logAlpha(Element e); ///< Returns log_alpha(value)
    
private:
    static uint8_t mulLong(uint8_t a, uint8_t b);
    static uint8_t mulPeasant(uint8_t a, uint8_t b);
    static uint8_t mulLookup(uint8_t a, uint8_t b);
    
    struct AlphaTables {
        std::array<GF256<RP>::Element, 256> pow;
        std::array<uint8_t, 256> log;
    };
    
    static AlphaTables generateAlphaTables();
    
    static const AlphaTables _alphaTables;
};


template <GF256_RP RP>
GF256<RP>::Element::Element(uint8_t value) : _value(value) {}


template <GF256_RP RP>
typename GF256<RP>::Element GF256<RP>::Element::operator+(const Element &other) const {
    return _value ^ other._value;
}


template <GF256_RP RP>
typename GF256<RP>::Element GF256<RP>::Element::operator-(const Element &other) const {
    return _value ^ other._value;
}


template <GF256_RP RP>
typename GF256<RP>::Element GF256<RP>::Element::operator*(const Element &other) const {
    return mulLookup(_value, other._value);
}


template <GF256_RP RP>
typename GF256<RP>::Element GF256<RP>::Element::operator/(const Element &other) const {
    Element inverse = alpha(-_alphaTables.log[other._value]);
    return *this * inverse;
}


template <GF256_RP RP>
bool GF256<RP>::Element::operator==(const Element &other) const {
    return _value == other._value;
}


template <GF256_RP RP>
GF256<RP>::Element::operator uint8_t() const {
    return _value;
}


template <GF256_RP RP>
GF256<RP>::Element::operator int() const {
    return static_cast<int>(_value) & 0xFF;
}


template <GF256_RP RP>
typename GF256<RP>::AlphaTables GF256<RP>::generateAlphaTables() {
    // This map contains all reducing polynomials over GF(256) and the corresponding
    // smallest primitive elements.
    static const std::map<uint8_t, uint8_t> GF256RPs{
        {0x1B, 3}, {0x1D, 2}, {0x2B, 2}, {0x2D, 2}, {0x39, 3}, {0x3F, 3}, {0x4D, 2}, {0x5F, 2},
        {0x63, 2}, {0x65, 2}, {0x69, 2}, {0x71, 2}, {0x77, 3}, {0x7B, 9}, {0x87, 2}, {0x8B, 6},
        {0x8D, 2}, {0x9F, 3}, {0xA3, 3}, {0xA9, 2}, {0xB1, 6}, {0xBD, 7}, {0xC3, 2}, {0xCF, 2},
        {0xD7, 7}, {0xDD, 6}, {0xE7, 2}, {0xF3, 6}, {0xF5, 2}, {0xF9, 3}
    };
    assert(GF256RPs.find(static_cast<std::underlying_type<GF256_RP>::type>(RP)) != GF256RPs.end());
    
    AlphaTables result;
    Element alpha{GF256RPs.find(static_cast<std::underlying_type<GF256_RP>::type>(RP))->second};
    result.pow[0] = one();
    result.log.fill(0);
    for (std::size_t i = 1; i < result.pow.size(); ++i) {
        GF256<RP>::Element value{mulPeasant(result.pow[i - 1], alpha)};
        result.pow[i] = value;
        result.log[uint8_t(value)] = i;
    }
    result.log[1] = 0;
    return result;
}


template<GF256_RP RP>
typename GF256<RP>::Element GF256<RP>::zero() {
    return 0;
}


template<GF256_RP RP>
typename GF256<RP>::Element GF256<RP>::one() {
    return 1;
}


template<GF256_RP RP>
typename GF256<RP>::Element GF256<RP>::alpha(int n) {
    return _alphaTables.pow[n % 255 + (n >= 0 ? 0 : 255)];
}


template<GF256_RP RP>
uint8_t GF256<RP>::logAlpha(Element e) {
    return _alphaTables.log[static_cast<uint8_t>(e)];
}


template<GF256_RP RP>
uint8_t GF256<RP>::mulLong(uint8_t a, uint8_t b) {
    // Note: doesn't work if a or b are 0.
    // This function is not actually used, I'm keeping it here to compare its
    // results with that of the other multiplication functions.
    uint16_t result = 0;
    // multiplication
    for (uint8_t i = 0; i < 8; ++i) {
        if (b & 1 << i) {
            result ^= a << i;
        }
    }
    
    // modulo-RP
    for (uint8_t i = 15; i >= 8; --i) {
        if (result & (1 << i)) {
            result ^= static_cast<std::underlying_type<GF256_RP>::type>(RP) << (i - 8);
        }
    }
    
    return result;
}


template<GF256_RP RP>
uint8_t GF256<RP>::mulPeasant(uint8_t a, uint8_t b) {
    uint8_t result = 0;
    
    for (int i = 0; i <= 8; ++i) {
        result ^= -(b & 1) & a;
        uint8_t mask = -((a >> 7) & 1);
        a = (a << 1) ^ (static_cast<std::underlying_type<GF256_RP>::type>(RP) & mask);
        b >>= 1;
    }
    
    return result;
}


template<GF256_RP RP>
uint8_t GF256<RP>::mulLookup(uint8_t a, uint8_t b) {
    uint8_t mask = a | b ? 0xFF : 0x00;
    uint16_t a_powerOf2 = _alphaTables.log[a];
    uint16_t b_powerOf2 = _alphaTables.log[b];
    uint8_t c = (a_powerOf2 + b_powerOf2) % 255;
    uint8_t result = static_cast<std::underlying_type<GF256_RP>::type>(_alphaTables.pow[c]) & mask;
    return result;
}


template <GF256_RP RP>
const typename GF256<RP>::AlphaTables GF256<RP>::_alphaTables{GF256<RP>::generateAlphaTables()};

#endif // GF_H
