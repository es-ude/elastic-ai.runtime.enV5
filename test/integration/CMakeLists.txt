add_executable(SimpleSequentialTest SimpleSequentialTest.c)
target_link_libraries(SimpleSequentialTest PRIVATE
        Tensor
        Layer
        Linear
)