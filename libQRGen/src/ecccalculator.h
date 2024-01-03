// Copyright 2024 Benjamin Lutz.
// 
// This file is part of QRGen. QRGen is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.

#ifndef ECCCALCULATOR_H
#define ECCCALCULATOR_H

#include <cstdint>
#include <vector>
#include "gf.h"


/**
 * Calculates the error correction codewords as per Section 7.5.2 of
 * ISO/IEC 18004:2015.
 */
class ECCCalculator {
public:
    ECCCalculator(size_t eccCount);
    
    void reset();
    void feed(uint8_t value);
    std::vector<uint8_t> errorCodeWords() const;
    
    template <typename It>
    static std::vector<uint8_t> feed(It begin, It end, size_t eccCount);
    
private:
    using GFQR = GF256<GF256_RP::QR>;
    
    static const std::vector<uint8_t> &generatorPolynomial(size_t degree);
    static void generatorPolynomialCache();
    static std::vector<uint8_t> calculateGeneratorPolynomial(size_t degree);

    std::vector<GFQR::Element> _b;
    std::vector<GFQR::Element> _g;
};


template<typename It>
std::vector<uint8_t> ECCCalculator::feed(It begin, It end, size_t eccCount) {
    ECCCalculator eccc{eccCount};
    for (It it = begin; it != end; ++it) { eccc.feed(*it); }
    return eccc.errorCodeWords();
}

#endif // ECCCALCULATOR_H
