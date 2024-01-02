#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <cstdint>


/**
 * Polynomials as used in Bose-Chaudhuri-Hocquenghem codes.
 * 
 * These polynomials are of the form:
 *  
 *      a⁰x⁰ + a¹x¹ + ... + aⁿxⁿ
 *      
 * Where all the coefficients (a-values) are either 0 or 1. The maximum order
 * that is supported is 31.
 */
class Polynomial {
public:
    /**
     * Construct a polynomial using the bits of \a polynomial as a-values.
     * The least significant bit of \a polynomial corresponds to a⁰, and the
     * most significant bit of \a polynomial corresponds to a³¹.
     */
    Polynomial(uint32_t polynomial = 0);

    Polynomial operator+(const Polynomial &other) const;
    Polynomial operator-(const Polynomial &other) const;
    Polynomial operator*(const Polynomial &other) const;
    Polynomial operator%(const Polynomial &other) const;
    bool operator==(const Polynomial &other) const;
    
    /**
     * Returns the a-values in the same format as used for the
     * argument in ::Polynomial(unsigned int).
     */
    uint32_t value() const;

private:
    int highestBit() const;
    
    uint32_t _p;
};

#endif // POLYNOMIAL_H
