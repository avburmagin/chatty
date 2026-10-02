cmake_minimum_required(VERSION 3.28)
project(chatty)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_executable(${PROJECT_NAME} main.cpp)

find_package(Boost)
find_package(Threads)

#target_link_libraries(${PROJECT_NAME} Boost::boost Threads::Threads)

add_subdirectory(core)
add_subdirectory(main)
