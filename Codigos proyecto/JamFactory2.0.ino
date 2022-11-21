/****************************************
 * Include Libraries
 ****************************************/
#include "Ubidots.h"
#include <DHT.h>
#include <DHT_U.h>

/****************************************
 * Define Instances and Constants
 ****************************************/

const char* UBIDOTS_TOKEN = "BBFF-qde03o6mT1M3vyGxr76y6rWoCZUxD0";  // Put here your Ubidots TOKEN_UBIDOST_ESTEBAN
//const char* UBIDOTS_TOKEN = "BBFF-V73gYIzeZpRxhQ4EVfCxNmbytqr2v0";  // Put here your Ubidots TOKEN_UBIDOST_VALENTINA
const char* WIFI_SSID = "FAMILIA GOMEZ";      // Put here your Wi-Fi SSID
const char* WIFI_PASS = "AMIJOLISO.";      // Put here your Wi-Fi password

//SEND
const char* DEVICE_LABEL = "JamFactory"; //Replace with your device label
const char* SEND_VARIABLE_LABEL = "temperatura";   //(Temperatura) Put here your Variable label to which data  will be published
const char* SEND_VARIABLE_LABEL2 = "humedad";   //(Humedad) Put here your Variable label to which data  will be published
const char* SEND_VARIABLE_LABEL3 = "peso";   //(Peso) Put here your Variable label to which data  will be published
const char* SEND_VARIABLE_LABEL4 = "color";   //(Colores) Put here your Variable label to which data  will be published

//GET
const char* DEVICE_LABEL_TO_RETRIEVE_VALUES_FROM = "JamFactory";  // Replace with your device label
const char* VARIABLE_LABEL_TO_RETRIEVE_VALUES_FROM = "led";       // Replace with your variable label

/************* VARIABLES ************/
//HUMIDITY AND TEMPERATURE
int analogPin = 4;
int temperatura;
int humedad; 

//COLORS
float color=39;

//WEIGHT
#define FORCE_SENSOR_PIN 34 // ESP32 pin GIOP39  the FSR and 10K pulldown are connected to A0
int w2=36;
int peso=0;

//LED
const uint8_t LED = 18; // Pin used to write data based on 1's and 0's coming from Ubidots

Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);
// Ubidots ubidots(UBIDOTS_TOKEN, UBI_TCP); // Uncomment to use TCP
// Ubidots ubidots(UBIDOTS_TOKEN, UBI_UDP); // Uncomment to use UDP
DHT dht(analogPin,DHT11);
/****************************************
 * Main Functions
 ****************************************/

void setup() {
  Serial.begin(115200);
  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);
  dht.begin();
  pinMode(analogPin, INPUT);//Humedad y temperatura
  //pinMode(w, INPUT);//Peso+
  //pinMode(w2, INPUT);//Peso-
  //pinMode(colors, INPUT);//Color
  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  // ubidots.setDebug(true);  // Uncomment this line for printing debug messages
  pinMode(LED,OUTPUT);
}

void loop() {
  float value = ubidots.get(DEVICE_LABEL_TO_RETRIEVE_VALUES_FROM, VARIABLE_LABEL_TO_RETRIEVE_VALUES_FROM);//GET
  float value1 = analogRead(color);//Colores
  int analogReading = analogRead(FORCE_SENSOR_PIN);//Peso

  temperatura=dht.readTemperature();
  humedad=dht.readHumidity();

    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print("C°");    
    Serial.print("   Humedad: ");
    Serial.println(humedad);  
    
    /*peso=(1000*total)/4095;
    Serial.print("   Gramos: ");
    Serial.print(peso);
    Serial.println("g");*/
    Serial.print("The force sensor value = ");
  Serial.print(analogReading); // print the raw analog reading

  if (analogReading < 600){      // from 0 to 9
    Serial.println(" -> no pressure");
  }else if (analogReading < 1800){ // from 10 to 199
    Serial.println(" -> light touch");
  }else if (analogReading < 2900){ // from 200 to 499
    Serial.println(" -> light squeeze");
  }else if (analogReading < 3800){ // from 500 to 799
    Serial.println(" -> medium squeeze");
  }else{ // from 800 to 1023
    Serial.println(" -> big squeeze");
  }

  peso=(analogReading*80)/1282;
  Serial.print("Peso: ");
  Serial.print(peso);
  Serial.println("g");

    if(value1>80 && value1<300){
    Serial.print(" Color: ");
    Serial.print("rojo");
    }else{
    Serial.print("HONGOS!!");  
    }
    color=value1*(3.3/4095);
    Serial.print("      Bits del color: ");
    Serial.print(value1);
    Serial.print("      Voltage: ");
    Serial.println(color);

    ubidots.add(SEND_VARIABLE_LABEL, temperatura);  // TEMPERATURA
    ubidots.add(SEND_VARIABLE_LABEL2, humedad);//HUMEDAD
    ubidots.add(SEND_VARIABLE_LABEL3, peso);//PESO
    ubidots.add(SEND_VARIABLE_LABEL4, value1);//COLOR

  bool bufferSent = false;
  bufferSent = ubidots.send(DEVICE_LABEL);  // Will send data to a device label that matches the device Id

  if (bufferSent) {
    // Do something if values were sent properly
    Serial.println("Values sent by the device");
  }

// Evaluates the results obtained
  if (value != ERROR_VALUE) {
    Serial.print("Led:");
    Serial.println(value);
        if (value == 1)
      {
        digitalWrite(LED, HIGH);
      }
      else
      {
        digitalWrite(LED, LOW);
      }  
  }

  delay(1000);
}