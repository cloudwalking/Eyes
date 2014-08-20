#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <LSM303.h>

#include "types.h"

#define LED_DATA_PIN 12
#define LED_COUNT 32
#define LED_BRIGHTNESS 60

#define SENSITIVITY 5
#define OFFSET 15

#define BUTTON_PIN 10

// Change each pixels color randomly up to this amount.
#define COLOR_RANDOMNESS 3

#define RING_LEFT_ROTATION  0
#define RING_RIGHT_ROTATION -2

Adafruit_NeoPixel _pixels = Adafruit_NeoPixel(LED_COUNT, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);
LSM303 _compass;

uint16_t _currentHeading;

ButtonState _mode;
ButtonState _function;

bool _buttonIsPushed;
unsigned long _time;

void setup(void) {
  Serial.begin(9600);
  
  compassSetup();

  lightSetup();
  
  pinMode(BUTTON_PIN, INPUT);
  
  _currentHeading = 999;
  
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
  
  delay(20);
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
  if (_mode == MODE_2) {
    headlamp(heading);
  } else {
    rainbowCycle(heading);
  }
  
  if (_function == FUNCTION_2) {
    sparkle();
  }
}

void headlamp(int heading) {
  if (abs(_currentHeading - heading) > SENSITIVITY) {
    _currentHeading = heading;
  }
  
  for (uint8_t led = 0; led < LED_COUNT; led++) {
    _pixels.setPixelColor(led, color(84));
  }

  updatePupil(_currentHeading);
      
  _pixels.show();
}

float leftRotation = 0.0, rightRotation = 0.0;

void rainbowCycle(int heading) {
  int led;
  
  if (abs(_currentHeading - heading) > SENSITIVITY) {
    _currentHeading = heading;
    
    leftRotation = heading / 360.0 * 256;
    rightRotation = (heading + OFFSET) / 365.0 * 256;
  }
  
  // Serial.print(leftRotation);
  // Serial.print("\t");
  // Serial.print(rightRotation);
  // Serial.println();
  
  for (led = 0; led < LED_COUNT / 2; led++) {
    byte leftColor = (int)leftRotation & 255;
    _pixels.setPixelColor(led, colorNear(leftColor));
  }

  for(led = LED_COUNT / 2; led < LED_COUNT; led++) {
    // Rainbow mode
    // byte position = ((led * 256 / strip.num_pixels()) + heading) & 255;

    byte rightColor = (int)rightRotation & 255;
    _pixels.setPixelColor(led, colorNear(rightColor));
  }
  
  updatePupil(_currentHeading);

  _pixels.show();
}

uint8_t pupil = 0;

void updatePupil(int heading) {
  float pupil_f = round(LED_COUNT / 2) * heading / 360.0;
  pupil = (uint8_t)pupil_f;
  
  Serial.print("pupil "); Serial.println(pupil);

  int8_t left  = pupil + RING_LEFT_ROTATION;
  left = constrainBetween(left, 0, LED_COUNT / 2);
  Serial.print("left "); Serial.println(left);
  _pixels.setPixelColor(left, _pixels.Color(0,0,0));

  left += 1;
  left = constrainBetween(left, 0, LED_COUNT / 2);
  Serial.print("left+1 "); Serial.println(left);
  _pixels.setPixelColor(left, _pixels.Color(0,0,0));
  
  int8_t right = pupil + RING_RIGHT_ROTATION + LED_COUNT / 2;
  right = constrainBetween(right, LED_COUNT / 2, LED_COUNT);
  Serial.print("right "); Serial.println(right);
  _pixels.setPixelColor(right, _pixels.Color(0,0,0));
  
  right += 1;
  right = constrainBetween(right, LED_COUNT / 2, LED_COUNT);
  Serial.print("right+1 "); Serial.println(right);
  _pixels.setPixelColor(right, _pixels.Color(0,0,0));
}

int constrainBetween(int value, int lower, int higher) {
  if (value < lower) {
    value = higher - (lower - value) + 1;
  } else if (value > higher) {
    value = lower + (value - higher) - 1;
  }
  return value;
}

void sparkle() {
  uint8_t count = (random() % 5) + 2; // 2 to 5 LEDs.

  uint8_t chance;
  uint32_t pixelColor;

  if (_mode == MODE_1) {
    chance = 20;
    pixelColor = _pixels.Color(255,255,255);
  } else {
    chance = 100;
    pixelColor = _pixels.Color(255,0,255);
  }

  if (random() % chance == 0) {
    for (uint16_t i = 0; i < count; i++) {
      uint16_t pixel = random() % LED_COUNT;
      _pixels.setPixelColor(pixel, pixelColor);
    }
    _pixels.show();
  }
}

// Random color close to the given color.
uint32_t colorNear(byte position) {
  // First choose above or below position
  int8_t sign = 1;
  if (random() % 2 == 0) {
    sign = -1;
  }
  
  // Difference from the given color
  uint8_t change = COLOR_RANDOMNESS ? sign * (random() % COLOR_RANDOMNESS) : 0;
  
  return color(position + change);
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
