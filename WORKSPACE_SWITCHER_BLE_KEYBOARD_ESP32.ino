/**
               _   _                    _       _____ _  __
    /\        | | | |                : | |     / ____| |/ /
   /  \  _   _| |_| |__   ___  _ __  : | |    | (___ | ' / 
  / /\ \| | | | __| '_ \ / _ \| '__' : | |     \___ \|  <  
 / ____ \ |_| | |_| | | | (_) | |    : | |____ ____) | . \ 
/_/    \_\__,_|\__|_| |_|\___/|_|    : |______|_____/|_|\_\                                                 

  Description: This code uses ble keyboard library and joystick to create a macOS workspace switcher and also play/pause the media by using swBtn.

  Connections:
    JOYSTICK:
      VRX = 4
      VRY = 2
      SWPIN = 19
*/

#include <BleKeyboard.h>

enum Direction {
  LEFT,
  RIGHT,
  DOWN,
  UP,
  CENTRE
};

Direction direction;
Direction _direction;
int vrxPin = 4;
int vryPin = 2;
int swPin = 18;

int centreX = 0;
int centreY = 0;

int x = 0;
int y = 0;

BleKeyboard bleKeyboard;

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

Button button1 = { 18, 0, false };

unsigned long button_time = 0;
unsigned long last_button_time = 0;

void IRAM_ATTR isr() {
  button_time = millis();
  if (button_time - last_button_time > 250) {
    button1.numberKeyPresses++;
    button1.pressed = true;
    last_button_time = button_time;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, isr, FALLING);
  Serial.println("Starting BLE work!");
  bleKeyboard.begin();

  x = analogRead(vrxPin);
  y = analogRead(vryPin);

  // Calibrating
  centreX = x;
  centreY = y;
}

void loop() {
  x = analogRead(vrxPin);
  y = analogRead(vryPin);

  int xDistance = centreX - x;
  int yDistance = centreY - y;

  if (xDistance == yDistance || abs(xDistance) <= 100 || abs(yDistance) <= 100) {
    direction = CENTRE;
  } else if (abs(xDistance) > abs(yDistance)) {
    if (xDistance < -900) {
      direction = RIGHT;
    } else if (xDistance > 1000) {
      direction = LEFT;
    }
  } else if (xDistance == yDistance || abs(xDistance) <= 1000 || abs(yDistance) <= 1000) {
    direction = CENTRE;
  } else {  // xDistance < yDistance
    if (yDistance < -1000) {
      direction = DOWN;
    } else if (yDistance > 1000) {
      direction = UP;
    }
  }

  if (_direction != UP && direction == UP) {
    bleKeyboard.press(KEY_LEFT_CTRL);
    bleKeyboard.press(KEY_LEFT_ARROW);
    delay(100);
    bleKeyboard.releaseAll();
  } else if (_direction != DOWN && direction == DOWN) {
    bleKeyboard.press(KEY_RIGHT_CTRL);
    bleKeyboard.press(KEY_RIGHT_ARROW);
    delay(100);
    bleKeyboard.releaseAll();
    delay(100);
  }

  if (button1.pressed) {
    if (bleKeyboard.isConnected()) {
      button1.pressed = false;
      Serial.println("pressed");
      bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
      delay(200);
    }
  }

  _direction = direction;
}