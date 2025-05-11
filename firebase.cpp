#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// 🔐 Wi-Fi Credentials
#define WIFI_SSID "MyHomeWiFi"                  // ← Change to your WiFi name
#define WIFI_PASSWORD "mypassword123"           // ← Change to your WiFi password

// 🔑 Firebase Credentials (from your Firebase config)
#define API_KEY "AIzaSyBGGiCIob4h4Nwm-d7B0wQn3dmYLbJ-T-U"
#define DATABASE_URL "https://saferide-50fb4-default-rtdb.firebaseio.com/"

// 👤 Firebase Authentication (from Users tab in Firebase)
#define USER_EMAIL "saferideuser@gmail.com"     // ← Your registered email
#define USER_PASSWORD "mypassword456"           // ← Your Firebase user password

// 🔧 Firebase Objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);

  // 📡 Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected!");

  // ⚙️ Configure Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // 🔔 Send Alert to Firebase Realtime Database
  if (Firebase.RTDB.setString(&fbdo, "/bike/alert", "Unauthorized access detected")) {
    Serial.println("✅ Alert sent to Firebase!");
  } else {
    Serial.println("❌ Error: " + fbdo.errorReason());
  }

  delay(10000);  // 🔁 Send alert every 10 seconds (for testing)
}
