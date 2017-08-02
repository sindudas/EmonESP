/*
 * -------------------------------------------------------------------
 * EmonESP Serial to Emoncms gateway
 * -------------------------------------------------------------------
 * Adaptation of Chris Howells OpenEVSE ESP Wifi
 * by Trystan Lea, Glyn Hudson, OpenEnergyMonitor
 * All adaptation GNU General Public License as below.
 *
 * -------------------------------------------------------------------
 *
 * This file is part of OpenEnergyMonitor.org project.
 * EmonESP is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * EmonESP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with EmonESP; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "emonesp.h"
#include "config.h"
#include "wifi.h"
#include "web_server.h"
#include "ota.h"
#include "input.h"
#include "emoncms.h"
#include "mqtt.h"
#include "http.h"
#include "autoauth.h"

bool enable_mqtt_control = 1;

// -------------------------------------------------------------------
// SETUP
// -------------------------------------------------------------------
void setup() {

  // ---------------------------------------------------------
  // Hard-coded initial config for node_name and node_describe
  // ---------------------------------------------------------
  if (enable_mqtt_control) {
    node_name = "smartplug";
    node_describe = "describe:smartplug";
  } else {
    node_name = "emontx";
    node_describe = "describe:emontx-HP";
  }
  // ---------------------------------------------------------
  
  delay(2000);

  Serial.begin(115200);
#ifdef DEBUG_SERIAL1
  Serial1.begin(115200);
#endif


  if (enable_mqtt_control) {
    // pinMode(5, OUTPUT);    // WIFI RELAY
    pinMode(12, OUTPUT);      // SONOFF S20
    pinMode(16, OUTPUT); 
  }

  DEBUG.println();
  DEBUG.print("EmonESP ");
  DEBUG.println(ESP.getChipId());
  DEBUG.println("Firmware: "+ currentfirmware);

  // Read saved settings from the config
  config_load_settings();

  // Initialise the WiFi
  wifi_setup();

  // Bring up the web server
  web_server_setup();

  // Start the OTA update systems
  ota_setup();

  DEBUG.println("Server started");

  // Start auto auth
  auth_setup();
  
  delay(100);
} // end setup

// -------------------------------------------------------------------
// LOOP
// -------------------------------------------------------------------
void loop()
{
  ota_loop();
  web_server_loop();
  wifi_loop();

  String input = "";
  boolean gotInput = input_get(input);

  if (wifi_mode == WIFI_MODE_STA || wifi_mode == WIFI_MODE_AP_AND_STA)
  {
    if(emoncms_apikey != 0 && gotInput) {
      emoncms_publish(input);
    }
    if(mqtt_server != 0)
    {
      mqtt_loop();
      if(gotInput) {
        mqtt_publish(input);
      }
    }
  }

  auth_loop();
  
} // end loop
