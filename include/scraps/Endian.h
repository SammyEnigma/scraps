/**
* Copyright 2016 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#pragma once

#include <scraps/config.h>

#include <type_traits>

namespace scraps {

#if __clang__ || __GNUC__

enum class Endian
{
    kNative = __BYTE_ORDER__,
    kLittle = __ORDER_LITTLE_ENDIAN__,
    kBig    = __ORDER_BIG_ENDIAN__
};

constexpr uint8_t  SwapByteOrder(uint8_t  x) noexcept { return x; }
constexpr int8_t   SwapByteOrder(int8_t   x) noexcept { return x; }
constexpr uint16_t SwapByteOrder(uint16_t x) noexcept { return __builtin_bswap16(x); }
constexpr int16_t  SwapByteOrder(int16_t  x) noexcept { return static_cast<int16_t>(SwapByteOrder(static_cast<uint16_t>(x))); }
constexpr uint32_t SwapByteOrder(uint32_t x) noexcept { return __builtin_bswap32(x); }
constexpr int32_t  SwapByteOrder(int32_t  x) noexcept { return static_cast<int32_t>(SwapByteOrder(static_cast<uint32_t>(x))); }
constexpr uint64_t SwapByteOrder(uint64_t x) noexcept { return __builtin_bswap64(x); }
constexpr int64_t  SwapByteOrder(int64_t  x) noexcept { return static_cast<int64_t>(SwapByteOrder(static_cast<uint64_t>(x))); }

#endif

namespace detail {
    template <typename T>
    constexpr T MaybeSwapByteOrder(T x, std::false_type) noexcept { return x; }

    template <typename T>
    constexpr T MaybeSwapByteOrder(T x, std::true_type) noexcept { return SwapByteOrder(x); }
}

template <typename T>
constexpr T NativeToLittleEndian(T x) noexcept {
    return detail::MaybeSwapByteOrder(x, std::integral_constant<bool, Endian::kNative != Endian::kLittle>{});
}

template <typename T>
constexpr T NativeToBigEndian(T x) noexcept {
    return detail::MaybeSwapByteOrder(x, std::integral_constant<bool, Endian::kNative != Endian::kBig>{});
}

template <typename T>
constexpr T LittleToNativeEndian(T x) noexcept {
    return detail::MaybeSwapByteOrder(x, std::integral_constant<bool, Endian::kLittle != Endian::kNative>{});
}

template <typename T>
constexpr T BigToNativeEndian(T x) noexcept {
    return detail::MaybeSwapByteOrder(x, std::integral_constant<bool, Endian::kBig != Endian::kNative>{});
}

template <typename T>
constexpr T BigToLittleEndian(T x) noexcept {
    return SwapByteOrder(x);
}

template <typename T>
constexpr T LittleToBigEndian(T x) noexcept {
    return SwapByteOrder(x);
}

} // namespace scraps
