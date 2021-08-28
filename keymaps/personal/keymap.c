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

#define _ZOOM         0
#define _FIGMA        1
#define _MISC         2
#define _VIA3         3
#define _VIA4         4
#define _SYSTEM       5

int active_layer = _FIGMA;
int total_layers = 6;

enum macro_keycodes {
  // ZOOM MACROS
  ZOOM_TOGGLE_MUTE = SAFE_RANGE,
  ZOOM_TOGGLE_VIDEO,
  ZOOM_TOGGLE_SCRSHARE,
  
  // FIGMA MACROS
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_ZOOM] = LAYOUT(
                  ZOOM_TOGGLE_MUTE,     ZOOM_TOGGLE_VIDEO,    ZOOM_TOGGLE_SCRSHARE,       \
  TO(_ZOOM),      KC_NO,                KC_NO,                KC_NO,                      \
  KC_NO,          KC_NO,                KC_NO,                KC_NO,                      \
  KC_NO,          KC_NO,                KC_NO,                KC_NO,                      \
  KC_NO,          KC_NO,                KC_NO,                KC_NO                       \
  ),

  [_FIGMA] = LAYOUT(
           KC_NO, KC_NO, KC_NO, \
  KC_TRNS, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO  \
  ),

  [_MISC] = LAYOUT(
           KC_NO, KC_NO, KC_NO, \
  KC_TRNS, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO  \
  ),

  [_VIA3] = LAYOUT(
           KC_NO, KC_NO, KC_NO, \
  KC_TRNS, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO  \
  ),

  [_VIA4] = LAYOUT(
           KC_NO, KC_NO, KC_NO, \
  KC_TRNS, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO  \
  ),

  [_SYSTEM] = LAYOUT(
           KC_NO, KC_NO, KC_NO, \
  KC_TRNS, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  KC_NO, KC_NO, KC_NO, KC_NO  \
  ),
};

#ifdef OLED_DRIVER_ENABLE
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
            // Or use the write_ln shortcut over adding '\n' to the end of your string
            oled_write_ln_P(PSTR("Undefined"), false);
    }
}
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case ZOOM_TOGGLE_MUTE: if (record->event.pressed) SEND_STRING(SS_DOWN(X_LGUI) SS_DOWN(X_LSHIFT) "A"); clear_keyboard(); break;
    case ZOOM_TOGGLE_VIDEO: if (record->event.pressed) SEND_STRING(SS_DOWN(X_LGUI) SS_DOWN(X_LSHIFT) "V"); clear_keyboard(); break;
    case ZOOM_TOGGLE_SCRSHARE: if (record->event.pressed) SEND_STRING(SS_DOWN(X_LGUI) SS_DOWN(X_LSHIFT) "S"); clear_keyboard(); break;

    default: process_record_remote_kb(keycode, record);
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
    int new_layer;
    if (clockwise) {
      new_layer = ((active_layer - 1) + total_layers) % total_layers; // reverse rollover
    } else {
      new_layer = (active_layer + 1) % total_layers; // rollover
    }
    layer_move(new_layer);
  }

  if (index == 1) {
    if (clockwise) {
      tap_code(KC_VOLU);
    } else {
      tap_code(KC_VOLD);
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