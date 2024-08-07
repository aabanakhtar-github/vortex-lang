cmake_minimum_required(VERSION 3.26)
project(vlc)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQURIED True)

add_subdirectory(vvm)

set(VORTEX_SOURCES 
  src/main.cpp
  src/Token.cpp 
  src/Lexer.cpp 
  src/Error.cpp
  src/Parser.cpp
)
add_executable(${PROJECT_NAME} 
  ${VORTEX_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE vvm/src)
target_link_libraries(${PROJECT_NAME} PRIVATE libvvm)
# build tests (maybe)

