#include <MicroGear.h>
#include <WiFi.h>
#include "Arduino.h"
#include "WiFiMulti.h"
#include "HTTPClient.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <iostream>
#include <esp_bt_device.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

int stat1 = 0;

uint32_t value = 0;
static BLEAddress *pServerAddress;

float txValue = 0;
int i = 0;
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// constants won't change. They're used here to 
// set pin numbers:

#define ledPin  2        // the number of the LED pin
const char* ssid     = "Miru";
const char* password = "55555555";

#define APPID   "tahewkaw"
#define KEY     "WKNyyjMOpphlDgU"
#define SECRET  "GObXnTHztwfMusuTw2viMhs8R"

#define ALIAS   "A_token"
#define TargetWeb "W_token"

WiFiClient client;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) 
{
  Serial.print("Incoming message --> ");
  Serial.print(topic);
  Serial.print(" : ");
  char strState[msglen];
  for (int i = 0; i < msglen; i++) 
  {
    strState[i] = (char)msg[i];
    Serial.print((char)msg[i]);
  }
  Serial.println();

  String stateStr = String(strState).substring(0, msglen);

  if(stateStr == "ON") 
  {
    digitalWrite(ledPin, LOW);
    microgear.chat(TargetWeb, "ON");
    stat1 = 1;
  } 
  else if (stateStr == "OFF") 
  {
    digitalWrite(ledPin, HIGH);
    microgear.chat(TargetWeb, "OFF");
    stat1 = 2;
  }
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) 
{
  Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      std::cout << "---Device Connecting---" << "\n";
      BLEDevice::startAdvertising();
    };


    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      std::cout << "---Device Disconnecting---" << "\n";

    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");

        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
        }

        Serial.println();
        Serial.println("*********");
      }
    }
};



WiFiMulti WiFiMulti;

void setup() 
{
    /* Event listener */
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(CONNECTED,onConnected);

    Serial.begin(115200);
    Serial.println("Starting...");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
       delay(250);
       Serial.print(".");
    }

    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    microgear.init(KEY,SECRET,ALIAS);
    microgear.connect(APPID);

    pinMode(ledPin,OUTPUT);
    digitalWrite(ledPin,HIGH); // Turn off LED
    
    for (uint8_t t = 3; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  WiFiMulti.addAP("nn", "12345678"); // ssid , password
//  WiFiMulti.addAP("w85", "85858585"); // ssid , password
  randomSeed(50);

  BLEDevice::init("COM-MA-DY-1");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop() 
{
//  microgear.loop();
  if(microgear.connected()){   
    microgear.loop();
    Serial.println("connect...");
    if(stat1 == 1){
      
    if ((WiFiMulti.run() == WL_CONNECTED)) {
//      if(stat1 == 1){
        HTTPClient http;


    //int temp = random(25, 35);
        Serial.println("tokennnnnn");
        if (deviceConnected || !oldDeviceConnected) {
        
          char txString[8]; // make sure this is big enuffz
          dtostrf(txValue, 1, 2, txString); // float_val, min_width, digits_after_decimal, char_buffer
          
          int generated = 0;
          std::string text;
    
          Serial.print("*** Send Key: ");
          while (generated < 12)
          {
            byte randomValue = random(0, 26);
            char letter = randomValue + 'a';
            if (randomValue > 26)
              letter = (randomValue - 26) ;
              Serial.print(letter);
              generated ++;
              text += letter;
          }
          pCharacteristic->setValue(text); // Sending a test message
          pCharacteristic->notify(); // Send the value to the app!
          Serial.println(" ***");
          
//          String url = "http://192.168.64.2/myapp/add.php?esp32_token=" + String(text.c_str());
          String url = "http://192.168.137.1/AppBLE/BLE/add.php?esp32_name=COM-MA-DY-1&room=1641&esp32_token=" + String(text.c_str());
          Serial.println(url);
          http.begin(url); //HTTP
    
          int httpCode = http.GET();
          if (httpCode > 0) {
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            if (httpCode == HTTP_CODE_OK) {
              String payload = http.getString();
              Serial.println(payload);
            }
          } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
          }
    
         
            delay(7000);
            
          
          
      }
      if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        oldDeviceConnected = deviceConnected;
      }
  
      // connecting
      if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
  
        //Serial.println("connecting");
  
      }
  
      http.end();

    }
      else{
        Serial.println("เข้า9 ไม่เชื่อมwifi");
        Serial.println(stat1);  
      }
    }
    else{
      Serial.println("เข้า8 stat1 != 1");
      Serial.println(stat1);
      i=0;
    }
  }
    
 
  else
  {
    Serial.println(stat1);
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }
  delay(1000);
}
