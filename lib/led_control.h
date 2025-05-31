#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdbool.h>
#include "mqtt_client.h"

void control_led(MQTT_CLIENT_DATA_T *state, bool on);
void control_red_led(bool on);

#endif // LED_CONTROL_H
