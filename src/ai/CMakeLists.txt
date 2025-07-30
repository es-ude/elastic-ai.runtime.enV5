add_library(ai
        ai.c
)
target_include_directories(ai PUBLIC include)
target_link_libraries(ai PRIVATE
    Common
)