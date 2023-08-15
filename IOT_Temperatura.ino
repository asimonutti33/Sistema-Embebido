#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <StreamString.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <Wire.h>

#include "DHT.h"



#define pin1 2       //Pin del DHT11.


const char* ssid = "UNRaf_Libre";                        //SSID de vuestro router.
const char* password = "unraf2021";                //Contraseña de vuestro router.

unsigned long channelID = 2216558;                //ID de vuestro canal.
const char* WriteAPIKey = "GAMEI4H9T7R326A2";     //Write API Key de vuestro canal.

WiFiClient cliente;

DHT dht1(pin1, DHT11);    //El azul.
LiquidCrystal_I2C lcd(0x27,16,2);
float temp;
int LedR = 14;
int LedV = 12;
int LedA = 16;
const int OnOff = 13;
int Alarma = 15;
int Enfriar = 3; 
float tempControl = 0;
const int timeThreshold = 150;
const int intPin = 2;
volatile int ISRCounter = 0;
int counter = 0;
long startTime = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Test de sensores:");

  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi conectado!");

  ThingSpeak.begin(cliente);

  dht1.begin();
  pinMode(LedR, OUTPUT);
  pinMode(LedV, OUTPUT);
  pinMode(LedA, OUTPUT);
  //pinMode(OnOff, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(OnOff), debounceCount, RISING);
  pinMode(Alarma, INPUT);
  pinMode(Enfriar, INPUT);
  lcd.begin(16, 2); 
  lcd.backlight();
}

void loop() {
temp = dht1.readTemperature();
//int recep = Serial.read();
//Serial.println(recep);
tempControl = temp;
   if(temp >= 0 && temp < 25){
    leerdht1();
    digitalWrite(LedV, HIGH);
    digitalWrite(LedR, LOW);
    delay(2000);
  }
  
  if(temp >= 25){
    leerdht1();
    digitalWrite(LedV, LOW);
    digitalWrite(LedR, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Advertencia");
    lcd.setCursor(12, 0);
    lcd.print(temp);
    lcd.setCursor(0, 1);
    lcd.print("Temp Critica");
    delay(2000);
  }
  delay(2000);
  temp = dht1.readTemperature();
  if(temp != tempControl){
  ThingSpeak.writeFields(channelID,WriteAPIKey);
  Serial.println("Datos enviados a ThingSpeak!");
  delay(8000);
  }
if (counter != ISRCounter) {
    counter = ISRCounter;
    if(digitalRead(OnOff) == HIGH){
    fullStop();
    }
}
}

void leerdht1() {
  float t1 = dht1.readTemperature();
    while (isnan(t1)){
    Serial.println("Lectura fallida en el sensor DHT11, repitiendo lectura...");
    delay(2000);   
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperatura: ");
  lcd.setCursor(0, 1);
  lcd.print(t1);
  Serial.print("Temperatura DHT11: ");
  Serial.print(t1);
  Serial.println(" ºC.");

  Serial.println("-----------------------");

  ThingSpeak.setField (1,t1);
  
}

void fullStop(){
  lcd.begin(16, 2);
  lcd.clear();
  lcd.noBacklight();
  ESP.deepSleep(5e6);
}

ICACHE_RAM_ATTR void debounceCount()
{
  if (millis() - startTime > timeThreshold)
  {
    ISRCounter++;
    startTime = millis();
  }
}



