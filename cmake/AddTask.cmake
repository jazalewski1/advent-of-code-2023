function(parse_day_id)
    cmake_parse_arguments("ARG" "" "INPUT_STR;RETVALUE" "" ${ARGN})
    string(REGEX MATCH "day[0-9]+" DAY_NAME ${ARG_INPUT_STR})
    string(REGEX MATCH "[0-9]+" ID ${DAY_NAME})
    set(${ARG_RETVALUE} ${ID} PARENT_SCOPE)
endfunction(parse_day_id)

function(add_task)
    cmake_parse_arguments(
        "ARG"
        ""
        "DAY_ID;PART_ID"
        "INCLUDES;SOURCES"
        ${ARGN}
    )

    set(
        CMAKE_RUNTIME_OUTPUT_DIRECTORY 
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/day${ARG_DAY_ID}"
    )

    set(TARGET_NAME "d${ARG_DAY_ID}p${ARG_PART_ID}")
    add_executable("${TARGET_NAME}" "${ARG_SOURCES}")
    set(EXE_NAME "part${ARG_PART_ID}")
    set_target_properties("${TARGET_NAME}" PROPERTIES OUTPUT_NAME "${EXE_NAME}")

    target_include_directories(
        "${TARGET_NAME}"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

    target_link_libraries(
        "${TARGET_NAME}"
        "utility"
    )
endfunction(add_task)
