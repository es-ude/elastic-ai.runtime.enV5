add_library(float32 Float32.c)

target_include_directories(float32 PUBLIC include)
target_link_libraries(float32 PRIVATE
        AiHelpers
)

add_library(float16 Float16.c)

target_include_directories(float16 PUBLIC include)
target_link_libraries(float16 PRIVATE
        AiHelpers
)

add_library(Quantization Quantization.c
)
target_include_directories(Quantization PUBLIC include)
target_link_libraries(Quantization PRIVATE
        Common
        AiHelpers
        m
)