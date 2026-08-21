message("building for native host platform")

set(_BUILDING_FOR_HARDWARE OFF CACHE INTERNAL
        "We're building for host target"
)

set(PICO_BOARD none)
set(PICO_PLATFORM host)

