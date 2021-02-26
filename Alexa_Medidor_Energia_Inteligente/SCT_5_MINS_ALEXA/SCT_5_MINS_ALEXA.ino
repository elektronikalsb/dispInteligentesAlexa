#include "TRIGGER_WIFI.h"               /*Includes ESP8266WiFi.h and WiFiClientSecure.h, just have these two libraries downloaded before*/
#include "TRIGGER_GOOGLESHEETS.h"       /*Library file for Google Sheets, has to be used after Wi-Fi Client Secure declaration, here everything is in Trigger_WIFI.h, so using it after Trigger_WIFI.h*/ 

#include "NTPClient.h"
#include "WiFiUdp.h"

#include "EmonLib.h"
// Include Emon Library

#include <ESP8266WiFi.h>
#include <Espalexa.h>
Espalexa esp_alexa;

EnergyMonitor emon1;
float tiempo_anterior, espera;
// Create an instance

int VoltajeRed=230;

/**********Google Sheets Definations***********/
char column_name_in_sheets[ ][20] = {"HORA","POTEN","IRMS"};                        /*1. The Total no of column depends on how many value you have created in Script of Sheets;2. It has to be in order as per the rows decided in google sheets*/
String Sheets_GAS_ID = "AKfycbzgv4IZU7i0x-EzRYLXDIDBpwYj9f4QURoGupH9wg";                                         /*This is the Sheets GAS ID, you need to look for your sheets id*/
int No_of_Parameters = 3;                                                                /*Here No_of_Parameters decides how many parameters you want to send it to Google Sheets at once, change it according to your needs*/
/*********************************************/

const long utcOffsetInSeconds = 3600;
int hora, minutos, segundos, resto;
unsigned long tiempo;
boolean UnaVez = 0;
boolean SCT = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() 
{
  Serial.begin(9600);
  tiempo_anterior = millis();
  espera = millis();

  emon1.current(A0, 25);             // Current: input pin, calibration.
  
  while (!Serial);

  WIFI_Connect("cursoarduino1","cursoarduino1");                                                     /*Provide you Wi-Fi SSID and password to connect to Wi-Fi*/
  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters );  /*Sets the column name for Google Sheets, the GAS ID, and the No of Parameter we want to send*/   
  
  esp_alexa.addDevice("SENSOR",foco);
  esp_alexa.begin();

 
  timeClient.begin();     
}

void loop() 
{
  
  if (millis() - tiempo_anterior > 2000)
     {
      double Irms = emon1.calcIrms(1480);  // Calculate Irms only
      double Potencia = Irms*VoltajeRed;
      Serial.println("LA POTENCIA ES.");
      Serial.println(Potencia);
      Serial.println("LA INTENSIDAD ES");
      Serial.println(Irms);

      timeClient.update();
      hora=timeClient.getHours();
      minutos=timeClient.getMinutes();
      segundos=timeClient.getSeconds();
      tiempo=hora*10000+minutos*100+segundos;
      //Serial.println(tiempo);
     
      resto = minutos % 5;
      
      Serial.println("ESTAMOS EN EL MINUTO:     ");
      Serial.println(minutos);
      Serial.println("EL RESTO ES:     ");
      Serial.println(resto);

      if (millis() - espera > 20000) // Q no haga nada los primeros 20 Seg. por que manda mal la informaacion al arrancar el ESP8266.
      {

        if (((resto == 0) && (UnaVez == 0)) or (SCT == 1))
        {

          Serial.println("ES MULTIPLO DE 5");
      
          float a = tiempo, b = Potencia, c = Irms;                           /*Demo values that has to be sent to google sheets, you can use sensor values*/     
      
          Data_to_Sheets(No_of_Parameters,  a,  b, c);         /*1. This function accepts multiple float parameter, here No_of_Parameters decides how many parameters you want to send to Google Sheets; 2. The values sent should be in order as per the column in Google Sheets*/
      
          UnaVez = 1;
          SCT = 0;  
      
        }
        if (resto != 0)
        {
       
          Serial.println("NO ES MULTIPLO DE 5"); 
          UnaVez = 0;
      
        }
      }
      Serial.println(); /*10 Sec Delay, Here 10 second delay is just used so that we can see the data pushed to sheets one by one
                                                        There is nothing like a minimum delay between two data push*/
      tiempo_anterior = millis();
                                                        
      }

    esp_alexa.loop();  
      
 }

     void foco(uint8_t Mandar)
{
 if (Mandar)
 {
  SCT = 1;  
 }
 else
 {
  SCT = 0;    
 }
}
