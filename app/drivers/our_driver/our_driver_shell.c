#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

#include "our_driver.h"

// Valid range for "sensor set <value>": 0 = LED2 disabled, 1 = LED2 enabled
#define SET_VALUE_MIN 0
#define SET_VALUE_MAX 1

//const struct device *dev_driver = shell_device_get_binding("our_driver");
static const struct device *const dev_driver = DEVICE_DT_GET(DT_NODELABEL(our_driver0));

// Create Handler for READ command
static int read_cmd_handler (const struct shell* sh, size_t argc, char** argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    if (!device_is_ready(dev_driver)) {
        shell_error(sh, "%s is not ready", dev_driver->name);
        return -ENODEV;
    }

    // CALL: sensor_channel_get()
    struct sensor_value val;
    int ret = sensor_channel_get(dev_driver, SENSOR_CHAN_AMBIENT_TEMP, &val);
    // Check for Driver error
     if (ret < 0) {
      shell_warn(sh, "sensor_channel_get: LED Driver reports not ready!");
      return ret;   
     }

    shell_fprintf(sh, SHELL_INFO, "read command handler: READ val: %d; LED State: %s\n", val.val1, val.val1 ? "ON" : "OFF");

    return 0;
}

// Create Handler for FETCH command
static int fetch_cmd_handler (const struct shell* sh, size_t argc, char** argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    if (!device_is_ready(dev_driver)) {
        shell_error(sh, "%s is not ready", dev_driver->name);
        return -ENODEV;
    }

    // CALL: sensor_sample_fetch()
    int ret = sensor_sample_fetch(dev_driver);
    if (ret < 0) {
        shell_warn(sh, "fetch_cmd_handler: LED Driver reports not ready!");
        return ret;
    }

    // shell_info() already adds the newline
    shell_print(sh, "fetch command handler. LED Turned ON through Fetch");

    return 0;
}

// Create Handler for SET command: sensor set <0|1>
static int set_cmd_handler (const struct shell* sh, size_t argc, char** argv)
{
    // SHELL_CMD_ARG(..., 2, 0) means the shell rejects a missing value before we
    // get here, but check anyway in case the registration is ever changed
    if (argc < 2) {
        shell_error(sh, "Missing value. Usage: sensor set <%d|%d>", SET_VALUE_MIN, SET_VALUE_MAX);
        return -EINVAL;
    }

    // Convert the argument text to a number; err is only changed on failure
    int err = 0;
    long value = shell_strtol(argv[1], 10, &err);
    if (err != 0) {
        shell_error(sh, "'%s' is not a number. Usage: sensor set <%d|%d>", argv[1], SET_VALUE_MIN, SET_VALUE_MAX);
        return -EINVAL;
    }

    // Range check
    if (value < SET_VALUE_MIN || value > SET_VALUE_MAX) {
        shell_error(sh, "Value %ld out of range (%d..%d)", value, SET_VALUE_MIN, SET_VALUE_MAX);
        return -ERANGE;
    }

    if (!device_is_ready(dev_driver)) {
        shell_error(sh, "%s is not ready", dev_driver->name);
        return -ENODEV;
    }

    // CALL: our custom extension API from L06 Task 2
    int ret = our_driver_set_led_enabled(dev_driver, value == 1);
    if (ret < 0) {
        shell_error(sh, "our_driver_set_led_enabled failed: %d", ret);
        return ret;
    }

    shell_print(sh, "set command handler. LED2 %s", value ? "ENABLED" : "DISABLED");

    return 0;
}

// Create Handler for INFO command
static int info_cmd_handler (const struct shell* sh, size_t argc, char** argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    // Check if the Device is Ready
    shell_fprintf(sh, SHELL_INFO, "Device: %s (%s)\n", dev_driver->name, device_is_ready(dev_driver) ? "ready" : "NOT ready");

    shell_fprintf(sh, SHELL_INFO, "  sensor fetch : turn the LED ON  (sensor_sample_fetch)\n");

    shell_fprintf(sh, SHELL_INFO, "  sensor read  : read LED state, then turn it OFF (sensor_channel_get)\n");

    shell_fprintf(sh, SHELL_INFO, "  sensor set <0|1> : disable/enable LED2 (our_driver_set_led_enabled)\n");

    return 0;
}

// create sub commands
// SHELL_CMD_ARG takes (syntax, subcmd, help, handler, mandatory args, optional args)
SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
    SHELL_CMD_ARG(read, NULL, "Read Command for SENSOR Command Group", read_cmd_handler, 1, 0),
    SHELL_CMD_ARG(fetch, NULL, "Fetch Command for SENSOR Command Group", fetch_cmd_handler, 1, 0),
    SHELL_CMD_ARG(info, NULL, "INFO Command for SENSOR Command Group", info_cmd_handler, 1, 0),
    // 2 mandatory = "set" itself + <value>, 0 optional: exactly one value allowed
    SHELL_CMD_ARG(set, NULL, "Enable/disable LED2: sensor set <0|1>", set_cmd_handler, 2, 0),
    SHELL_SUBCMD_SET_END
);

// Register sensor as root command
SHELL_CMD_REGISTER(sensor, &sub_sensor, "Sensor commands: read, fetch, set and info", NULL);
