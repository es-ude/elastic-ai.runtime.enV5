message("Building for elastic node version 5")

set(_BUILDING_FOR_HARDWARE ON CACHE INTERNAL
  "we're building for a hardware target"
)

set(PICO_BOARD none)
set(PICO_PLATFORM rp2040)
if(${ELASTIC_AI_TARGET} STREQUAL ENV5_1)
  message("Building for enV5 hardware revision 1")
  set(_EAI_REVISION "1" CACHE INTERNAL "")
elseif(${ELASTIC_AI_TARGET} STREQUAL ENV5_2)
  message("Building for enV5 hardware revision 2")
  set(_EAI_REVISION "2" CACHE INTERNAL "")
endif()
