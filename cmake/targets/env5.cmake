message("Building for elastic node version 5")

set(_BUILDING_FOR_HARDWARE ON CACHE INTERNAL
        "We're building for a hardware target"
)

set(PICO_BOARD none)
set(PICO_PLATFORM rp2040)

if (EAI_TARGET_PLATFORM_REVISION STREQUAL 1)
    message("Building for enV5 hardware revision 1")
    set(_EAI_REVISION "1" CACHE INTERNAL "")
    set(HW_CONFIG_PATH)

elseif (EAI_TARGET_PLATFORM_REVISION STREQUAL 2)
    message("Building for enV5 hardware revision 2")
    set(_EAI_REVISION "2" CACHE INTERNAL "")

endif ()
