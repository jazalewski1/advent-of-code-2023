cmake_minimum_required(VERSION 3.17)

project(aoc23 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

include(FetchContent)

FetchContent_Declare(
	googletest
	GIT_REPOSITORY "https://github.com/google/googletest.git"
	GIT_TAG e2239ee6043f73722e7aa812a459f54a28552929 # release 1.11.0, latest master caused compilation warnings
)
FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(${CMAKE_SOURCE_DIR}/cmake/AddTask.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/AddTestModule.cmake)

add_subdirectory(utility)

foreach(DAY_ID RANGE 25)
    if ("${DAY_ID}" LESS 10)
        set(DAY_ID "0${DAY_ID}")
    endif()
	set(DAY_NAME "day${DAY_ID}")
	set(DAY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${DAY_NAME}")
    if (EXISTS "${DAY_PATH}" AND EXISTS "${DAY_PATH}/CMakeLists.txt")
		add_subdirectory("${DAY_NAME}")
	endif()
endforeach()
