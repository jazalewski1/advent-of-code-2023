parse_day_id(INPUT_STR "${CMAKE_CURRENT_SOURCE_DIR}" RETVALUE DAY_ID)

set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/day${DAY_ID}"
)

add_task(
    DAY_ID "${DAY_ID}"
    PART_ID "1"
    SOURCES
        "src/Part1.cpp"
        "src/Task.cpp"
)
add_task(
    DAY_ID "${DAY_ID}"
    PART_ID "2"
    SOURCES
        "src/Part2.cpp"
        "src/Task.cpp"
)
