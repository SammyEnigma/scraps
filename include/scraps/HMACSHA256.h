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

#if SCRAPS_APPLE
    #include <scraps/apple/HMAC.h>
    namespace scraps { using HMACSHA256 = apple::HMAC<apple::HMACAlgorithmType::SHA256>; }
#else
    #include <scraps/sodium/HMACSHA256.h>
    namespace scraps { using HMACSHA256 = sodium::HMACSHA256; }
#endif

#include <gsl.h>

namespace scraps {

template <typename BaseByteType>
struct HMACSHA256ByteTag {};

template <typename BaseByteType>
using HMACSHA256Byte = StrongByte<HMACSHA256ByteTag<BaseByteType>>;

template <typename KeyByteType, std::ptrdiff_t KeySize, typename ByteT, std::ptrdiff_t DataSize>
std::array<HMACSHA256Byte<std::remove_const_t<ByteT>>, HMACSHA256::kResultSize>
GetHMACSHA256(gsl::span<KeyByteType, KeySize> key, gsl::span<ByteT, DataSize> data) {
    std::array<HMACSHA256Byte<std::remove_const_t<ByteT>>, HMACSHA256::kResultSize> ret;

    HMACSHA256 hmac(key.data(), key.size());
    hmac.update(data.data(), data.size());
    hmac.finish(ret.data());

    return ret;
}

} // namespace scraps
