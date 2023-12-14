set(
    HEADERS
    include/utility/Arguments.hpp
    include/utility/Dbg.hpp
    include/utility/Stream.hpp
)

set(
    SOURCES
    src/Dbg.cpp
    src/Arguments.cpp
    src/Stream.cpp
)

add_library(
    utility
    "${HEADERS}"
    "${SOURCES}"
)

target_include_directories(
    utility
    PUBLIC include
    PRIVATE src
)

set_target_properties(
	utility
	PROPERTIES
	LINKER_LANGUAGE CXX
)
