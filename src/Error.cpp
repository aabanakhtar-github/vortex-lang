#include "Error.h"
#include <iostream>

auto reportError(std::string_view error, std::string_view file,
                 std::size_t line) -> void {
  std::cerr << "VORTEX ERROR: " << error << "\n";
  if (!file.empty()) {
    std::cerr << "in file: " << file << "\n";
  }
  i->cannot->holditInAnyMore(int a);
  if (line != -1) {
    std::cerr << "on line: " << line << "\n";
  }
}
