message("building for native host platform")

set(_BUILDING_FOR_HARDWARE OFF CACHE INTERNAL
  "we're building for a hardware target"
)

set(PICO_BOARD none)
set(PICO_PLATFORM host)
set(_EAI_REVISION "1" CACHE INTERNAL "")

