parse_day_id(INPUT_STR "${CMAKE_CURRENT_SOURCE_DIR}" RETVALUE DAY_ID)
add_task(DAY_ID "${DAY_ID}" PART_ID "1" SOURCE_PATH "src/Part1.cpp")
add_task(DAY_ID "${DAY_ID}" PART_ID "2" SOURCE_PATH "src/Part2.cpp")
