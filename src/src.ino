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
#include <WiFiUdp.h>

WiFiUDP Udp;
unsigned int localUdpPort = 5005;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets

// -------------------------------------------------------------------
// SETUP
// -------------------------------------------------------------------
void setup() {
  delay(2000);

  Serial.begin(115200);
#ifdef DEBUG_SERIAL1
  Serial1.begin(115200);
#endif

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

  Udp.begin(localUdpPort);
  
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
  if (wifi_mode==WIFI_MODE_STA || wifi_mode==WIFI_MODE_AP_AND_STA)
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

  // UDP Broadcast receive 
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);

    if (strcmp(incomingPacket,"emonpi.local")==0) {
      if (mqtt_server!=Udp.remoteIP().toString().c_str()) {
        config_save_mqtt_server(Udp.remoteIP().toString().c_str());        
        Serial.printf("MQTT Server Updated");

        // Fetch emoncms mqtt broker credentials
        String url = "/emoncms/device/mqttauth.json";
        String result="";
        String mqtt_username = "";
        String mqtt_password = "";
        String mqtt_basetopic = "";
        int stringpart = 0;

        // This needs to be done with an encrypted request otherwise credentials are shared as plain text
        result = get_http(Udp.remoteIP().toString().c_str(), url);
        if (result!="request registered") {
            for (int i=0; i<result.length(); i++) {
                char c = result[i];
                if (c==':') { 
                    stringpart++; 
                } else {
                    if (stringpart==0) mqtt_username += c;
                    if (stringpart==1) mqtt_password += c;
                    if (stringpart==2) mqtt_basetopic += c;
                }
            }
            config_save_mqtt(Udp.remoteIP().toString().c_str(),mqtt_basetopic,"",mqtt_username,mqtt_password);
            DEBUG.println("MQTT Settings:"); DEBUG.println(result);
        }
        // ---------------------------------------------------------------------------------------------------
      }
    }
  }
} // end loop
