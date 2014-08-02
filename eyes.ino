#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <LSM303.h>

#include "types.h"

#define LED_DATA_PIN 12
#define LED_COUNT 32
#define LED_BRIGHTNESS 60

#define SENSITIVITY 1
#define OFFSET 15

#define BUTTON_PIN 10

Adafruit_NeoPixel _pixels = Adafruit_NeoPixel(LED_COUNT, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);
LSM303 _compass;

uint16_t _currentHeading = 999;

ButtonState _mode;
ButtonState _function;

bool _buttonIsPushed;
unsigned long _time;

void setup(void) {
  Serial.begin(9600);
  
  compassSetup();

  lightSetup();
  
  pinMode(BUTTON_PIN, INPUT);
  
  _mode = MODE_1;
  _function = FUNCTION_1;
  
  _buttonIsPushed = false;
  _time = 0;
}

void loop() {
  int heading = 0;

  _compass.read();
  heading = _compass.heading((LSM303::vector){0,-1,0});
    
  handleButton();
  
  updatePixels(heading);
}

////////////////////////////////////////////////////////

void compassSetup() {
  _compass.init();
  _compass.enableDefault();

  // M min X: -200 Y: -459 Z: -290 M max X: 1195 Y: 914 Z: 602
  // M min X: -502 Y: -656 Z: -535 M max X: 616 Y: 354 Z: 165

  _compass.m_min.x = -502;
  _compass.m_min.y = -656;
  _compass.m_min.z = -535;

  _compass.m_max.x = 616;
  _compass.m_max.y = 354;
  _compass.m_max.z = 165;
}

void lightSetup() {
  Wire.begin();
  
  _pixels.begin();
  _pixels.show();
  _pixels.setBrightness(LED_BRIGHTNESS);
}

///////////////////////////////////////////////////////

void handleButton() {
  int read = digitalRead(BUTTON_PIN);
  
  if (_buttonIsPushed == false) {
    if (read == HIGH) {
      _buttonIsPushed = true;
      _time = millis();
    }
  } else {
    if (read == LOW) {
      _buttonIsPushed = false;
    }

    unsigned long now = millis();
    unsigned long elapsed = now - _time;
    
    // Change mode when button is held for 3 seconds
    if (elapsed >= 3000) {
      changeMode();
      // Reset the timer
      _time = millis();
    } 
    // Change function when button is released after 1+ seconds
    else if (_buttonIsPushed == false && elapsed >= 1000) {
      changeFunction();
    }
  }
}

void changeMode() {
  _mode = _mode == MODE_1 ? MODE_2 : MODE_1;
  
  // Changing mode defaults to FUNCTION_1
  _function = FUNCTION_1;
  
  // Set a bogus current heading to force a heading update
  _currentHeading = 999;
}

void changeFunction() {
  _function = _function == FUNCTION_1 ? FUNCTION_2 : FUNCTION_1;
  
  // Set a bogus current heading to force a heading update
  _currentHeading = 999;
}

///////////////////////////////////////////////////////

void updatePixels(int heading) {
  if (abs(_currentHeading - heading) > SENSITIVITY) {
    _currentHeading = heading;

    if (_mode == MODE_2) {
      headlamp(heading);
    } else {
      rainbowCycle(heading);
    }
  }
}

void headlamp(int heading) {
  for (uint8_t led = 0; led < LED_COUNT; led++) {
    _pixels.setPixelColor(led, color(84));
  }
  
  float pupil_f = LED_COUNT * heading / 360.0;
  uint8_t pupil = (uint8_t)pupil_f;
  
  _pixels.setPixelColor(pupil, _pixels.Color(0,0,0));
  _pixels.setPixelColor(LED_COUNT - 1 - pupil, _pixels.Color(0,0,0));
      
  _pixels.show();
}

void rainbowCycle(int heading) {
  int led;
  
  float leftRotation = heading / 360.0 * 256;
  float rightRotation = (heading + OFFSET) / 365.0 * 256;
  
  // Serial.print(leftRotation);
  // Serial.print("\t");
  // Serial.print(rightRotation);
  // Serial.println();
  
  for (led = 0; led < LED_COUNT / 2; led++) {
    byte leftColor = (int)leftRotation & 255;
    _pixels.setPixelColor(led, color(leftColor));
  }

  for(led = LED_COUNT / 2; led < LED_COUNT; led++) {
    // Rainbow mode
    // byte position = ((led * 256 / strip.num_pixels()) + heading) & 255;

    byte rightColor = (int)rightRotation & 255;
    _pixels.setPixelColor(led, color(rightColor));
  }

  _pixels.show();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t color(byte WheelPos) {
  if(WheelPos < 85) {
   return _pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return _pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return _pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
