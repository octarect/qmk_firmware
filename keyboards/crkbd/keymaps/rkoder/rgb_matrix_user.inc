// vi: ft=c

RGB_MATRIX_EFFECT(ALPHA_UTIL)
RGB_MATRIX_EFFECT(RANDOM_REACTIVE_MULTICROSS)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#   define RGB_MATRIX_KEYREACTIVE_ENABLED
#   define UG 255
#   define LED_KEY_INDEX( \
  L00, L01, L02, L03, L04, L05,           R00, R01, R02, R03, R04, R05, \
  L10, L11, L12, L13, L14, L15,           R10, R11, R12, R13, R14, R15, \
  L20, L21, L22, L23, L24, L25,           R20, R21, R22, R23, R24, R25, \
                      L31, L32, L33, R31, R32, R33 \
) { \
   UG,  UG,  UG,  UG,  UG,  UG, \
  L33, L25, L15, L05, L04, L14, \
  L24, L32, L31, L23, L13, L03, \
  L02, L12, L22, L21, L11, L01, \
  L00, L10, L20, \
   UG,  UG,  UG,  UG,  UG,  UG, \
  R31, R20, R10, R00, R01, R11, \
  R21, R32, R33, R22, R12, R02, \
  R03, R13, R23, R24, R14, R04, \
  R05, R15, R25 \
}
#   define LED_KEY_MAPS LED_KEY_INDEX( \
   0,  1,  2,  3,  4,  5,         29, 28, 27, 26, 25, 24, \
   6,  7,  8,  9, 10, 11,         35, 34, 33, 32, 31, 30, \
  12, 13, 14, 15, 16, 17,         41, 40, 39, 38, 37, 36, \
                  21, 22, 23, 47, 46, 45)
#   define LED_NUM_SPLIT (DRIVER_LED_TOTAL / 2)

uint8_t led_keymaps[] = LED_KEY_MAPS;
uint16_t get_keycode_on_led(uint8_t i, bool isLeftHand) {
    uint8_t keymap_idx = led_keymaps[i];
    if (isLeftHand) {
      keymap_idx = led_keymaps[i];
    } else {
      keymap_idx = led_keymaps[i + LED_NUM_SPLIT];
    }
    uint8_t row = keymap_idx / MATRIX_COLS;
    uint8_t col = keymap_idx % MATRIX_COLS;
    uint8_t layer_idx = get_highest_layer(layer_state);
    return pgm_read_word_near(&keymaps[layer_idx][row][col]);
}

extern bool isLeftHand;

static bool ALPHA_UTIL(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    if (params->init) {
        rgb_matrix_set_color_all(0, 0, 0);
        memset(g_rgb_frame_buffer, 0, sizeof g_rgb_frame_buffer);
    }

    for (uint8_t i = led_min; i < led_max; i++) {
        if (HAS_FLAGS(g_led_config.flags[i], LED_FLAG_UNDERGLOW)) {
            rgb_matrix_set_color(i, 0x00, 0x10, 0x00);
            continue;
        }

        uint16_t kc = get_keycode_on_led(i, isLeftHand);
        rgb_matrix_set_color(i, 0x00, 0x00, 0x00);
        if (isLeftHand) {
            if (KC_1 <= kc && kc <= KC_0) {
                rgb_matrix_set_color(i, 0x40, 0x00, 0x20);
            } else if ((KC_KP_SLASH <= kc && kc <= KC_KP_PLUS) || kc == KC_KP_EQUAL) {
                rgb_matrix_set_color(i, 0x00, 0x00, 0x60);
            }
        } else {
            if (KC_MS_UP <= kc && kc <= KC_MS_RIGHT) {
                rgb_matrix_set_color(i, 0x40, 0x00, 0x20);
            } else if (KC_MS_BTN1 <= kc && kc <= KC_MS_BTN5) {
                rgb_matrix_set_color(i, 0x00, 0x00, 0x60);
            } else if (KC_MS_WH_UP <= kc && kc <= KC_MS_WH_RIGHT) {
                rgb_matrix_set_color(i, 0x30, 0x30, 0x00);
            }
        }
    }
    return led_max < DRIVER_LED_TOTAL;
}

HSV cross_colors[32];
static HSV RANDOM_REACTIVE_MULTICROSS_math(HSV hsv, uint8_t current, int16_t dx, int16_t dy, uint8_t dist, uint16_t tick) {
    uint16_t effect = tick + dist;
    dx              = dx < 0 ? dx * -1 : dx;
    dy              = dy < 0 ? dy * -1 : dy;
    dx              = dx * 16 > 255 ? 255 : dx * 16;
    dy              = dy * 16 > 255 ? 255 : dy * 16;
    effect += dx > dy ? dy : dx;
    if (effect > 255) effect = 255;
    hsv.v = qadd8(hsv.v, 255 - effect);

    if (tick == 0) {
      cross_colors[current].h = rand() % 256;
      cross_colors[current].s = 160 + (rand() % 96);
    }
    hsv.h = cross_colors[current].h;
    hsv.s = cross_colors[current].s;

    return hsv;
}

static bool RANDOM_REACTIVE_MULTICROSS(effect_params_t* params) {
    return effect_runner_reactive_splash_buf(0, params, &RANDOM_REACTIVE_MULTICROSS_math);
}
#endif
