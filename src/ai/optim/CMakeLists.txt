add_library(SGD
        SGD.c
)
target_include_directories(SGD PUBLIC include)
target_link_libraries(SGD PRIVATE
        Common
        AiHelpers
)