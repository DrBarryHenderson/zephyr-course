if (CONFIG_OUR_DRIVER)
    target_sources(app PRIVATE our_driver.c)
    # conditional compilation of Shell C code if ZEPHYR Shell is enabled
    if (CONFIG_SHELL)
       target_sources(app PRIVATE our_driver_shell.c)
    endif ()
endif ()
