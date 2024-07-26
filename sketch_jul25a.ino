#include <WiFi.h>
#include <FirebaseESP32.h>
#include <WebSocketsClient.h>

const char* ssid = "hello";
const char* password = "wing2330";
#define FIREBASE_HOST "sample-609d0-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyAo6O014JzlXqca_uZlGZTo56S9nk8QkS4"

const char* websocketHost = "192.168.137.77";
const uint16_t websocketPort = 8080;
const char* websocketURL = "ws://192.168.137.77:8080";

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

WebSocketsClient webSocket;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected.");

  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);

  webSocket.begin(websocketHost, websocketPort, websocketURL);
  webSocket.setReconnectInterval(5000);
  webSocket.onEvent(webSocketEvent);
  webSocket.sendTXT("ESP32"); 
  Serial.println("Setup complete.");
}

void loop() {
  webSocket.loop();
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  if (currentTime - lastTime > 5000) {
    lastTime = currentTime;
    if (Firebase.getString(fbdo, "led")) {
      String firebaseData = fbdo.stringData();
      Serial.println("Data from Firebase: " + firebaseData);
      webSocket.sendTXT(firebaseData);
    } else {
      Serial.println("Failed to get data from Firebase");
    }
  }
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("Disconnected from WebSocket server");
      webSocket.begin(websocketHost, websocketPort, websocketURL);
      webSocket.setReconnectInterval(5000);
      break;
    case WStype_CONNECTED:
      Serial.printf("Connected to WebSocket server at %s:%d\n", websocketHost, websocketPort);
      webSocket.sendTXT("ESP32");
      break;
    case WStype_TEXT:
      {
        String receivedMessage = String((char*)payload);
        Firebase.setString(fbdo, "led", receivedMessage);
      }
      break;
    case WStype_BIN:
      Serial.println("Binary message received");
      break;
    case WStype_ERROR:
      Serial.println("WebSocket error");
      break;
    case WStype_PING:
      Serial.println("WebSocket PING received");
      break;
    case WStype_PONG:
      Serial.println("WebSocket PONG received");
      break;
  }
}

