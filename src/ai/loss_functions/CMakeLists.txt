add_library(MSE
        MSE.c
)
target_include_directories(MSE PUBLIC include)
target_link_libraries(MSE PRIVATE
        Common
        AiHelpers
)