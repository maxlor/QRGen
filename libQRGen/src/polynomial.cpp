// Copyright 2024 Benjamin Lutz.
// 
// This file is part of QRGen. QRGen is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.

#include "polynomial.h"
#include <cassert>
#include <cstdlib>


Polynomial::Polynomial(uint32_t polynomial) : _p(polynomial) {}


Polynomial Polynomial::operator+(const Polynomial &other) const {
    return Polynomial{_p ^ other._p};
}


Polynomial Polynomial::operator-(const Polynomial &other) const {
    return *this + other;
}


Polynomial Polynomial::operator*(const Polynomial &other) const {
    unsigned int result = 0;
    unsigned int a = this->_p;
    unsigned int b = other._p;
    for (size_t i = 0; i <= sizeof(typeof(_p)) * 8; ++i) {
        result ^= -(b & 1) & a;
        a <<= 1;
        b >>= 1;
    }
    return Polynomial{result};
}


Polynomial Polynomial::operator%(const Polynomial &other) const {
    unsigned int a = this->_p;
    unsigned int b = other._p;

    const int highestDivisorBit = other.highestBit();
    if (highestDivisorBit == -1) {
        assert(false);
        return Polynomial{0};
    }

    for (int i = 8 * sizeof(decltype(_p)) - 1; i >= highestDivisorBit; --i) {
        if ((a & (1 << i)) != 0) {
            a ^= b << (i - highestDivisorBit);
        }
    }

    return Polynomial{a};
}


bool Polynomial::operator==(const Polynomial &other) const {
    return _p == other._p;
}


uint32_t Polynomial::value() const {
    return _p;
}


int Polynomial::highestBit() const {
    int i;
    for (i = 0; (1 << i) != 0; ++i) {
        if (_p < (1 << i)) {
            return i - 1;
        }
    }
    return i - 1;
}
