cmake_minimum_required(VERSION 3.26)
project(vortex-lang)


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
