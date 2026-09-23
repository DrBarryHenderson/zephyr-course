if (CONFIG_OUR_DRIVER)
    target_sources(app PRIVATE our_driver.c)
    # Make our_driver.h (the custom extension API) visible to main.cpp
    target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif ()
