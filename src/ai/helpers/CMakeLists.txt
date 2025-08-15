add_library(AiHelpers
        AiHelpers.c
)
target_include_directories(AiHelpers PUBLIC include)
target_link_libraries(AiHelpers PRIVATE
        Common
        Linear
        ReLU
        Conv1d
)
