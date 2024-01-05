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
    uint32_t result = 0;
    uint32_t a = this->_p;
    uint32_t b = other._p;
    for (size_t i = 0; i <= sizeof(typeof(_p)) * 8; ++i) {
        result ^= -(b & 1) & a;
        a <<= 1;
        b >>= 1;
    }
    return Polynomial{result};
}


Polynomial Polynomial::operator%(const Polynomial &other) const {
    if (other._p == 0) {
        assert(false);
        return Polynomial{0};
    }
    
    uint32_t a = this->_p;
    const uint32_t &b = other._p;
    
    const int divisorOrder = other.order();

    for (int i = 31; i >= divisorOrder; --i) {
        if ((a & (uint32_t{1} << i)) != 0) {
            a ^= b << (i - divisorOrder);
        }
    }

    return Polynomial{a};
}


bool Polynomial::operator==(const Polynomial &other) const {
    return _p == other._p;
}


unsigned int Polynomial::order() const {
    for (unsigned int i = 1; i < 32; ++i) {
        if (_p < (uint32_t{1} << i)) {
            return i - 1;
        }
    }
    return 31;
}


uint32_t Polynomial::value() const {
    return _p;
}
