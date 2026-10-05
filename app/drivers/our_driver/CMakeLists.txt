if (CONFIG_OUR_DRIVER)
    target_sources(app PRIVATE our_driver.c)
    # Make our_driver.h (the custom extension API) visible to the shell and main.cpp
    target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    # conditional compilation of Shell C code if ZEPHYR Shell is enabled
    if (CONFIG_SHELL)
       target_sources(app PRIVATE our_driver_shell.c)
    endif ()
endif ()
