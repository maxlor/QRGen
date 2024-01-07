// Copyright 2024 Benjamin Lutz.
// 
// This file is part of QRGen. QRGen is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.

#ifndef UTIL_H
#define UTIL_H

#define caseEnumAsString(enumType, x) case enumType::x: return #x;

#ifndef __cpp_lib_to_underlying
#include <type_traits>
namespace std {
template< class Enum >
constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
    return static_cast<std::underlying_type_t<Enum>>(e);
}
}
#endif

#endif // UTIL_H
