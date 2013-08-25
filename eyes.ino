#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <LSM303.h>

#define LEFT_PIN 9
#define RIGHT_PIN 10

#define SENSITIVITY 1
#define OFFSET 15

// Adafruit_NeoPixel left = Adafruit_NeoPixel(16, LEFT_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel right = Adafruit_NeoPixel(32, RIGHT_PIN, NEO_GRB + NEO_KHZ800);
LSM303 compass;

void setup(void) {
  Serial.begin(9600);

  compassSetup();

  lightSetup();
}

void loop() {
  compass.read();
  int heading = compass.heading((LSM303::vector){0,-1,0});
  
  rainbowCycle(heading);
}

////////////////////////////////////////////////////////

void compassSetup() {
  compass.init();
  compass.enableDefault();
  
  // M min X: -200 Y: -459 Z: -290 M max X: 1195 Y: 914 Z: 602

  compass.m_min.x = -200;
  compass.m_min.y = -459;
  compass.m_min.z = -290;

  compass.m_max.x = 1195;
  compass.m_max.y = 914;
  compass.m_max.z = 602;
}

void lightSetup() {
  Wire.begin();

  // left.begin();
  // left.show();
  // left.setBrightness(24);
  
  right.begin();
  right.show();
  right.setBrightness(24);
}

////////////////////////////////////////////////////////

int currentHeading = 999;

void rainbowCycle(int heading) {
  int led;
  
  if (abs(currentHeading - heading) > SENSITIVITY) {
    currentHeading = heading;
  
    float leftRotation = heading / 365.0 * 256;
    float rightRotation = (heading + OFFSET) / 365.0 * 256;
    
    Serial.print(leftRotation);
    Serial.print("\t");
    Serial.print(rightRotation);
    Serial.println();

    for(led = 0; led < right.numPixels(); led++) {
      // Rainbow mode
      // byte position = ((led * 256 / strip.numPixels()) + heading) & 255;
      
      // Solid mode
      byte leftColor = (int)leftRotation & 255;
      byte rightColor = (int)rightRotation & 255;
      
      // left.setPixelColor(led, color(leftColor));
      right.setPixelColor(led, color(rightColor));
    }

    // left.show();
    right.show();
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t color(byte WheelPos) {
  if(WheelPos < 85) {
   return right.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return right.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return right.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
