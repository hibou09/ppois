cmake_minimum_required(VERSION 3.10)
project(lab1 CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(UTPP_LIB UnitTest++ REQUIRED)
find_path(UTPP_INC UnitTest++/UnitTest++.h REQUIRED)

add_library(rectangle_lib rectangle/Rectangle.cpp)
target_include_directories(rectangle_lib PUBLIC rectangle)
add_executable(rectangle_tests rectangle/tests_rectangle.cpp)
target_include_directories(rectangle_tests PRIVATE ${UTPP_INC})
target_link_libraries(rectangle_tests rectangle_lib ${UTPP_LIB})

add_library(markov_lib markov/Alphabet.cpp markov/MarkovRule.cpp
            markov/MarkovProgram.cpp markov/MarkovMachine.cpp)
target_include_directories(markov_lib PUBLIC markov)
add_executable(markov markov/main.cpp)
target_link_libraries(markov markov_lib)
add_executable(markov_tests markov/tests_markov.cpp)
target_include_directories(markov_tests PRIVATE ${UTPP_INC})
target_link_libraries(markov_tests markov_lib ${UTPP_LIB})
