#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

// refer to Heartbeat LED 
//#define LED_NODE DT_ALIAS(led1)
#define HBEAT_LED_NODE DT_ALIAS(app_led)

// Used by Dr Barry on Norid nRF52840-DK Board for LED0 Blink
//#define SLEEP_TIME_MS 1000 

/* The devicetree node identifier for the "led0" alias. */
//#define LED_NODE DT_ALIAS(led0)

// static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

/* The devicetree node identifier for the "Heart Beat LED alias. */
static const struct gpio_dt_spec hbeat_led = GPIO_DT_SPEC_GET(HBEAT_LED_NODE, gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    bool led_state = true;
    bool hbeat_led_state = true;

   // if (!gpio_is_ready_dt(&led)) return 0;
    //if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) return 0;

    // Check Heart Beat LED is ready and ACTIVE as an output
    if (!gpio_is_ready_dt(&hbeat_led)) return 0;
    if (gpio_pin_configure_dt(&hbeat_led, GPIO_OUTPUT_ACTIVE) < 0) return 0;

    while (1) {
      
      //if (gpio_pin_toggle_dt(&led) < 0) return 0;
      // led_state = !led_state;
      // Toggle led state
      // LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
      //  k_msleep(SLEEP_TIME_MS);
      //  k_msleep(CONFIG_BLINK_TIME_TIME_MS);

        // Attempt to Toggle the Heart beat LED state: return to OS if it fails to Toggle
        if (gpio_pin_toggle_dt(&hbeat_led) < 0) return 0;
        // Toggle Heart beat state variable
        hbeat_led_state = !hbeat_led_state;
        // Print out the Heartbeat led state
        LOG_INF("Heartbeat LED state: %s", hbeat_led_state ? "ON" : "OFF");
        // WAIT for HEARTBEAT Delay Heart beat LED
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
    return 0;
}
