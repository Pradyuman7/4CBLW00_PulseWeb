#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>

Servo myServo;

#define NEOPIXEL_PIN 14 // The data pin your NeoPixel ring is connected to
#define NUM_PIXELS   12 // The number of LEDs in your NeoPixel ring

const char* ssid = "WIFI_SSID";
const char* password = "PASSWORD";

int servoPin = 18;
int startAngle = 90;
int openAngle = 125;
int closedAngle = 55;
int currentServoAngle;
int stepDelay = 20;
int stepSize = 1;


Adafruit_NeoPixel ring(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

WebServer server(80);

void moveServoSmoothly(int targetAngle);
void setColor(int r, int g, int b);
void handleSetColor();

void setColor(int r, int g, int b) {
  for (int i = 0; i < NUM_PIXELS; i++) {
    ring.setPixelColor(i, r, g, b);
  }
  ring.show();
  delay(10);

  const int GREEN_THRESHOLD = 200;
  const int RED_THRESHOLD = 200;

  if (g >= GREEN_THRESHOLD && r < GREEN_THRESHOLD && b < GREEN_THRESHOLD) {
    Serial.println("High Green detected: Opening petals!");
    moveServoSmoothly(openAngle);
  } else if (r >= RED_THRESHOLD && g < RED_THRESHOLD && b < RED_THRESHOLD) {
    Serial.println("High Red detected: Keeping petals closed!");
    moveServoSmoothly(closedAngle);
  } else {
    Serial.println("Other color detected: Moving petals to start angle.");
    moveServoSmoothly(startAngle);
  }
}

void moveServoSmoothly(int targetAngle) {
  if (targetAngle == currentServoAngle) {
    return;
  }

  int direction = (targetAngle > currentServoAngle) ? 1 : -1;

  while (currentServoAngle != targetAngle) {
    currentServoAngle += direction * stepSize;

    if ((direction == 1 && currentServoAngle > targetAngle) ||
        (direction == -1 && currentServoAngle < targetAngle)) {
      currentServoAngle = targetAngle;
    }

    myServo.write(currentServoAngle);
    delay(stepDelay);
  }

  Serial.print("Servo moved to: ");
  Serial.println(currentServoAngle);
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

  ring.begin();
  ring.show();  // Turn off all pixels (clear the strip)
  ring.setBrightness(50);

  myServo.attach(servoPin);
  currentServoAngle = startAngle;
  myServo.write(currentServoAngle);
  Serial.print("Servo initialized to: ");
  Serial.println(currentServoAngle);

  server.on("/setColor", handleSetColor);
  server.begin();
}

void loop() {
  server.handleClient();
}
