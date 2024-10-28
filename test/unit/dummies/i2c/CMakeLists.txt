add_library(I2cDummy)
target_sources(I2cDummy PRIVATE I2c.c)
target_sources(I2cDummy PUBLIC
        FILE_SET headers
        TYPE HEADERS
        BASE_DIRS include
        FILES include/I2cUnitTest.h)
target_link_libraries(I2cDummy I2c__hdrs)
