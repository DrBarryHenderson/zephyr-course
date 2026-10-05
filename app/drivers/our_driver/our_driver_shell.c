#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

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

// Create Handler for INFO command
static int info_cmd_handler (const struct shell* sh, size_t argc, char** argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    // Check if the Device is Ready
    shell_fprintf(sh, SHELL_INFO, "Device: %s (%s)\n", dev_driver->name, device_is_ready(dev_driver) ? "ready" : "NOT ready");

    shell_fprintf(sh, SHELL_INFO, "  sensor fetch : turn the LED ON  (sensor_sample_fetch)\n");

    shell_fprintf(sh, SHELL_INFO, "  sensor read  : read LED state, then turn it OFF (sensor_channel_get)\n");

    return 0;
}

// create sub commands
// SHELL_CMD_ARG takes (syntax, subcmd, help, handler, mandatory args, optional args)
SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
    SHELL_CMD_ARG(read, NULL, "Read Command for SENSOR Command Group", read_cmd_handler, 1, 0),
    SHELL_CMD_ARG(fetch, NULL, "Fetch Command for SENSOR Command Group", fetch_cmd_handler, 1, 0),
    SHELL_CMD_ARG(info, NULL, "INFO Command for SENSOR Command Group", info_cmd_handler, 1, 0),
    SHELL_SUBCMD_SET_END
);

// Register sensor as root command
SHELL_CMD_REGISTER(sensor, &sub_sensor, "Sensor commands: read, fetch and info", NULL);
