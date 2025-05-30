#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "WIFI_SSID";
const char* password = "PASSWORD";

// change the pins
const int redPin = 14;
const int greenPin = 12;
const int bluePin = 13;

WebServer server(80);

void setColor(int r, int g, int b) {
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}

void handleSetColor() {
  if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
    int r = server.arg("r").toInt();
    int g = server.arg("g").toInt();
    int b = server.arg("b").toInt();
    Serial.print("Received Color: R = ");
    Serial.print(r);
    Serial.print(", G = ");
    Serial.print(g);
    Serial.print(", B = ");
    Serial.println(b);
    setColor(r, g, b);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "Color set");
  } else {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(400, "text/plain", "Missing RGB values");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("ESP32 connected with IP: ");
  Serial.println(WiFi.localIP());

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  server.on("/setColor", handleSetColor);
  server.begin();
}

void loop() {
  server.handleClient();
}
