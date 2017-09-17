#ifndef _EMONESP_SMARTPLUG_H
#define _EMONESP_SMARTPLUG_H

#include <Arduino.h>

// Sonoff S20 ESP Smart Plug

extern byte relay_state;


// -------------------------------------------------------------------
// Update Relay
// -------------------------------------------------------------------
extern byte control_loop();
extern void switch_relay(boolean relay_state);

#endif // _EMONESP_SMARTPLUG_H