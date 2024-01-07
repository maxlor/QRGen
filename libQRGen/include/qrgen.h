// Copyright 2024 Benjamin Lutz.
// 
// This file is part of QRGen. QRGen is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.

#ifndef QRGEN_H
#define QRGEN_H

#include <cstdint>

namespace QRGen {

enum class ErrorCorrection : uint8_t { L = 0, M = 1, Q = 2, H = 3 };

} // namespace QRGen

#endif // QRGEN_H
