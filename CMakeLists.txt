cmake_minimum_required(VERSION 3.26)
project(vlc)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQURIED True)

enable_testing()

# include google test
include(FetchContent)
FetchContent_Declare(
  googletest 
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) 
FetchContent_MakeAvailable(googletest)

add_subdirectory(vvm)
# TODO: move vvm to vendor 


set(VORTEX_SOURCES 
  src/Token.cpp 
  src/Lexer.cpp 
  src/Error.cpp
  src/Parser.cpp
  src/PrettyPrintExpressionVisitor.cpp
  src/CodeGenVisitor.cpp
)

# tests 
set(TEST_SOURCES
  tests/LexerTests.cpp
  tests/ParserTests.cpp
  ${VORTEX_SOURCES}
)

add_executable(Tests ${TEST_SOURCES})
target_link_libraries(Tests GTest::gtest_main libvvm)
target_include_directories(Tests PRIVATE src vvm/src) # access the compiler's stuff
include(GoogleTest)
gtest_discover_tests(Tests)

# main executable
add_executable(${PROJECT_NAME} 
  ${VORTEX_SOURCES}
  src/main.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE vvm/src)
target_link_libraries(${PROJECT_NAME} PRIVATE libvvm)
# build tests (maybe)

