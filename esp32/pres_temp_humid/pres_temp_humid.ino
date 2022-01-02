#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA 1013

Adafruit_BME280 bme; // I2C

/*
 * BME280 ESP32
 * SCK (SCL Pin) GPIO 22
 * SDI (SDA pin) GPIO 21
 */


unsigned long delayTime = 1000;

void setup() {
  Serial.begin(9600);
  Serial.println("hello");
  Serial.println(F("BME280 test"));

  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    //while (1);
  }

  Serial.println("-- Default Test --");
  Serial.println();
}


void loop() { 
  printValues();
  delay(delayTime);
}

void printValues() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}
