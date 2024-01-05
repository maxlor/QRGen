#ifndef QR_H
#define QR_H

#include <cstdint>

namespace QR {

enum class Mode : uint8_t { automatic = 16, eci = 7, numeric = 1, alphanumeric = 2,
                            eightbit = 4, kanji = 8, structuredAppend = 3,
                            fnc1_first = 5, fnc1_second = 9, terminator = 0 };

enum class ErrorCorrection : uint8_t { L = 0, M = 1, Q = 2, H = 3 };

} // namespace QR

#endif // QR_H
