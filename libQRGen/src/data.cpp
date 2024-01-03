#include "data.h"
#include <algorithm>
#include <cassert>

using namespace std;

Data::Data() : _bitCount{0} {}


Data::Data(std::initializer_list<T> list) : _d{list}, _bitCount{list.size() * TBitSize} {}


void Data::append(size_t bits, uint32_t value) {
    static constexpr size_t valueBitSize = 8 * sizeof(value);
    
    if (bits > valueBitSize) {
        const size_t numZeros = bits - sizeof(value) * 8;
        appendZeros(numZeros);
        bits -= numZeros;
    }
    assert(bits <= valueBitSize);
    
    if (bits < valueBitSize) {
        const uint32_t mask = (uint32_t{1} << bits) - uint32_t{1};
        value &= mask;
    }
    
    if ((_bitCount & TBitMask) != 0) {
        const size_t used = _bitCount & TBitMask;
        const size_t remaining = TBitSize - used;
        const size_t bitsToInsert = min(remaining, bits);
        const uint32_t valueBits = value >> (bits - bitsToInsert);
        _d.back() |= valueBits << (remaining - bitsToInsert);
        bits -= bitsToInsert;
        _bitCount += bitsToInsert;
    }
    
    assert((_bitCount & TBitMask) == 0 || bits == 0);
    
    while (bits >= TBitSize) {
        _d.push_back(value >> (bits - TBitSize));
        bits -= TBitSize;
        _bitCount += TBitSize;
    }
    
    if (bits > 0) {
        _d.push_back(value << (TBitSize - bits));
        _bitCount += bits;
    }
}


void Data::append(const Data &other) {
    // save the values, so if a Data is appended to itself, they don't change
    // during this function.
    const size_t otherSize = other.size();
    const size_t otherBitCount = other.bitCount();
    
    const size_t otherWholeElements = otherBitCount / TBitSize;
    for (size_t i = 0; i < otherWholeElements; ++i) {
        append(TBitSize, other.at(i));
    }
    
    const size_t remaining = otherBitCount & TBitMask;
    if (remaining > 0) {
        append(remaining, other.at(otherSize - 1) >> (TBitSize - remaining));
    }
}


void Data::clear() {
    _d.clear();
    _bitCount = 0;
}


void Data::padLastByte() {
    const size_t zeroCount = TBitSize - (_bitCount & TBitMask);
    if (zeroCount != 0) {
        appendZeros(zeroCount);
    }
}


bool Data::operator==(const Data &other) const {
    return _bitCount == other._bitCount && _d == other._d;
}


void Data::appendZeros(std::size_t count) {
    _bitCount += count;
    const size_t newSize = (_bitCount + TBitSize - 1) / TBitSize;
    _d.resize(newSize);
}
