add_library(DTypes
        DTypes.c
)
target_include_directories(DTypes PUBLIC include)
target_link_libraries(DTypes PRIVATE
        Common
)

