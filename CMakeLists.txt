cmake_minimum_required(VERSION 3.10)
project(Seb_HFT_Client_KeboWorking)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(main_engine main_engine.cpp)
    target_link_libraries(main_engine ${Boost_LIBRARIES})
else()
    message(FATAL_ERROR "Boost not found")
endif()

add_executable(Seb_HFT_Client_KeboWorking main_engine.cpp)
