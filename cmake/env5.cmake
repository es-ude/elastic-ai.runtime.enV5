message("Building for env5")

set(BUILDING_FOR_ELASTIC_NODE ON CACHE INTERNAL "we're building for elastic node")
set(PICO_BOARD none)
set(PICO_PLATFORM rp2040)

if(${ELASTIC_AI_TARGET} EQUAL ENV5_REV1)
    set(REVISION "1" CACHE INTERNAL "")
else()
    set(REVISION "2" CACHE INTERNAL "")
endif ()