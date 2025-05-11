#include <Wire.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// WiFi credentials
#define WIFI_SSID "MyHomeWiFi"
#define WIFI_PASSWORD "mypassword123"

// Firebase credentials
#define API_KEY "AIzaSyBGGiCIob4h4Nwm-d7B0wQn3dmYLbJ-T-U"
#define DATABASE_URL "https://saferide-50fb4-default-rtdb.firebaseio.com"
#define USER_EMAIL "saferideuser@gmail.com"
#define USER_PASSWORD "mypassword456"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// MPU6050 sensor
MPU6050 mpu;
#define MOTION_THRESHOLD 25000

// Buzzer pin
#define BUZZER_PIN 26

void setup() {
  Serial.begin(115200);

  // Start MPU6050
  Wire.begin(21, 22);
  mpu.initialize();
  if (mpu.testConnection()) {
    Serial.println("✅ MPU6050 connected");
  } else {
    Serial.println("❌ MPU6050 connection failed");
    while (1);
  }

  // Set buzzer pin as output
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");

  // Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  long totalForce = abs(ax) + abs(ay) + abs(az);
  Serial.print("Accel: ");
  Serial.println(totalForce);

  if (totalForce > MOTION_THRESHOLD) {
    Serial.println("⚠️ Motion Detected!");

    // Buzzer ON
    digitalWrite(BUZZER_PIN, HIGH);
    delay(3000);  // Buzz for 3 seconds
    digitalWrite(BUZZER_PIN, LOW);

    // Send alert to Firebase
    if (Firebase.RTDB.setString(&fbdo, "/bike/motionAlert", "Motion Detected!")) {
      Serial.println("✅ Motion alert sent to Firebase");
    } else {
      Serial.println("❌ Firebase error: " + fbdo.errorReason());
    }

    delay(5000); // Wait to avoid spamming alerts
  }

  delay(1000);
}
