#include <Arduino.h>
#include <Servo.h>

Servo myServo;

#include <Adafruit_NeoPixel.h>

// ==== CONFIGURATION ====
#define PIN         6       // Pin where the NeoPixel ring is connected
#define NUMPIXELS   12      // Number of pixels in the ring (e.g., 8, 12, 16, 24, etc.)

Adafruit_NeoPixel ring(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int startAngle = 90;     // Middle position
int moveAmount = 35;     // Move 30 degrees up/down
int currentAngle;
int targetAngle;
int stepDelay = 20;      // Delay between small steps (ms)
int stepSize = 1;        // Move 1 degree per step

void setup() {
  myServo.attach(9);
  currentAngle = startAngle;
  targetAngle = startAngle + moveAmount;
  myServo.write(currentAngle);
  delay(2);

  ring.begin();      // Initialize the NeoPixel library
  ring.show();       // Initialize all pixels to 'off'
}


uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return ring.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return ring.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return ring.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j = 0; j < 256; j++) { // One cycle of all colors
    for(i = 0; i < ring.numPixels(); i++) {
      ring.setPixelColor(i, Wheel((i * 256 / ring.numPixels() + j) & 255));
    }
    ring.show();
    delay(wait);
  }
}



void loop() {
  // Move slowly towards targetAngle
  rainbowCycle(10);  // Cycle through colors

  if (currentAngle < targetAngle) {
    currentAngle += stepSize;
    if (currentAngle > targetAngle) currentAngle = targetAngle;
  } else if (currentAngle > targetAngle) {
    currentAngle -= stepSize;
    if (currentAngle < targetAngle) currentAngle = targetAngle;
  }

  myServo.write(currentAngle);
  delay(stepDelay);

  // When reached target, switch target to other direction
  if (currentAngle == targetAngle) {
    if (targetAngle == startAngle + moveAmount) {
      targetAngle = startAngle - moveAmount;  // Move down next
    } else {
      targetAngle = startAngle + moveAmount;  // Move up next
    }
  }
}