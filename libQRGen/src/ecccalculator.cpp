#include "ecccalculator.h"
#include <algorithm>
#include <bit>
#include <iostream>
#include <iterator>
#include <map>

using namespace std;


ECCCalculator::ECCCalculator(size_t eccCount)
    : _b(eccCount, GFQR::zero()), _g(eccCount) {
    const vector<uint8_t> &gp = generatorPolynomial(eccCount);
    transform(gp.begin(), gp.end(), _g.begin(), &GFQR::alpha);
}


void ECCCalculator::reset() {
    fill(_b.begin(), _b.end(), GFQR::zero());
}


void ECCCalculator::feed(uint8_t value) {
    const GFQR::Element lastRegister = _b[0];
    const GFQR::Element polyInput = lastRegister + GFQR::Element{value};
    for (size_t i = _b.size(); i-- > 0;) {
        GFQR::Element x = polyInput * _g[i];
        _b[_b.size() - i - 1] = x + (i > 0 ? _b[_b.size() - i] : GFQR::zero());
    }
}


vector<uint8_t> ECCCalculator::errorCodeWords() const {
    vector<uint8_t> result;
    copy(_b.begin(), _b.end(), back_inserter(result));
    return result;
}


const vector<uint8_t> &ECCCalculator::generatorPolynomial(size_t degree) {
    // Precalculated polynomials, since calculating these may take some time,
    // especially for the higher degrees, which can take minutes.
    // Compare with Annex A of ISO/IEC 18004:2015.
    // These values are powers of alpha.
    // 
    // Note: it would be more efficient to store GF-elements rather than powers
    // of alpha, since only GF-elements are actually used in calculations.
    static map<size_t, vector<uint8_t>> polynomials = {
        { 16, { 120, 225, 194, 182, 169, 147, 191, 91, 3, 76, 161, 102, 109, 107, 104, 120, }},
        { 17, { 136, 163, 243, 39, 150, 99, 24, 147, 214, 206, 123, 239, 43, 78, 206, 139, 43, }},
        { 22, { 231, 165, 105, 160, 134, 219, 80, 98, 172, 8, 74, 200, 53, 221, 109, 14, 230, 93, 242, 247, 171, 210, }},
        { 24, { 20, 156, 215, 248, 132, 200, 241, 111, 218, 228, 226, 192, 152, 169, 180, 159, 126, 251, 117, 211, 48, 135, 121, 229, }},
        { 26, { 69, 217, 144, 40, 31, 171, 183, 15, 20, 96, 243, 201, 166, 165, 28, 111, 201, 145, 17, 118, 182, 103, 2, 158, 125, 173, }},
        { 28, { 122, 8, 36, 241, 118, 165, 25, 42, 244, 106, 133, 199, 89, 14, 125, 183, 47, 195, 190, 110, 180, 108, 234, 224, 104, 200, 223, 168, }},
        { 30, { 179, 191, 39, 237, 215, 250, 36, 208, 182, 90, 152, 62, 216, 152, 175, 159, 1, 129, 178, 46, 48, 50, 182, 179, 31, 216, 152, 145, 173, 41, }},
    };
    
    if (polynomials.find(degree) == polynomials.end()) {
        polynomials.insert({degree, calculateGeneratorPolynomial(degree)});
    }
    
    return polynomials[degree];
}


/**
 * Generate the code for the cached polynomials in generatorPolynomial(size_t).
 */
void ECCCalculator::generatorPolynomialCache() {
    cout << "static map<size_t, vector<uint8_t>> polynomials = {" << endl;
    
    for (size_t degree : { 16, 17, 22, 24, 26, 28, 30 }) {
        cout << "\t{ " << degree << ", { ";
        
        const vector<uint8_t> gp = generatorPolynomial(degree);
        for (uint8_t x : gp) {
            cout << unsigned{x} << ", ";
        }
        
        cout << "}}," << endl;
    }
    
    cout << "};" << endl;
}


/**
 * Calculate the product of the first degree polynomials:
 * 
 *     x - α⁰, x - α¹, ..., x - αⁿ⁻¹
 * 
 * where n is the degree.
 * 
 * The runtime of this function scales exponentially with the degree, so at
 * degree 20 or above, it starts becoming slow. Calculating the polynomial
 * with degree 30 takes several minutes at the time of writing.
 */
vector<uint8_t> ECCCalculator::calculateGeneratorPolynomial(size_t degree) {
    vector<GFQR::Element> polynomial(degree);
    
    for (size_t i = 0; i < (1U << degree); ++i) {
        // Algorithm: for each polynomial, the product first degree
        // polynomials is expanded into a sum of products:
        // 
        //   (x - α²)(x - α¹)(x - α⁰)                                   (A)
        // = xxx-xxα⁰ - xα¹x + xα¹α⁰ - α²xx + α²xα⁰ + α²α¹x - α²α¹α⁰    (B)
        //   000 001    010    011    100     101     110     111       (C)
        //
        // So the summands of (B) can be enumerated with a counter i (C),
        // where each bit of selects a summand of each term in (A). If the
        // bit is 0, the x is chosen, otherwise an α is chosen.
        //
        // The degree of each summand of (B) is determined by the number
        // of 0s in i, this is saved as k.
        //
        // Since all coefficients are powers of α, We can sum up the
        // exponents.
        const size_t k = degree - popcount(i);
        uint8_t exponent = 0;
        for (size_t j = 0; j < degree; ++j) {
            if (i & (1 << j)) {
                exponent += j;
            }
        }
        polynomial[k] = polynomial[k] + GFQR::alpha(exponent);
    }
    
    // Finally convert the results to α-exponents.
    vector<uint8_t> result;
    result.reserve(polynomial.size());
    transform(polynomial.begin(), polynomial.end(), back_inserter(result), &GFQR::logAlpha);
    return result;
}
