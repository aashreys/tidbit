/* Copyright 2020 Jay Greco
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "remote_kb.h"
#include "bitc_led.h"
// #include "print.h"
// #include "stdio.h"

// RBG Timeout Variables
static void refresh_rgb(void);
static void check_rgb_timeout(void);
bool is_rgb_timeout = false;
static uint16_t key_timer;

// Custom Layers and Macros
#define _ZOOM         0
#define _FIGMA        1
#define _MISC         2
#define _VIA3         3
#define _VIA4         4
#define _SYSTEM       5

int active_layer = 0;
int total_layers = 6;

enum macro_keycodes {
  // ZOOM
  ZOOM_TOGGLE_MUTE = SAFE_RANGE,
  ZOOM_TOGGLE_VIDEO,
  ZOOM_TOGGLE_SCRSHARE,
  ZOOM_TOGGLE_RAISE_HAND,
  
  // FIGMA
  FIGMA_ALIGNH_LEFT,
  FIGMA_ALIGNH_CENTER,
  FIGMA_ALIGNH_RIGHT,
  FIGMA_ALIGNV_TOP,
  FIGMA_ALIGNV_CENTER,
  FIGMA_ALIGNV_BOTTOM,
  FIGMA_DIST_HORIZONTAL,
  FIGMA_DIST_VERTICAL,
  FIGMA_RUN_LAST_PLUGIN,

  // SYSTEM
  SYS_TOGGLE_RGB
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_ZOOM] = LAYOUT(
                  ZOOM_TOGGLE_MUTE,     ZOOM_TOGGLE_VIDEO,    ZOOM_TOGGLE_SCRSHARE,      \
  TO(_ZOOM),      KC_NO,                KC_NO,                KC_NO,                     \
  TO(_FIGMA),          KC_NO,                KC_NO,                KC_NO,                     \
  KC_NO,          KC_NO,                KC_NO,                KC_NO,                     \
  KC_NO,          KC_NO,                KC_NO,                ZOOM_TOGGLE_RAISE_HAND                      \
  ),

  [_FIGMA] = LAYOUT(
           FIGMA_RUN_LAST_PLUGIN, KC_NO, KC_NO, \
  KC_TRNS, FIGMA_ALIGNH_LEFT, FIGMA_ALIGNH_CENTER, FIGMA_ALIGNH_RIGHT, \
  KC_TRNS, FIGMA_DIST_HORIZONTAL, KC_NO, FIGMA_ALIGNV_TOP, \
  KC_NO, KC_NO, KC_NO, FIGMA_ALIGNV_CENTER, \
  KC_NO, KC_NO, FIGMA_DIST_VERTICAL, FIGMA_ALIGNV_BOTTOM  \
  ),

  [_MISC] = LAYOUT(
           KC_NO, KC_NO, KC_NO, \
  KC_TRNS, KC_NO, KC_NO, KC_NO, \
  KC_TRNS, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO  \
  ),

  [_VIA3] = LAYOUT(
           KC_NO, KC_NO, KC_NO, \
  KC_TRNS, KC_NO, KC_NO, KC_NO, \
  KC_TRNS, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO  \
  ),

  [_VIA4] = LAYOUT(
           KC_A, KC_B, KC_C, \
  KC_TRNS, KC_NO, KC_NO, KC_NO, \
  KC_TRNS, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO  \
  ),

  [_SYSTEM] = LAYOUT(
           KC_D, KC_E, KC_F, \
  KC_TRNS, KC_NO, KC_NO, KC_NO, \
  KC_TRNS, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  SYS_TOGGLE_RGB, KC_NO, KC_NO, KC_NO  \
  ),
};

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) { return OLED_ROTATION_270; }

void oled_task_user(void) {
    // Host Keyboard Layer Status
    switch (get_highest_layer(layer_state)) {
        case _FIGMA:
            oled_write_P(PSTR("FIGMA\n"), false);
            break;
        case _ZOOM:
            oled_write_P(PSTR("ZOOM\n"), false);
            break;
        case _MISC:
            oled_write_P(PSTR("MISC\n"), false);
            break;
        case _VIA3:
            oled_write_P(PSTR("VIA 3\n"), false);
            break;
        case _VIA4:
            oled_write_P(PSTR("VIA 4\n"), false);
            break;
        case _SYSTEM:
            oled_write_P(PSTR("SYS\n"), false);
            break;
        default:
            oled_write_ln_P(PSTR("Undefined"), false);
    }
}
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  process_record_remote_kb(keycode, record);
  if (record->event.pressed) {
    switch (keycode) {
      // ZOOM
      case ZOOM_TOGGLE_MUTE: SEND_STRING(SS_DOWN(X_LGUI) SS_DOWN(X_LSHIFT) "a"); clear_keyboard(); break;
      case ZOOM_TOGGLE_VIDEO: SEND_STRING(SS_DOWN(X_LGUI) SS_DOWN(X_LSHIFT) "v"); clear_keyboard(); break;
      case ZOOM_TOGGLE_SCRSHARE: SEND_STRING(SS_DOWN(X_LGUI) SS_DOWN(X_LSHIFT) "s"); clear_keyboard(); break;
      case ZOOM_TOGGLE_RAISE_HAND: SEND_STRING(SS_LALT("y")); break;

      // FIGMA
      case FIGMA_ALIGNH_LEFT: SEND_STRING(SS_LALT("a")); break;
      case FIGMA_ALIGNH_CENTER: SEND_STRING(SS_LALT("h")); break;
      case FIGMA_ALIGNH_RIGHT: SEND_STRING(SS_LALT("d")); break;
      case FIGMA_ALIGNV_TOP: SEND_STRING(SS_LALT("w")); break;
      case FIGMA_ALIGNV_CENTER: SEND_STRING(SS_LALT("v")); break;
      case FIGMA_ALIGNV_BOTTOM: SEND_STRING(SS_LALT("s")); break;
      case FIGMA_DIST_HORIZONTAL: SEND_STRING(SS_DOWN(X_LCTRL) SS_DOWN(X_LALT) "h"); clear_keyboard(); break;
      case FIGMA_DIST_VERTICAL: SEND_STRING(SS_DOWN(X_LCTRL) SS_DOWN(X_LALT) "v"); clear_keyboard(); break;
      case FIGMA_RUN_LAST_PLUGIN: SEND_STRING(SS_DOWN(X_LGUI) SS_DOWN(X_LALT) "p"); clear_keyboard(); break;

      // SYSTEM
      case SYS_TOGGLE_RGB: rgblight_toggle(); break;

      default: break;
    }
  }
  return true;
}

void matrix_init_user(void) {
  matrix_init_remote_kb();
  set_bitc_LED(LED_OFF);
}

void matrix_scan_user(void) {
  matrix_scan_remote_kb();
}

bool encoder_update_user(uint8_t index, bool clockwise) {
  if (index == 0) {
    layer_move(
      clockwise ? ((active_layer - 1) + total_layers) % total_layers // reverse rollover
                : (active_layer + 1) % total_layers // rollover
    );
  }

  if (index == 1 && active_layer == _FIGMA) {
    if (clockwise) {
      tap_code(KC_DOWN);
    } else {
      tap_code(KC_UP);
    }
  }
  return true;
}

void led_set_kb(uint8_t usb_led) {
  if (usb_led & (1<<USB_LED_NUM_LOCK))
    set_bitc_LED(LED_DIM);
  else
    set_bitc_LED(LED_OFF);
}

layer_state_t layer_state_set_user(layer_state_t state) {
  active_layer = get_highest_layer(state);
  return state;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
  #ifdef RGBLIGHT_TIMEOUT
  if (record->event.pressed) refresh_rgb();
  #endif
}

void post_encoder_update_user(uint8_t index, bool clockwise) {
  #ifdef RGBLIGHT_TIMEOUT
  refresh_rgb();
  #endif
}

void housekeeping_task_user(void) {
  #ifdef RGBLIGHT_TIMEOUT
  check_rgb_timeout();
  #endif
}

void suspend_wakeup_init_user(void) {
    refresh_rgb();
}

void suspend_power_down_user(void) {
    is_rgb_timeout = true;
}

void refresh_rgb() {
  key_timer = timer_read(); // store time of last refresh
  if (is_rgb_timeout) { // only do something if rgb has timed out
    print("Activity detected, removing timeout\n");
    is_rgb_timeout = false;
    rgblight_wakeup();
  }
}

void check_rgb_timeout() {
  if (!is_rgb_timeout && timer_elapsed(key_timer) > RGBLIGHT_TIMEOUT) {
    rgblight_suspend();
    is_rgb_timeout = true;
  }
}