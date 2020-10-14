#pragma once

#include <array>
#include <numeric>  // std::lcm
#include <type_traits>

namespace pt_math {

// Поиск lcm для массива знаковых элементов
template <typename T,
          typename std::enable_if<!std::is_unsigned_v<T>>::type* = nullptr,
          size_t N>
constexpr uint32_t lcm(const std::array<T, N> arr) {
    uint32_t ans = abs(arr[0]);

    for (uint32_t i = 1; i < N; i++) {
        ans = std::lcm(ans, abs(arr[i]));
    }

    return ans;
}

// Поиск lcm для массива беззнаковых элементов
template <typename T,
          typename std::enable_if<std::is_unsigned_v<T>>::type* = nullptr,
          size_t N>
constexpr uint32_t lcm(const std::array<T, N> arr) {
    uint32_t ans = arr[0];

    for (uint32_t i = 1; i < N; i++) {
        ans = std::lcm(ans, arr[i]);
    }

    return ans;
}

}  // namespace pt_math
