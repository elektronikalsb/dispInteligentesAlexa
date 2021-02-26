#include <ESP8266WiFi.h>
#include <Espalexa.h>
Espalexa esp_alexa;

#define led 2
#define bombilla D0

const char* ssid = "cursoarduino1";
const char* password = "cursoarduino1";

void setup() 
{
  Serial.begin(115200);
  pinMode(led,OUTPUT); 
  pinMode(bombilla,OUTPUT);
  digitalWrite(led,HIGH); //inicia apagado
  ConectarWifi(); 
  esp_alexa.addDevice("LED",foco);
  esp_alexa.addDevice("BOMBILLA",foco2);
  esp_alexa.begin();
}

void loop()
{
  ConectarWifi();
  esp_alexa.loop();
  delay(1);
}
   
void ConectarWifi() 
{
  if(WiFi.status() != WL_CONNECTED) {
     WiFi.mode(WIFI_STA);
     WiFi.begin(ssid, password);
     Serial.println("");
     Serial.println("Connecting to WiFi");
     while(WiFi.status() != WL_CONNECTED) {
           delay(500);
           Serial.print(".");}
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());}
}

void foco(uint8_t brillo)
{
     if(brillo){
      digitalWrite(led, LOW);}
     else{
      digitalWrite(led, HIGH);}
}   
void foco2(uint8_t brillo2)
{
     if(brillo2){
      digitalWrite(bombilla, HIGH);}
     else{
      digitalWrite(bombilla, LOW);}
}   
