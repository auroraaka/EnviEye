/* 
Atmospheric Air Analyzer
Environa
Akarsh Aurora
11/13/20
*/
 
#include <ArduinoJson.h>
#include <DHT.h>

// Gas Sensor Pins
#define F2600 A1
#define MQ135 A2
#define F2602 A3
#define DHTPIN A4
    
// 2600 Gases
float A_CO = 1.144997421;
float B_CO = -0.21687423;

float A_CH4 = 1.05777824;
float B_CH4 = -0.0795645;

// 2602 Gases
float A_NH3 = 0.92372844;
float B_NH3 = -0.291578925;

float A_H2S = 0.38881036;
float B_H2S = -0.35010059;

float A_VOC = 0.3220722;
float B_VOC = -0.6007520;

// MQ-135 Gases
float A_CO2 = 112.89808; 
float B_CO2 = -2.868463517;  

float A_NOx = 34.69756084;  
float B_NOx = -3.422829698; 

// DHT Temperature and Humidity
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE, 6);
float H, T, F;

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  dht.begin();
}

void loop() {

  // Setting Pin 13 to GND
  digitalWrite(13, LOW);
  
  // DHT11 Temperature and Humidity Measurements
  DetectTemperatureHumidity ();
  
  // Figaro 2600 Gas Concentrations (CO and CH4)
  float ratio_F2600 = CalculateRatio(F2600, CalculateK_2600(), 400);
  float ppm_CO = A_CO * pow(ratio_F2600, B_CO) * 0.1;
  float ppm_CH4 = A_CH4 * pow(ratio_F2600, B_CH4) * 0.1;

  // MQ-135 Gas Concentrations (CO2 and NOx)
  float ratio_MQ135 = CalculateRatio(MQ135, CalculateK_MQ135(), 24);
  float ppm_CO2 = analogRead(MQ135) + 325;
  float ppm_NOx = A_NOx * pow(ratio_MQ135, B_NOx) * 10;

  // Figaro 2602 Gas Concentrations (VOC, H2S, and NH3)
  float ratio_F2602 = CalculateRatio(F2602, CalculateK_2602(), 225);
  float ppm_VOC = A_VOC * pow(ratio_F2602, B_VOC) * 0.001 * 1000000/2;
  float ppm_H2S = A_H2S * pow(ratio_F2602, B_H2S) * 0.001;
  float ppm_NH3 = A_NH3 * pow(ratio_F2602, B_NH3) * 0.001;  
  
  // Upload data to ThingSpeak Channel
  DynamicJsonBuffer jBuffer; 
  JsonObject& root = jBuffer.createObject();

  root["CO2"] = ppm_CO2;
  root["CO"] = ppm_CO;
  root["CH4"] = ppm_CH4;
  root["NOx"] = ppm_NOx;
  root["NH3"] = ppm_NH3;
  root["H2S"] = ppm_H2S;
  root["VOC"] = ppm_VOC;
  root["Temperature"] = T;
  root["Humidity"] = H;
   
  root.prettyPrintTo(Serial);
  Serial.println("");

  // ThingSpeak Delay
  delay(10000);
}

// Calculate Rs/Ro
float CalculateRatio (int SensorPin, float K, float Rs0){
  float VRL;
  float Rs;
  float RL = 10;
  float Ro;
  float ratio;
  VRL = analogRead(SensorPin)*(5.0/1023.0);
  Rs = (5.0/VRL - 1)*RL;
  Ro = Rs0/K;
  ratio = Rs/Ro;
  return ratio;
}


float CalculateK_2600 (){
  float K_2600;
  K_2600 = (-6.073E-7)*pow(T, 4) + (4.218E-5)*pow(T, 3) + (-8.710E-5)*pow(T, 2) + (-0.055)*T + (1.901);
  return K_2600;
}

float CalculateK_2602 (){
  float K_2602;
  K_2602 = (-1.519E-7)*pow(T, 4) + (1.796E-5)*pow(T, 3) + (-5.415E-4)*pow(T, 2) + (-0.024)*T + (1.585);
  return K_2602;
}

float CalculateK_MQ135 (){
  float K_MQ135;
  K_MQ135 = (-1.028E-7)*pow(T, 4) + (3.911E-6)*pow(T, 3) + (4.215E-4)*pow(T, 2) + (-0.027)*T + (1.318);
  return K_MQ135; 
}

float DetectTemperatureHumidity (){
  H = dht.readHumidity();
  T = dht.readTemperature();
  F = dht.readTemperature(true);
  if (isnan(H) || isnan(T) || isnan(F)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
}
