#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <math.h>

// Wi-Fi credentials
#define WIFI_SSID "MyHomeWiFi"
#define WIFI_PASSWORD "mypassword123"

// Firebase credentials
#define API_KEY "AIzaSyBGGiCIob4h4Nwm-d7B0wQn3dmYLbJ-T-U"
#define DATABASE_URL "https://saferide-50fb4-default-rtdb.firebaseio.com"
#define USER_EMAIL "saferideuser@gmail.com"
#define USER_PASSWORD "mypassword456"

// GPS setup
SoftwareSerial gpsSerial(16, 17); // RX, TX
TinyGPSPlus gps;

// Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Distance check
bool initialSet = false;
double initialLat, initialLon;
#define DISTANCE_THRESHOLD_METERS 300.0

// Haversine formula to calculate distance between two GPS points
double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
  const double R = 6371000; // Earth radius in meters
  double dLat = radians(lat2 - lat1);
  double dLon = radians(lon2 - lon1);
  double a = sin(dLat / 2) * sin(dLat / 2) +
             cos(radians(lat1)) * cos(radians(lat2)) *
             sin(dLon / 2) * sin(dLon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return R * c;
}

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");

  // Firebase setup
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());

    if (gps.location.isUpdated()) {
      double currentLat = gps.location.lat();
      double currentLon = gps.location.lng();

      Serial.print("📍 Current Location: ");
      Serial.print(currentLat, 6);
      Serial.print(", ");
      Serial.println(currentLon, 6);

      if (!initialSet) {
        initialLat = currentLat;
        initialLon = currentLon;
        initialSet = true;
        Serial.println("📌 Initial location saved.");
      } else {
        double distance = calculateDistance(initialLat, initialLon, currentLat, currentLon);
        Serial.print("📏 Distance from start: ");
        Serial.print(distance);
        Serial.println(" meters");

        if (distance > DISTANCE_THRESHOLD_METERS) {
          Serial.println("🚨 Movement Alert! Bike moved > 300 meters!");

          // Send alert to Firebase
          if (Firebase.RTDB.setString(&fbdo, "/bike/distanceAlert", "Bike moved over 300m!")) {
            Serial.println("✅ Distance alert sent to Firebase");
          } else {
            Serial.println("❌ Firebase error: " + fbdo.errorReason());
          }

          delay(10000); // wait to avoid spamming alerts
        }
      }
    }
  }

  delay(1000); // 1-second loop
}
