#include <TinyGPS++.h>
#include <HardwareSerial.h>

TinyGPSPlus gps;
HardwareSerial gpsSerial(1); // UART1: RX = 4, TX = 2

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 4, 2); // GPS RX=4, TX=2

  Serial.println("GPS Module Test Started");
}

void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
    if (gps.location.isUpdated()) {
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
    }
  }
}

