/* 
  WiFiTelnetToSerial - Example Transparent UART to Telnet Server for esp8266

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the ESP8266WiFi library for Arduino environment.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <ESP8266WiFi.h>
#define LINE_START_SYMBOL '~'
//adding Cyryllic symbols for print function.
//char* RUS(char* StrIn){char* StrOut=StrIn; uint8_t i[4]={0,0,StrIn[0],StrIn[1]}; while(i[2]>0&&i[0]<255){if(i[2]==0xd0&&i[3]>0x8F&&i[3]<0xC0){StrOut[i[1]]=i[3]+0x30;i[0]++;}else if(i[2]==0xd0&&i[3]==0x81){StrOut[i[1]]=0xA8;i[0]++;}else if(i[2]==0xd1&&i[3]>0x7F&&i[3]<0x90){StrOut[i[1]]=i[3]+0x70;i[0]++;}else if(i[2]==0xd1&&i[3]==0x91){StrOut[i[1]]=0xB8;i[0]++;}else{StrOut[i[1]]=i[2];} i[0]++; i[1]++; i[2]=StrIn[i[0]]; i[3]=StrIn[i[0]+1];} StrOut[i[1]]='\0'; return StrOut;}

//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 1
const char* ssid = "onlime03";
const char* password = "9687485462";

WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

void print(String string){
  Serial.println(String(LINE_START_SYMBOL) + string);
}

String DisplayAddress(IPAddress address)
{
 return String(address[0]) + "." + 
        String(address[1]) + "." + 
        String(address[2]) + "." + 
        String(address[3]);
}

void setup() {
//  Serial1.begin(115200);
  WiFi.begin(ssid, password);
  delay(5000);
  Serial.begin(115200);
  Serial.print("\n-");
  print("Connecting to ");
  print(ssid);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  if(i == 21){
    print("Could not connect to"); print(ssid);
    while(1) delay(500);
  }
  //start UART and the server
  server.begin();
  server.setNoDelay(true);
  print("Ready! Use telnet ");
  print(DisplayAddress(WiFi.localIP()));
}

void loop() {
  uint16_t i;
  uint8_t c;
  //check if there are any new clients
  if (server.hasClient()){
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()){
        if(serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        print("New client: " + i);
// стираем буфер - начало
        delay(50);
        while(serverClients[i].available())
        { 
          c = serverClients[i].read();
        }
// стираем буфер - конец       
        continue;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }

  
  //check clients for data | PuTTY to Serial Monitor
  for(i = 0; i < MAX_SRV_CLIENTS; i++){
    if (serverClients[i] && serverClients[i].connected()){
      if(serverClients[i].available()){
        //get data from the telnet client and push it to the Serial Monitor
        while(serverClients[i].available())
        { 
          c = serverClients[i].read();
          Serial.write(c);
        }
      }
    }
  }
  //check Serial Monitor for data
  if(Serial.available()){
    delay(100);
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    //push UART data to all connected telnet clients
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      if (serverClients[i] && serverClients[i].connected()){
        serverClients[i].write('~');
        serverClients[i].write(sbuf, len);
        serverClients[i].write('\r');
        serverClients[i].write('\n');
        delay(1);
      }
    }
  }
}

