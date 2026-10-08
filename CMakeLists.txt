cmake_minimum_required(VERSION 3.10)
project(wordBuilder)

set(CMAKE_CXX_STANDARD 23)

# 1. Add raylib
add_subdirectory(libs/raylib)

# 2. Find all .cpp files recursively
# RECURSE ensures it finds files in includes/Entity/ and includes/Game/
file(GLOB_RECURSE MY_SRC "${CMAKE_SOURCE_DIR}/includes/*.cpp")
# main.cpp is listed explicitly: a recursive glob on "main.cpp" would also pick up the
# example programs shipped inside the libs/ submodules.
list(APPEND MY_SRC "${CMAKE_SOURCE_DIR}/main.cpp")

add_executable(wordBuilder ${MY_SRC})

# 3. Link libraries
target_link_libraries(wordBuilder PUBLIC raylib)

# 4. Include directories (The most important part)
target_include_directories(wordBuilder PUBLIC
    ${CMAKE_SOURCE_DIR}/libs/rlTextLib/rlText
    ${CMAKE_SOURCE_DIR}/libs/json
    ${CMAKE_SOURCE_DIR}/includes
    ${CMAKE_SOURCE_DIR}/libs/raylib-cpp/include
)

# 5. Copy the level files next to the executable (the game loads them from its working directory)
file(COPY ${CMAKE_SOURCE_DIR}/levels ${CMAKE_SOURCE_DIR}/levels.json DESTINATION ${CMAKE_BINARY_DIR})
