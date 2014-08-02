#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <LSM303.h>

#define LED_DATA_PIN 12
#define LED_COUNT 32
#define LED_BRIGHTNESS 60

#define SENSITIVITY 1
#define OFFSET 15

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_COUNT, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);
LSM303 compass;

void setup(void) {
  Serial.begin(9600);
  
  compassSetup();

  lightSetup();
}

void loop() {
  int heading = 0;

  compass.read();
  heading = compass.heading((LSM303::vector){0,-1,0});
  
  Serial.println(heading);
  
  rainbowCycle(heading);
}

////////////////////////////////////////////////////////

void compassSetup() {
  compass.init();
  compass.enableDefault();

  // M min X: -200 Y: -459 Z: -290 M max X: 1195 Y: 914 Z: 602
  // M min X: -502 Y: -656 Z: -535 M max X: 616 Y: 354 Z: 165

  compass.m_min.x = -502;
  compass.m_min.y = -656;
  compass.m_min.z = -535;

  compass.m_max.x = 616;
  compass.m_max.y = 354;
  compass.m_max.z = 165;
}

void lightSetup() {
  Wire.begin();
  
  pixels.begin();
  pixels.show();
  pixels.setBrightness(LED_BRIGHTNESS);
}

////////////////////////////////////////////////////////

int currentHeading = 999;

void rainbowCycle(int heading) {
  int led;
  
  if (abs(currentHeading - heading) > SENSITIVITY) {
    currentHeading = heading;
  
    float leftRotation = heading / 365.0 * 256;
    float pixelsRotation = (heading + OFFSET) / 365.0 * 256;
    
    Serial.print(leftRotation);
    Serial.print("\t");
    Serial.print(pixelsRotation);
    Serial.println();
    
    for (led = 0; led < LED_COUNT / 2; led++) {
      byte leftColor = (int)leftRotation & 255;
      pixels.setPixelColor(led, color(leftColor));
    }

    for(led = LED_COUNT / 2; led < LED_COUNT; led++) {
      // Rainbow mode
      // byte position = ((led * 256 / strip.numPixels()) + heading) & 255;

      byte pixelsColor = (int)pixelsRotation & 255;
      pixels.setPixelColor(led, color(pixelsColor));
    }

    // left.show();
    pixels.show();
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t color(byte WheelPos) {
  if(WheelPos < 85) {
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
