////////////////////////////////////////////////////////////////////////////////
// Includes
#include "state.h"

////////////////////////////////////////////////////////////////////////////////
// Global state: is_initialized
bool led_is_fast = false;

void set_led_is_fast(bool val) {
    led_is_fast = val;
}

bool get_led_is_fast() {
    return led_is_fast;
}
////////////////////////////////////////////////////////////////////////////////