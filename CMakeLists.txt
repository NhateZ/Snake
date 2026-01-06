cmake_minimum_required(VERSION 3.31)
project(Snake LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/pixelFont.ttf" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/bin")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/floor.png" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/bin")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/wall.png" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/bin")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")
    add_executable(main WIN32 src/main.cpp)
else()
    add_executable(main src/main.cpp)
endif()
target_compile_features(main PRIVATE cxx_std_20)
target_link_libraries(main PRIVATE SFML::Graphics)