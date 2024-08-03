add_executable(flashLoader)
target_sources(flashLoader PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/FlashLoader.c
)
target_include_directories(flashLoader PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(flashLoader PUBLIC
        hardware_structs
        hardware_sync
        hardware_flash
        hardware_watchdog
        hardware_resets
        hardware_xosc
        hardware_clocks
        hardware_pll
        hardware_dma
        pico_platform
        pico_standard_link
        pico_divider
)

pico_add_uf2_output(flashLoader)
#pico_set_program_name(${FLASHLOADER} ${FLASHLOADER})
target_compile_options(flashLoader PRIVATE -Wall -Wextra -Wno-ignored-qualifiers -Os)

# Use a separate linker script for the flashloader to make sure it is built
# to run at the right location and cannot overflow into the applications's
# address space
set_linker_script(flashLoader memmap_flashloader.ld)

add_library(flashLoader_lib INTERFACE include/FlashLoader.h)
target_include_directories(flashLoader_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
