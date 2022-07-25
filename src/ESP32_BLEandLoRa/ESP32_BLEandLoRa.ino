/*

    Creted by: Sebas Pineda
    
    [BLE]
    Based on ESP32 BLE Arduino examples

    and

    [LORA]
    Based on https://randomnerdtutorials.com/esp32-lora-rfm95-transceiver-arduino-ide/

    and 
    
    [OLED]
    Based on https://randomnerdtutorials.com/esp32-built-in-oled-ssd1306/
*/

// Include needed Libraries
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include <SPI.h>
#include <LoRa.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// Define RF Band to work (MHz)
#define BAND    433E6  //you can set band here directly,e.g. 868E6,915E6

// Define UUID, you can generate yours in https://www.uuidgenerator.net/
#define SERVICE_UUID        "86b3e5ce-e7ac-4f88-8973-8ede73f59094"
#define CHARACTERISTIC_UUID "f7c5104b-c698-42c9-8e76-aa421108805b"

// Define the pins used by the transceiver module
// Use GPIO notation, NOT Digital notation. I lost a day with this :'(
#define ss 18
#define rst 14
#define dio0 26
#define dio1 35
#define dio2 34

// Define OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCL 15
#define SDA 4
#define OLED_RST 16
#define OLED_ADD 0x3C   //Check OLED or board datasheet for this value
#define Rotation 2      // 180 degree rotation
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

String strMem = "";
int counter = 0;
#define msgToSend 20

// Define flags
static boolean deviceConnected = false;
bool newSOS = false;

// Define BLE Callbacks
// CallBack when the Characteristic is changed
class sosCallback: public BLECharacteristicCallbacks {
  virtual void onRead (BLECharacteristic *pCharacteristic){
    Serial.println("[BLE] onRead");
  }
  virtual void onWrite(BLECharacteristic* pCharacteristic){
    Serial.println("[BLE] onWrite");
    newSOS = true;
  }
};
// CallBack when the CellPhone start a connection
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("[BLE] device connected");
    deviceConnected = true;
    digitalWrite(LED_BUILTIN, HIGH);
  };
  void onDisconnect(BLEServer* pServer) {
    Serial.println("[BLE] device disconnected");
    deviceConnected = false;
    digitalWrite(LED_BUILTIN, LOW);
    BLEDevice::startAdvertising();
  }
};


// Function to add messages on display
void addThisOnDisplay(String str){
  strMem = strMem + "\n\n" + str;
  display.clearDisplay();
  #ifdef Rotation
    display.setRotation(Rotation);
  #endif
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.println(strMem);
  display.display();
}
// Function to write messages on display (Clean display and strMem)
void writeThisOnDisplay(String str){
  strMem = str;
  display.clearDisplay();
  #ifdef Rotation
    display.setRotation(Rotation);
  #endif
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.println(str);
  display.display();
}


void setup() {
  // Serial
  Serial.begin(115200);  
  pinMode(LED_BUILTIN, OUTPUT);
  while (!Serial){
    Serial.println("[Serial] Serial not Working");
  }
  Serial.println("[Serial] Serial Initializing OK!");

  //OLED
  pinMode(OLED_RST,OUTPUT);
  digitalWrite(OLED_RST, LOW); // low to reset OLED
  delay(50);
  digitalWrite(OLED_RST, HIGH); // must be high to turn on OLED
  delay(50);
  Wire.begin(SDA, SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADD, false, OLED_RST)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  addThisOnDisplay("[OLED] OLED is OK!");
  Serial.println("[OLED] OLED Initializing OK!");
  
  // BLE
  BLEDevice::init("ESP32-SOS");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setCallbacks(new sosCallback());
  
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("[BLE] BLE Initializing OK!");
  addThisOnDisplay("[BLE] BLE is OK!");

  // LoRa
  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  while (!LoRa.begin(BAND)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("[LoRa] LoRa Initializing OK!");
  addThisOnDisplay("[LoRa] LoRa is OK!");

  delay(2000);
}


void loop() {
  if (deviceConnected){
    writeThisOnDisplay("Connected to a device\n\nSOS ammount: "+ String(counter));
    
    if (newSOS) {
      counter++;
      addThisOnDisplay("new SOS!");
      Serial.println("[BLE] newSos = true");
      newSOS = false;
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("[LORA] Sending message with LoRa");
      for (int i = 0; i < msgToSend; i++) {
        LoRa.beginPacket();
        LoRa.print("[LoRa] SOS required");
        LoRa.endPacket();
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
      }
      Serial.println("[LORA] Message sent");
    }   
  }
  else {
    writeThisOnDisplay("Searching devices");
    counter = 0;
  }
  delay(2000);
}
