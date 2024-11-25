#ifndef ERROR_H
#define ERROR_H

#include <string_view>

auto reportError(std::string_view error, std::string_view file = "",
                 std::size_t line = -1) -> void;

#endif // !ERROR_H
