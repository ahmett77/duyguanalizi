#include <Arduino.h>


#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#define USE_SERIAL Serial
WiFiMulti wifiMulti;

#include <DHTesp.h>
 
#define DHTpin 15    //D15 of ESP32 DevKit
DHTesp dht;
#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN         22        
#define SS_PIN          5         
MFRC522 mfrc522(SS_PIN, RST_PIN); 
 
String nem = "http://api.thingspeak.com/update?api_key=8BAOE1K953PNDZX1&field1=";
String sicaklik = "http://api.thingspeak.com/update?api_key=XXHCQJKXHGMGLUOG&field1=";
String su = "http://api.thingspeak.com/update?api_key=1PYII6KALJ9NQ5TR&field1=";

void setup() {

    USE_SERIAL.begin(115200);
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();
    delay(2000);
     while((wifiMulti.run() != WL_CONNECTED)){
      wifiMulti.addAP("furkan3", "SekizTaneSekiz88888888");
      USE_SERIAL.println("Connecting Wifi...");
      
    delay(2000);
    }
    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }
    
   
    SPI.begin(); // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522
    mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
    USE_SERIAL.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

    dht.setup(DHTpin, DHTesp::DHT11);
    delay(2000);

     
  xTaskCreate(taskThree,"TaskThree",5000,NULL,1,NULL); 
  xTaskCreate(taskOne,"TaskOne",5000,NULL,2,NULL); 

  xTaskCreate(taskTwo,"TaskTwo",5000,NULL,3,NULL); 

  USE_SERIAL.println("BAŞLIYORUZ...");
}

 
void loop() {
  
}
void taskThree(void * parameter ){
  while(true){

    if ( mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
              mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
              USE_SERIAL.print("ID Numarasi: ");
              String giden = "";
              for(int sayac = 0; sayac < 4; sayac++){
                USE_SERIAL.print(mfrc522.uid.uidByte[sayac]);
                USE_SERIAL.print(" ");
                giden = giden + String(mfrc522.uid.uidByte[sayac]); 
              }
              USE_SERIAL.println("");
              

              if((wifiMulti.run() == WL_CONNECTED)) {
                    
                    String urlimiz = "http://furkansandal.com/gzs/veriekle.php?hangisi=4&kart_id=" + giden;
                    HTTPClient http;
                    USE_SERIAL.print("[HTTP] begin...\n");
                    http.begin(urlimiz); //HTTP
                    USE_SERIAL.print("[HTTP] GET...\n");
                    
                    int httpCode = http.GET();
                    if(httpCode > 0) {
                        USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
                        if(httpCode == HTTP_CODE_OK) {
                            String payload = http.getString();
                            USE_SERIAL.println(payload);
                        }
                    } else {
                        USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
                    }
            
                    http.end();
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                  }
      
      
    }
}
 
}
void taskOne( void * parameter )
{
    while(true){
        if((wifiMulti.run() == WL_CONNECTED)) {
          for(int i = 0; i < 2; i++){
            String giden;
              if(i == 0){
                float temperature = dht.getTemperature();
                 giden = sicaklik + String(temperature);
                USE_SERIAL.println("Sıcaklık İçin İşlem Yapıyorum....");
                }else{
                    USE_SERIAL.println("Nem İçin İşlem Yapıyorum....");
                    float humidity = dht.getHumidity();
                     giden = nem + String(humidity);
                  }
                
                HTTPClient http;
                USE_SERIAL.print("[HTTP] begin Su...\n");
                http.begin(giden); //HTTP
                USE_SERIAL.print("[HTTP] GET Su...\n");
                
                int httpCode = http.GET();
                if(httpCode > 0) {
                    USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
                    if(httpCode == HTTP_CODE_OK) {
                        String payload = http.getString();
                        USE_SERIAL.println(payload);
                    }
                } else {
                    USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
                }
        
                http.end();
                vTaskDelay(1000 / portTICK_PERIOD_MS);
              }
              vTaskDelay(1000 / portTICK_PERIOD_MS);
            }else{
             vTaskDelay(1000 / portTICK_PERIOD_MS);
          }
    }
    }


void taskTwo( void * parameter)
{
    while(true){
        if((wifiMulti.run() == WL_CONNECTED)) {
          int su_analog = analogRead(A0);
          USE_SERIAL.print("Su Degeri : ");
          USE_SERIAL.println (su_analog);
          if(su_analog > 100){
            su_analog = 1;
            }else{
              su_analog = 0;
            }
          String giden = su + String(su_analog);
          HTTPClient http;
          USE_SERIAL.print("[HTTP] begin...\n");
          http.begin(giden); //HTTP
          USE_SERIAL.print("[HTTP] GET...\n");
          
          int httpCode = http.GET();
          if(httpCode > 0) {
              USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
              if(httpCode == HTTP_CODE_OK) {
                  String payload = http.getString();
                  USE_SERIAL.println(payload);
              }
          } else {
              USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
          }
  
          http.end();
          vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/*
void loop() {
    // wait for WiFi connection
    if((wifiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://www.howsmyssl.com/a/check", ca); //HTTPS
        http.begin("http://example.com/index.html"); //HTTP

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(5000);
}
*/
