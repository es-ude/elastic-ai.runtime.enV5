add_library(SGD
        SGD.c
)
target_include_directories(SGD PUBLIC include)
target_link_libraries(SGD PRIVATE
        AiHelpers
        Linear
        Conv1d
        ReLU
)