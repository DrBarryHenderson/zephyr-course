#ifndef OUR_DRIVER_H_
#define OUR_DRIVER_H_

#include <stdbool.h>
#include <zephyr/device.h>

// main.cpp is C++ but our driver is C: stop C++ renaming (mangling) these functions
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Custom extension API for our_driver (on top of the standard sensor API).
 *
 * Enable or disable LED2. When disabled, sensor_sample_fetch() will not turn it ON.
 *
 * @param dev    our_driver device
 * @param enable true = LED2 allowed to turn ON, false = LED2 stays OFF
 * @return 0 on success, negative error code on failure
 */
int our_driver_set_led_enabled(const struct device *dev, bool enable);

#ifdef __cplusplus
}
#endif

#endif /* OUR_DRIVER_H_ */
