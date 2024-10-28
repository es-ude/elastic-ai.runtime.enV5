add_library(FlashDummy)
target_sources(FlashDummy PRIVATE Flash.c)
target_include_directories(FlashDummy PUBLIC include)
target_link_libraries(FlashDummy PRIVATE Flash__hdrs Common__hdrs)