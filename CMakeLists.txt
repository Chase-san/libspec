cmake_minimum_required(VERSION 3.25)
project(libspec LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(spec_data ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(spec_generated ${CMAKE_CURRENT_BINARY_DIR}/gen)
set(spec_name_kinds species form move ability item nature type)
set(spec_tables ${spec_generated}/experience.c)
set(spec_name_data)
foreach(kind ${spec_name_kinds})
    list(APPEND spec_tables ${spec_generated}/${kind}_names.c)
    list(APPEND spec_name_data ${spec_data}/${kind}_names.tsv)
endforeach()
add_custom_command(
    OUTPUT ${spec_tables}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_spec_tables.py
            ${spec_data} ${spec_generated}
    DEPENDS tools/generate_spec_tables.py ${spec_name_data}
    VERBATIM)

set(gb_data ${CMAKE_CURRENT_SOURCE_DIR}/data/gb)
set(gb_generated ${CMAKE_CURRENT_BINARY_DIR}/gb/gen)
set(gb_tables
    ${gb_generated}/species_national.c
    ${gb_generated}/species_data.c
    ${gb_generated}/charmap.c
    ${gb_generated}/items.c)
add_custom_command(
    OUTPUT ${gb_tables}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_gb_tables.py
            ${gb_data} ${gb_generated}
    DEPENDS tools/generate_gb_tables.py ${gb_data}/species.tsv ${gb_data}/charmap.tsv
            ${gb_data}/items.tsv
    VERBATIM)

set(gbc_data ${CMAKE_CURRENT_SOURCE_DIR}/data/gbc)
set(gbc_generated ${CMAKE_CURRENT_BINARY_DIR}/gbc/gen)
set(gbc_tables
    ${gbc_generated}/species_data.c
    ${gbc_generated}/charmap.c
    ${gbc_generated}/items.c)
add_custom_command(
    OUTPUT ${gbc_tables}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_gbc_tables.py
            ${gbc_data} ${gbc_generated}
    DEPENDS tools/generate_gbc_tables.py tools/generate_gb_tables.py ${gbc_data}/species.tsv
            ${gbc_data}/charmap.tsv ${gbc_data}/items.tsv
    VERBATIM)

set(gba_data ${CMAKE_CURRENT_SOURCE_DIR}/data/gba)
set(gba_generated ${CMAKE_CURRENT_BINARY_DIR}/gba/gen)
set(gba_tables
    ${gba_generated}/species_national.c
    ${gba_generated}/species_data.c
    ${gba_generated}/charmap.c
    ${gba_generated}/items.c)
add_custom_command(
    OUTPUT ${gba_tables}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_gba_tables.py
            ${gba_data} ${gba_generated}
    DEPENDS tools/generate_gba_tables.py ${gba_data}/species.tsv ${gba_data}/charmap.tsv
            ${gba_data}/items.tsv
    VERBATIM)

set(nds_data ${CMAKE_CURRENT_SOURCE_DIR}/data/nds)
set(nds_generated ${CMAKE_CURRENT_BINARY_DIR}/nds/gen)
set(nds_tables
    ${nds_generated}/species_data.c
    ${nds_generated}/charmap.c
    ${nds_generated}/items.c)
add_custom_command(
    OUTPUT ${nds_tables}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_nds_tables.py
            ${nds_data} ${nds_generated}
    DEPENDS tools/generate_nds_tables.py ${nds_data}/species.tsv ${nds_data}/charmap.tsv
            ${nds_data}/items.tsv
    VERBATIM)

set(ndsi_data ${CMAKE_CURRENT_SOURCE_DIR}/data/ndsi)
set(ndsi_generated ${CMAKE_CURRENT_BINARY_DIR}/ndsi/gen)
set(ndsi_tables
    ${ndsi_generated}/species_data.c
    ${ndsi_generated}/items.c)
add_custom_command(
    OUTPUT ${ndsi_tables}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_ndsi_tables.py
            ${ndsi_data} ${ndsi_generated}
    DEPENDS tools/generate_ndsi_tables.py ${ndsi_data}/species.tsv ${ndsi_data}/items.tsv
    VERBATIM)

set(3ds_data ${CMAKE_CURRENT_SOURCE_DIR}/data/3ds)
set(3ds_generated ${CMAKE_CURRENT_BINARY_DIR}/3ds/gen)
set(3ds_tables
    ${3ds_generated}/species_data.c
    ${3ds_generated}/items.c
    ${3ds_generated}/chinese.c)
add_custom_command(
    OUTPUT ${3ds_tables}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_3ds_tables.py
            ${3ds_data} ${3ds_generated}
    DEPENDS tools/generate_3ds_tables.py ${3ds_data}/species.tsv ${3ds_data}/items.tsv
            ${3ds_data}/chinese_glyphs.tsv ${3ds_data}/chinese_species_names.tsv
    VERBATIM)

add_library(spec STATIC
    src/spec.c
    src/names.c
    src/bytes.c
    src/utf8.c
    src/crc16.c
    src/bag.c
    src/personality.c
    src/record.c
    src/stats.c
    src/gb/save.c
    src/gb/layout.c
    src/gb/list.c
    src/gb/checksum.c
    src/gb/player.c
    src/gb/pokedex.c
    src/gb/storage.c
    src/gb/pokemon.c
    src/gb/items.c
    src/gb/text.c
    src/gbc/save.c
    src/gbc/layout.c
    src/gbc/copy.c
    src/gbc/player.c
    src/gbc/pokedex.c
    src/gbc/storage.c
    src/gbc/pokemon.c
    src/gbc/traits.c
    src/gbc/mail.c
    src/gbc/items.c
    src/gbc/text.c
    src/gba/save.c
    src/gba/layout.c
    src/gba/slot.c
    src/gba/player.c
    src/gba/pokedex.c
    src/gba/storage.c
    src/gba/pokemon.c
    src/gba/personality.c
    src/gba/items.c
    src/gba/text.c
    src/nds/save.c
    src/nds/layout.c
    src/nds/block.c
    src/nds/player.c
    src/nds/pokedex.c
    src/nds/storage.c
    src/nds/pokemon.c
    src/nds/mail.c
    src/nds/personality.c
    src/nds/items.c
    src/nds/text.c
    src/ndsi/save.c
    src/ndsi/layout.c
    src/ndsi/copy.c
    src/ndsi/player.c
    src/ndsi/pokedex.c
    src/ndsi/storage.c
    src/ndsi/pokemon.c
    src/ndsi/mail.c
    src/ndsi/personality.c
    src/ndsi/items.c
    src/ndsi/text.c
    src/3ds/save.c
    src/3ds/layout.c
    src/3ds/footer.c
    src/3ds/signature.c
    src/3ds/sha.c
    src/3ds/aes.c
    src/3ds/rsa.c
    src/3ds/player.c
    src/3ds/pokedex.c
    src/3ds/storage.c
    src/3ds/pokemon.c
    src/3ds/personality.c
    src/3ds/items.c
    src/3ds/text.c
    ${spec_tables}
    ${gb_tables}
    ${gbc_tables}
    ${gba_tables}
    ${nds_tables}
    ${ndsi_tables}
    ${3ds_tables})
target_include_directories(spec PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include/spec>)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(spec PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

install(TARGETS spec ARCHIVE DESTINATION lib)
install(FILES src/spec.h DESTINATION include/spec)
install(FILES src/gb/gb.h DESTINATION include/spec/gb)
install(FILES src/gbc/gbc.h DESTINATION include/spec/gbc)
install(FILES src/gba/gba.h DESTINATION include/spec/gba)
install(FILES src/nds/nds.h DESTINATION include/spec/nds)
install(FILES src/ndsi/ndsi.h DESTINATION include/spec/ndsi)
install(FILES src/3ds/3ds.h DESTINATION include/spec/3ds)

enable_testing()
add_executable(names_test tests/names_test.c)
target_link_libraries(names_test PRIVATE spec)
add_test(NAME names_test COMMAND names_test)

if(SPEC_GB_TEST_SAVES)
    enable_testing()
    add_executable(gb_round_trip tests/gb_round_trip.c)
    target_link_libraries(gb_round_trip PRIVATE spec)
    file(GLOB test_saves ${SPEC_GB_TEST_SAVES})
    foreach(test_save IN LISTS test_saves)
        get_filename_component(test_name ${test_save} NAME_WE)
        add_test(NAME gb_round_trip_${test_name} COMMAND gb_round_trip ${test_save})
    endforeach()
endif()

if(SPEC_GBC_TEST_SAVES)
    enable_testing()
    add_executable(gbc_round_trip tests/gbc_round_trip.c)
    target_link_libraries(gbc_round_trip PRIVATE spec)
    file(GLOB test_saves ${SPEC_GBC_TEST_SAVES})
    foreach(test_save IN LISTS test_saves)
        get_filename_component(test_name ${test_save} NAME_WE)
        add_test(NAME gbc_round_trip_${test_name} COMMAND gbc_round_trip ${test_save})
    endforeach()
endif()

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

if(SPEC_NDS_TEST_SAVES)
    enable_testing()
    add_executable(nds_round_trip tests/nds_round_trip.c)
    target_link_libraries(nds_round_trip PRIVATE spec)
    file(GLOB test_saves ${SPEC_NDS_TEST_SAVES})
    foreach(test_save IN LISTS test_saves)
        get_filename_component(test_name ${test_save} NAME_WE)
        add_test(NAME nds_round_trip_${test_name} COMMAND nds_round_trip ${test_save})
    endforeach()
endif()

if(SPEC_NDSI_TEST_SAVES)
    enable_testing()
    add_executable(ndsi_round_trip tests/ndsi_round_trip.c)
    target_link_libraries(ndsi_round_trip PRIVATE spec)
    file(GLOB test_saves ${SPEC_NDSI_TEST_SAVES})
    foreach(test_save IN LISTS test_saves)
        get_filename_component(test_name ${test_save} NAME_WE)
        add_test(NAME ndsi_round_trip_${test_name} COMMAND ndsi_round_trip ${test_save})
    endforeach()
endif()

# Gen 7 writes need the games' signing key, its private exponent in a file of its own; without it
# the Gen 7 saves check reading and the refused writes only.
if(SPEC_3DS_TEST_SAVES)
    enable_testing()
    add_executable(3ds_round_trip tests/3ds_round_trip.c)
    target_link_libraries(3ds_round_trip PRIVATE spec)
    file(GLOB test_saves ${SPEC_3DS_TEST_SAVES})
    foreach(test_save IN LISTS test_saves)
        get_filename_component(test_name ${test_save} NAME_WE)
        add_test(NAME 3ds_round_trip_${test_name}
                 COMMAND 3ds_round_trip ${test_save} ${SPEC_3DS_SIGNING_KEY})
    endforeach()
endif()
