#ifndef DATA_H
#define DATA_H

#include <cstdint>
#include <initializer_list>
#include <vector>


/**
 * A container class to which bits can be appended, but which can also work with
 * byte-wise access.
 */
class Data {
public:
    using T = uint8_t;
    
    Data(); ///< Creates an empty data object.
    
    /** Creates a data object filled with the elements of \a list. */
    Data(std::initializer_list<T> list);
    
    const std::vector<T> &data() const; ///< The bits collected into units of type T.
    T at(std::size_t i) const; ///< Access the i-th data element (as T, not bits).
    
    std::size_t bitCount() const; ///< The number of bits stored.
    std::size_t size() const; ///< The number of stored T elements.
    
    /**
     * Append the \a bits least significant bits of \a value, high bit first.
     * If \a bits is larger than 32, zero-bits are appended before \a value,
     * i.e. \a value is zero-extended to \a bits bits
     * 
     * @param bits   the number of bits to append
     * @param value  the value containing the bits
     */
    void append(size_t bits, uint32_t value);
    void append(const Data &other); ///< Append the contens of another data object.
    
    void clear(); ///< Remove all data from this Data object, setting size and bitCount to 0.
    
    void padLastByte(); ///< Add zeroes to fill up the last byte.
    
    bool operator==(const Data &other) const;
    
private:
    void appendZeros(std::size_t count);
    
    static constexpr size_t TBitSize = 8 * sizeof(T);
    static constexpr size_t TBitMask = TBitSize - 1;
    
    // bit order: high bit of low byte comes first, low bit of high byte last
    std::vector<T> _d;
    std::size_t _bitCount;
};


inline const std::vector<Data::T> &Data::data() const { return _d; }
inline Data::T Data::at(std::size_t i) const { return _d.at(i); }
inline std::size_t Data::bitCount() const { return _bitCount; }
inline std::size_t Data::size() const { return _d.size(); }

#endif // DATA_H
