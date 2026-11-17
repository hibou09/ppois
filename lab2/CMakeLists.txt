cmake_minimum_required(VERSION 3.10)
project(lab2_oceanarium CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_library(UTPP_LIB UnitTest++ REQUIRED)
find_path(UTPP_INC UnitTest++/UnitTest++.h REQUIRED)

add_library(oceanarium_lib
    src/Common.cpp
    src/Water.cpp
    src/Food.cpp
    src/Health.cpp
    src/Animals.cpp
    src/Feeding.cpp
    src/Tanks.cpp
    src/Visitors.cpp
    src/Staff.cpp
    src/Management.cpp
)
target_include_directories(oceanarium_lib PUBLIC include)

add_executable(oceanarium src/main.cpp)
target_link_libraries(oceanarium oceanarium_lib)

add_executable(oceanarium_tests
    tests/main_tests.cpp
    tests/test_common.cpp
    tests/test_water.cpp
    tests/test_animals.cpp
    tests/test_health_food.cpp
    tests/test_tanks.cpp
    tests/test_visitors.cpp
    tests/test_staff.cpp
    tests/test_management.cpp
    tests/test_edge_cases.cpp
)
target_include_directories(oceanarium_tests PRIVATE ${UTPP_INC} tests)
target_link_libraries(oceanarium_tests oceanarium_lib ${UTPP_LIB})

enable_testing()
add_test(NAME oceanarium_tests COMMAND oceanarium_tests)
