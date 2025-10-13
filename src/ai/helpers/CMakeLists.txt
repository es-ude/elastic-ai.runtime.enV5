add_library(AiHelpers
        AiHelpers.c
)
target_include_directories(AiHelpers PUBLIC include)
target_link_libraries(AiHelpers PRIVATE
        Common
        Quantization
        Linear
        ReLU
        Conv1d
        Softmax
        CSVReader
        MSE
        CrossEntropy

)


add_library(CSVReader
        CSVReader.c
)
target_include_directories(CSVReader PUBLIC include)
target_link_libraries(CSVReader PRIVATE
        Common
)

