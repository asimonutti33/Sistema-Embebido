#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <StreamString.h>
#include <Arduino.h>
#include <ArduinoJson.h>
const char* ssid = "UNRaf_Libre";          //SSID del router.
const char* password = "unraf2021";                //Contraseña del router.
unsigned long channelID = 2216558;                //ID del canal
const char* WriteAPIKey = "GAMEI4H9T7R326A2";     //Write API Key del canal.
WiFiClient cliente;
char c;

void setup() {
  Serial.begin(115200); // Inicia la comunicación serial del ESP8266
  WiFi.begin(ssid,password); // Inicia la conexion wifi
  ThingSpeak.begin(cliente); // Inicia el cliente de ThingSpeak
}

void loop() {
   if (Serial.available()) {  // Si hay datos disponibles para leer desde el Arduino Uno
    String jsonString = Serial.readStringUntil('\n'); // Leer la cadena JSON recibida
    StaticJsonDocument<64> doc;
    DeserializationError error = deserializeJson(doc, jsonString); // Crear un objeto JSON y parsear la cadena JSON
    // Verificar si el parsing fue exitoso
    if (error) {
      Serial.print("Error parsing JSON: ");
      Serial.println(error.c_str());
    } else {
      // Obtener el valor float del objeto JSON
      float receivedData = doc["value"];
      Serial.println(receivedData);
      ThingSpeak.setField (1,receivedData); //Escribe el dato en el campo a mostrar en ThingSpeak
      ThingSpeak.writeFields(channelID,WriteAPIKey); //Envia el dato a la nube.
      delay(2000);
    }
  }
}
   
  
  





