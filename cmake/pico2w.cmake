message("Building for pico2_w ")

set(BUILDING_FOR_ELASTIC_NODE ON CACHE INTERNAL "we're building for pico2_w")
set(PICO_BOARD pico2_w)
#set(PICO_PLATFORM rp2350)

if(${ELASTIC_AI_TARGET} EQUAL ENV5_REV1)
    set(REVISION "1" CACHE INTERNAL "")
else()
    set(REVISION "2" CACHE INTERNAL "")
endif ()