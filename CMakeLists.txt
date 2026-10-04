cmake_minimum_required(VERSION 3.25)
project(libspec LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(spec_data ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(spec_generated ${CMAKE_CURRENT_BINARY_DIR}/gen)
set(spec_tables ${spec_generated}/species_names.c)
add_custom_command(
    OUTPUT ${spec_tables}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_spec_tables.py
            ${spec_data} ${spec_generated}
    DEPENDS tools/generate_spec_tables.py ${spec_data}/species_names.tsv
    VERBATIM)

set(gba_data ${CMAKE_CURRENT_SOURCE_DIR}/data/gba)
set(gba_generated ${CMAKE_CURRENT_BINARY_DIR}/gba/gen)
set(gba_tables
    ${gba_generated}/species_national.c
    ${gba_generated}/species_data.c
    ${gba_generated}/experience.c
    ${gba_generated}/charmap.c
    ${gba_generated}/items.c)
add_custom_command(
    OUTPUT ${gba_tables}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_gba_tables.py
            ${gba_data} ${gba_generated}
    DEPENDS tools/generate_gba_tables.py ${gba_data}/species.tsv ${gba_data}/charmap.tsv
            ${gba_data}/items.tsv
    VERBATIM)

add_library(spec STATIC
    src/spec.c
    src/names.c
    src/bytes.c
    src/utf8.c
    src/gba/save.c
    src/gba/slot.c
    src/gba/pokemon.c
    src/gba/personality.c
    src/gba/items.c
    src/gba/text.c
    ${spec_tables}
    ${gba_tables})
target_include_directories(spec PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include/spec>)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(spec PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

install(TARGETS spec ARCHIVE DESTINATION lib)
install(FILES src/spec.h DESTINATION include/spec)
install(FILES src/gba/gba.h DESTINATION include/spec/gba)

if(SPEC_GBA_TEST_SAVES)
    enable_testing()
    add_executable(gba_round_trip tests/gba_round_trip.c)
    target_link_libraries(gba_round_trip PRIVATE spec)
    file(GLOB test_saves ${SPEC_GBA_TEST_SAVES})
    foreach(test_save IN LISTS test_saves)
        get_filename_component(test_name ${test_save} NAME_WE)
        add_test(NAME gba_round_trip_${test_name} COMMAND gba_round_trip ${test_save})
    endforeach()
endif()
