cmake_minimum_required(VERSION 3.28)
project(chatty)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} src/main.cpp)

find_package(Boost)
find_package(Threads)
find_package(fmt REQUIRED)

target_sources(${PROJECT_NAME} PRIVATE
    src/peer.cpp
)

target_link_libraries(${PROJECT_NAME} Boost::boost Threads::Threads fmt::fmt)
