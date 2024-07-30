cmake_minimum_required(VERSION 3.26)
project(vortex-lang)
Include(FetchContent)

# import Catch2
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)
FetchContent_MakeAvailable(Catch2)


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQURIED True)

set(VORTEX_SOURCES 
  src/main.cpp
  src/Token.cpp 
  src/Lexer.cpp 
  src/Error.cpp 
)

add_executable(${PROJECT_NAME} 
  ${VORTEX_SOURCES})
# build tests

