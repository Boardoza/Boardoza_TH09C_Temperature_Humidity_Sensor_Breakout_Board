#include "TH09C.h"

TH09C th09cSensor;

void setup() {
  Serial.begin(115200);
  th09cSensor.begin();
}

void loop() {
  float temperature, humidity;

  th09cSensor.readTempHum(temperature, humidity);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C ");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(100);
}