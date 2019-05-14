#include <CasaIoTESP8266.h>

namespace {
  const char * WIFISSID = "<WiFi name>";
  const char * PASSWORD = "<WiFi password>";
}

Ubidots client;

void setup() {
  Serial.begin(115200);
  client.wifiConnection(WIFISSID, PASSWORD);
}

void loop() {
  client.readData();
  delay(1000);
}