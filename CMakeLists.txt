cmake_minimum_required(VERSION 3.18)
project(ItchParser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(itchcore STATIC
    Classes/ItchParser.cpp
    Classes/VWAPCalc.cpp
)

target_include_directories(itchcore
	PUBLIC
	    ${CMAKE_CURRENT_SOURCE_DIR}/Classes
	    ${CMAKE_CURRENT_SOURCE_DIR}/Messages
)

add_executable(itch_parser
	main.cpp
)

target_link_libraries(itch_parser
    PRIVATE itchcore
)

option(ITCH_BUILD_TESTS "Build GoogleTest unit tests" ON)

if (ITCH_BUILD_TESTS)
    include(FetchContent)

    # Download gtest 1.14 at configure time and make targets gtest & gtest_main
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    FetchContent_MakeAvailable(googletest)

    file(GLOB TEST_SRC CONFIGURE_DEPENDS tests/*.cpp)

    add_executable(itch_tests ${TEST_SRC})

    target_link_libraries(itch_tests
        PRIVATE
            itchcore
            gtest_main
    )

    # Pass the tiny fixture path as a preprocessor define
    target_compile_definitions(itch_tests
        PRIVATE TEST_FIXTURE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/tests/sample.itch"
    )

    enable_testing()
    add_test(NAME unit COMMAND itch_tests)
endif()



