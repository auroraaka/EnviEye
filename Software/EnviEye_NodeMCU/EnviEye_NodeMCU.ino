#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "ThingSpeak.h"

SoftwareSerial mySerial(D5, D6);
WiFiClient client; // Creating WiFiClient Object

//ThingSpeak Channel's API Keys
unsigned long myChannelNumber = // Enter ThingSpeak Channel ID
const char * myWriteAPIKey = // Enter ThingSpeak Channel Key

// Add WIFI credentials here

char* WIFI_SSID[] = {"WIFI_SSID_1", "WIFI_SSID_2", "WIFI_SSID_3"}; // List WIFI SSIDs
char* WIFI_PASSWORD[] = {"WIFI_PASSWORD_1", "WIFI_PASSWORD_2", "WIFI_PASSWORD_3"}; // List WIFI Passwords

void WIFI_SEARCH() {
  int SSID_COUNT = 0;
  do {
    Serial.println("Attempting connection to WIFI network: " + String(WIFI_SSID[SSID_COUNT]));
    WiFi.begin(WIFI_SSID[SSID_COUNT], WIFI_PASSWORD[SSID_COUNT]);
    int WIFI_TIMEOUT_COUNT = 0;
    while (WiFi.status() != WL_CONNECTED && WIFI_TIMEOUT_COUNT < 100) { 
      delay(100);
      Serial.println(".");
      ++WIFI_TIMEOUT_COUNT;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to WIFI network: " + String(WIFI_SSID[SSID_COUNT]));
      Serial.println(WiFi.localIP());
    }
    ++SSID_COUNT;
  }
  while (WiFi.status() != WL_CONNECTED);
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  WIFI_SEARCH();
  ThingSpeak.begin(client);
}

void loop() {

  // Check WiFi status
  while (mySerial.available())
  {
    const size_t capacity = JSON_OBJECT_SIZE(8) + 100;
    DynamicJsonBuffer jsonBuffer(capacity);
    JsonObject& root = jsonBuffer.parseObject(mySerial);
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }

    float CO2 = root["CO2"];
    float CO = root["CO"];
    float CH4 = root["CH4"];
    float NOx = root["NOx"];
    float NH3 = root["NH3"];
    float H2S = root["H2S"];
    float VOC = root["VOC"];
    float Temperature = root["Temperature"];

    Serial.print(CO2, D5);  Serial.print(",");
    Serial.print(CO, D5);  Serial.print(",");
    Serial.print(CH4, D5);  Serial.print(",");
    Serial.print(NOx, D5);  Serial.print(",");
    Serial.print(NH3, D5);  Serial.print(",");
    Serial.print(H2S, D5); Serial.print(",");
    Serial.print(VOC, D5); Serial.print(",");
    Serial.println(Temperature, D5); 
    
    // Send gas concentration data to ThingSpeak channel
    ThingSpeak.setField(1, CO2);
    ThingSpeak.setField(2, CO);
    ThingSpeak.setField(3, CH4);
    ThingSpeak.setField(4, NOx);
    ThingSpeak.setField(5, NH3);
    ThingSpeak.setField(6, H2S);
    ThingSpeak.setField(7, VOC);
    ThingSpeak.setField(8, Temperature);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  }
}
