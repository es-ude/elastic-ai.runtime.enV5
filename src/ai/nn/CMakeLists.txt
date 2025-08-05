add_library(Linear
        Linear.c
)
target_include_directories(Linear PUBLIC include)
target_link_libraries(Linear PRIVATE
        Common
        AiHelpers
)

add_library(ReLU
        ReLU.c
)
target_include_directories(ReLU PUBLIC include)
target_link_libraries(ReLU PRIVATE
        Common
        AiHelpers
)