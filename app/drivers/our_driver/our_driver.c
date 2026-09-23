#define DT_DRV_COMPAT our_driver

#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include "our_driver.h"

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

// Constant (read-only) settings for our device, filled in from the devicetree
struct our_driver_config {
    struct gpio_dt_spec led;
};

// Dynamic (read/write) data for our device, can change while running
struct our_driver_data {
    bool led_enabled;       // false = sample_fetch will NOT turn LED2 ON
    uint32_t fetch_count;   // how many times sample_fetch has been called
};

// sensor_sample_fetch(): turns the LED ON
static int sample_fetch_my_impl(const struct device *dev,
                                enum sensor_channel chan)
    {
        const struct our_driver_config *config = dev->config;
        struct our_driver_data *data = dev->data;

        data->fetch_count++;
        LOG_INF("Hello from Sensor Sample Fetch, channel %d, fetch #%u", chan, data->fetch_count);

        if (!data->led_enabled) {
            LOG_INF("LED2 is disabled, leaving it OFF");
            return 0;
        }

        // 1 = active = ON (the devicetree flags handle ACTIVE_LOW for us)
        int ret = gpio_pin_set_dt(&config->led, 1);
        if (ret < 0) return ret;

        LOG_INF("LED2 turned ON");
        return 0;
    }

// sensor_channel_get(): reads the LED state into val->val1, then turns the LED OFF
static int channel_get_my_impl(const struct device *dev,
                               enum sensor_channel chan,
                               struct sensor_value *val
                               )
    {
        const struct our_driver_config *config = dev->config;

        LOG_INF("Hello from Sensor Channel Get, channel %d",chan);

        // Read the LED pin: 1 = ON (active), 0 = OFF, negative = error
        int state = gpio_pin_get_dt(&config->led);
        if (state < 0) return state;

        int ret = gpio_pin_set_dt(&config->led, 0);
        if (ret < 0) return ret;

        LOG_INF("LED2 was %s, turned OFF", state ? "ON" : "OFF");

        // Return the state the LED was in before we turned it off
        val->val1 = state;
        val->val2 = 0;
        return 0;
    }

// Custom extension API (declared in our_driver.h): changes led_enabled in the dynamic data
int our_driver_set_led_enabled(const struct device *dev, bool enable)
{
    struct our_driver_data *data = dev->data;

    data->led_enabled = enable;
    LOG_INF("LED2 %s", enable ? "ENABLED" : "DISABLED");
    return 0;
}

// Init function
static int init(const struct device* dev) {
    const struct our_driver_config *config = dev->config;
    struct our_driver_data *data = dev->data;

    data->led_enabled = true;
    data->fetch_count = 0;

    if (!gpio_is_ready_dt(&config->led)) return -ENODEV;

    // Output so we can drive it, plus input so we can read the pin state back
    int ret = gpio_pin_configure_dt(&config->led, GPIO_OUTPUT_INACTIVE | GPIO_INPUT);
    if (ret < 0) return ret;

    LOG_INF("Sensor Device Initialised");
    return 0;
}

static DEVICE_API(sensor, api_barry_nordic) = {
    .sample_fetch = sample_fetch_my_impl,
    .channel_get = channel_get_my_impl,
};

/*
#define DEVICE_DT_DEFINE	(		
node_id,
init_fn,
pm,
data,
config,
level,
prio,
api,
... ) 

Parameters
node_id	The devicetree node identifier.
init_fn	Pointer to the device's initialization function, which will be run by the kernel during system initialization. Can be NULL.
deinit_fn	Pointer to the device's de-initialization function. Can be NULL. It must release any acquired resources (e.g. pins, bus, clock...) and leave the device in its reset state.
pm	Pointer to the device's power management resources, a pm_device, which will be stored in device::pm. Use NULL if the device does not use PM.
data	Pointer to the device's private mutable data, which will be stored in device::data.
config	Pointer to the device's private constant data, which will be stored in device::config field.
level	The device's initialization level (PRE_KERNEL_1, PRE_KERNEL_2 or POST_KERNEL).
prio	The device's priority within its initialization level. See SYS_INIT() for details.
api	Pointer to the device's API structure. Can be NULL.

*/

// Take the gpios of the LED node that our "led" property points to (&led2)
static const struct our_driver_config config0 = {
    .led = GPIO_DT_SPEC_GET(DT_INST_PHANDLE(0, led), gpios),
};

// Dynamic data for instance 0 (not const: the driver writes to it)
static struct our_driver_data data0;

DEVICE_DT_INST_DEFINE(0, init, NULL, &data0, &config0, POST_KERNEL, 80, &api_barry_nordic);
