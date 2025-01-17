add_library(FileSystem FileSystem.c)
target_include_directories(FileSystem PUBLIC include)
target_link_libraries(FileSystem PRIVATE
        Common
        Flash
)