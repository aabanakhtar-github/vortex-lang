#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include <tuple>

struct None {};
static constexpr auto NONE = None{};

inline auto sizeToTriByte(std::size_t i)
    -> std::tuple<std::uint8_t, std::uint8_t, std::uint8_t> {
  auto b1 = static_cast<std::uint8_t>(i >> 16);
  auto b2 = static_cast<std::uint8_t>(i >> 8);
  auto b3 = static_cast<std::uint8_t>(i);
  return {b1, b2, b3};
}

#endif //! UTIL_H
