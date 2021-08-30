#include QMK_KEYBOARD_H

void matrix_init_kb(void) {
    led_init_ports();
    matrix_init_user();
}

void matrix_scan_kb(void) {
    matrix_scan_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    return process_record_user(keycode, record);
}

void suspend_power_down_kb(void) {
    #ifdef RGBLIGHT_SLEEP
    rgblight_disable_noeeprom();
    #endif
    suspend_power_down_user();
}

void suspend_wakeup_init_kb(void) {
    #ifdef RGBLIGHT_SLEEP
    rgblight_enable_noeeprom();
    #endif
    suspend_wakeup_init_user();
}