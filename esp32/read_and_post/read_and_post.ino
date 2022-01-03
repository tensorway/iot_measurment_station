#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>

#include <Adafruit_BMP280.h>
#define PHOTORESISTOR_PIN 34
#define LED_BUILTIN 2
#define SOIL_SENSOR_PIN 33
#define PUMP_PIN 32

// wifi part
const char* ssid = "D308DE";
const char* password =  "2mevo0asbs";
const char* serverName = "http://192.168.0.22:5000/post_data"; //get-sensor";

// sensors part
Adafruit_BMP280 bmp = Adafruit_BMP280() ; // use I2C interface
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();
int soil_threshold = 600;

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
bool pumpin = false;
int pumpin_delta = 100;


void setup() {
  Serial.begin(115200);
  while (!Serial) delay(100);
delay(1000);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(PHOTORESISTOR_PIN, INPUT);
  pinMode(SOIL_SENSOR_PIN, INPUT);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
  
  if (!bmp.begin()){
    Serial.println("Could not initialize the pressure and temperature sensors");
    while(true){
      digitalWrite(LED_BUILTIN, 1);
      delay(500);
      digitalWrite(LED_BUILTIN, 0);
      delay(500); 
      Serial.println("error: Could not initialize the pressure and temperature sensors");     
    }
  }
    /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X1,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  bmp_temp->printSensorDetails();


  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(PHOTORESISTOR_PIN, INPUT);
  pinMode(SOIL_SENSOR_PIN, INPUT);
  Serial.print("phtosen");
  Serial.println(analogRead(PHOTORESISTOR_PIN));
}

void loop() {
  //Send an HTTP POST request every 5 minutes
  if ((millis() - lastTime) > timerDelay) {
    
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      sensors_event_t temp_event, pressure_event;
      bmp_temp->getEvent(&temp_event);
      bmp_pressure->getEvent(&pressure_event);
    
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      // Data to send with HTTP POST
      String httpRequestData  = "temperature="    + String(temp_event.temperature);
             httpRequestData += "&pressure="      + String(pressure_event.pressure);
             httpRequestData += "&brightness="    + String(4095-analogRead(PHOTORESISTOR_PIN));
             httpRequestData += "&humidity_soil=" + String(analogRead(SOIL_SENSOR_PIN));
            
      // Send HTTP POST request
      Serial.print("sending: ");
      Serial.print(httpRequestData);
      Serial.print("    HTTP Response code: ");
      digitalWrite(LED_BUILTIN, 1);
      Serial.println(http.POST(httpRequestData));
      digitalWrite(LED_BUILTIN, 0);
      http.end();
    }
    
    else  Serial.println("WiFi Disconnected");
    lastTime = millis();
  }
  if ((analogRead(SOIL_SENSOR_PIN) > soil_threshold) && pumpin || 
        analogRead(SOIL_SENSOR_PIN) > soil_threshold+pumpin_delta){
    digitalWrite(PUMP_PIN, 1);
    digitalWrite(LED_BUILTIN, 1);
    pumpin = true;
  }
  else{
    digitalWrite(PUMP_PIN, 0);
    digitalWrite(LED_BUILTIN, 0); 
    pumpin = false;   
  }

}
