message("building for native host platform")

set(BUILDING_FOR_ELASTIC_NODE OFF CACHE INTERNAL "we're building for elastic node" )
set(PICO_BOARD none )
set(PICO_PLATFORM host )

set(REVISION "1" CACHE INTERNAL "")