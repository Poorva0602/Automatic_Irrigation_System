#define BLYNK_PRINT Serial
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BlynkSimpleEsp8266.h>
#include <Blynk.h>


#define BLYNK_PRINT Serial
#define ONE_WIRE_BUS D2 
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

HTTPClient http;


char auth[] = "pL8naJucN20YLEfAcGxUuQDXtMWaMttA";        //authentication ID from Blynk app 
#define MAIN_SSID "RAHUL 4G"
#define MAIN_PASS "poorva2000"                     // WiFi password 

#define CSE_IP      "192.168.29.114"                   // system IP address
#define CSE_PORT    8080                          // port number
#define HTTPS       false
#define OM2M_ORGIN  "admin:admin"    
#define OM2M_MN     "/~/in-cse/in-name/"
#define OM2M_AE     "AE_Farmlands"                 // application entity name (AE)
#define OM2M_DATA_CONT  "Location-1/Data"         // DATA container under content instance (cnt)

#define DHTPIN D2                                //output pin of dht is connected
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
SimpleTimer timer;

void sendSensor()
{
float   h = dht.readHumidity();
float   t = dht.readTemperature();

if (isnan(h) || isnan(t)) {
 Serial.println("Failed to read from DHT sensor!");
 return;
 }  
 
  Blynk.virtualWrite(V5, h);  //V5 is for Humidity
  Blynk.virtualWrite(V6, t);  //V6 is for Temperature
}



void setup() {
Serial.begin(115200);
//delay(10);
dht.begin();

Blynk.begin(auth, MAIN_SSID, MAIN_PASS, "blynk-cloud.com", 8080);
   

dht.begin();

 timer.setInterval(1000L, sendSensor);
  Serial.begin(115200);
  
  Blynk.begin(auth, MAIN_SSID, MAIN_PASS, "blynk-cloud.com", 8080);
   sensors.begin();
}



int sensor=0;
void sendTemps()
{
sensor=analogRead(A0);
sensors.requestTemperatures();
float temp = sensors.getTempCByIndex(0); 
Serial.println(temp);
Serial.println(sensor);

Blynk.virtualWrite(V2,sensor);
delay(1000);
}


void loop() {
static int i=0; 
float h = dht.readHumidity();
float t = dht.readTemperature(true);
String data="["+String(h)+","+String(t)+"]";

String server="http://" + String() + CSE_IP + ":" + String() + CSE_PORT + String()+OM2M_MN;
http.begin(server + String() +OM2M_AE + "/" + OM2M_DATA_CONT + "/");
http.addHeader("X-M2M-Origin", OM2M_ORGIN);
http.addHeader("Content-Type", "application/json;ty=4");
http.addHeader("Content-Length", "100");

String req_data = String() + "{\"m2m:cin\": {"

  + "\"con\": \"" + data + "\","
  
  + "\"rn\": \"" + "cin"+String(i++) + "\","

  + "\"cnf\": \"text\""

  + "}}";
int code = http.POST(req_data);
http.end();
Serial.println(code);
delay(10000);
Blynk.run(); 
timer.run(); 
sendTemps();
}
