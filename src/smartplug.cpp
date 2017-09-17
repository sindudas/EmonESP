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
 
 // Handle Sonoff S20 ESP Smartplug control

#include "mqtt.h"
#include "emonesp.h"
#include "smartplug.h"

#include <Arduino.h>

boolean relay_state;

byte control_loop(){
  if (digitalRead(0) == 1 && relay_state ==0){
    relay_state = 1;
    switch_relay(relay_state);
  }
  
  
  if (digitalRead(0) == 1 && relay_state ==1){
    relay_state = 0;
    switch_relay(relay_state);
  }
  return relay_state;
}

void switch_relay(boolean state){
  
  if (state==1) {
      DEBUG.println("RELAY STATE:1");
      digitalWrite(12,HIGH);        // RELAY SONOFF S20
      digitalWrite(16,HIGH);
  if (state==0){
      DEBUG.println("RELAY STATE:0");
      digitalWrite(12,LOW);        // RELAY SONOFF S20
      digitalWrite(16,LOW);
    }
  }
}