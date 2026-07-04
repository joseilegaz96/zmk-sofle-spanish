#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led_strip.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

#define STRIP_NODE DT_CHOSEN(zmk_underglow)
#define STRIP_NUM_PIXELS DT_PROP(STRIP_NODE, chain_length)

static const struct device *led_strip = DEVICE_DT_GET(STRIP_NODE);
static struct led_rgb pixels[STRIP_NUM_PIXELS];

/* Posiciones de teclado (según orden en el keymap) que quieres en rojo */
static const uint8_t highlighted_positions[] = {
    14, 15, 16, 17,   /* Q W E R */
    27, 28, 29, 30,   /* A S D F */
    39,               /* LSHFT */
    53,               /* LCTRL */
    55                /* LEFT_ALT */
};

#define NUM_HIGHLIGHTED (sizeof(highlighted_positions) / sizeof(highlighted_positions[0]))

/* Asume que el índice de LED en la tira == índice dentro de esta lista.
   Ajusta esto si conoces el mapeo real LED<->tecla de tu placa. */
static bool position_is_highlighted(uint8_t position, int *led_index_out) {
    for (int i = 0; i < NUM_HIGHLIGHTED; i++) {
        if (highlighted_positions[i] == position) {
            *led_index_out = i;
            return true;
        }
    }
    return false;
}

static void set_test_pixels(bool on) {
    for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
        pixels[i] = (struct led_rgb){ .r = 0, .g = 0, .b = 0 };
    }

    if (on) {
        for (int i = 0; i < NUM_HIGHLIGHTED && i < STRIP_NUM_PIXELS; i++) {
            pixels[i] = (struct led_rgb){ .r = 255, .g = 0, .b = 0 };
        }
    }

    led_strip_update_rgb(led_strip, pixels, STRIP_NUM_PIXELS);
}

static int layer_state_listener(const zmk_event_t *eh) {
    uint8_t layer = zmk_keymap_highest_layer_active();
    set_test_pixels(layer == 3);
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(rgb_layer_test, layer_state_listener);
ZMK_SUBSCRIPTION(rgb_layer_test, zmk_layer_state_changed);
