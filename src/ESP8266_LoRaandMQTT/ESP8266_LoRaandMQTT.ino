/*********
  Modified from the examples of the Arduino LoRa library
  More resources: https://randomnerdtutorials.com
*********/

#include <SPI.h>
#include <LoRa.h>

#define CAYENNE_PRINT Serial  // Comment this out to disable prints and save space
#include <CayenneMQTTESP8266.h>

//define the pins used by the transceiver module
//numbers depends of GPIO number, not digital number
#define ss 15
#define rst 16
#define dio0 4

// Modify this with your cayenne my devices info
char* username = "your_username";
char* password = "your_password";
char* clientID = "your_clientID";

// Modify this with your WiFi info
char ssid[] = "your_WiFi_name";
char wifiPassword[] = "your_WiFi_password";
bool bitTs = 0;
long lastMsg = 0;
long timeToSend = 2 * 1000; // Time betweeen MQQT messages

#define VIRTUAL_CHANNEL 1
#define SENSOR_PIN 10

bool SOSflag = false;

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver");

  //Setting up Cayenne communication
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  
  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(433E6)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  // LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}

void loop() { 
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    SOSflag = true;
    Cayenne.virtualWrite(VIRTUAL_CHANNEL, SOSflag, TYPE_DIGITAL_SENSOR, UNIT_DIGITAL);
    Serial.println("SOS!!!");
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData); 
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
    delay(3000);
    lastMsg = millis();
  } else {
    if((millis() - lastMsg) > timeToSend){
      lastMsg = millis();
      SOSflag = false;
      Cayenne.virtualWrite(VIRTUAL_CHANNEL, SOSflag, TYPE_DIGITAL_SENSOR, UNIT_DIGITAL);
      Serial.println("Everything OK");
    }
  }
}
