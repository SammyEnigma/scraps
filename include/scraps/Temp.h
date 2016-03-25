#pragma once

#include "scraps/config.h"

namespace scraps {

/**
 * Utility to construct objects with rvalues where normally prohibited.
 *
 * In some cases, when an object disallows construction from rvalues, it is
 * because references to the input arguments are stored by the object and to
 * prevent dangling references/pointers, the rvalue constructor is deleted.
 *
 * In cases where a function argument has a known lifetime, it may be desirable
 * to allow rvalues for the function parameters. For such situations, the Temp
 * class can be used to construct an object of the templated type using rvalues
 * and lvalues. Internally, it works by forwarding all arguments through a
 * reference-to-lvalue-reference conversion function.
 *
 * In generally, usage of this class should be highly limited.
 *
 * Example:
 *
 * std::array<int, 5> GetArray();
 * void UseSpan(gsl::span<int> span);
 * void UseTempSpan(TempSpan<int> span);
 *
 * UseSpan(GetArray());     // using deleted constructor for gsl::span
 * UseTempSpan(GetArray()); // ok
 */
template <typename T>
class Temp : public T {
public:
    template <typename... Args>
    constexpr Temp(Args&&... args)
        : T{_asLValue(args)...}
    {}

private:
    template <typename V>
    constexpr V& _asLValue(V&& v) {
        return v;
    }
};

} // namespace scraps
