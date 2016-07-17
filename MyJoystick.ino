// Arduino Pro Micro Joystick
//--------------------------------------------------------------------

#include <Joystick.h>

const bool DEBUG = false;

const int BUTTON_COUNT = 14;
const int RX_LED = 17;
const int X_PIN = A3;
const int Y_PIN = A2;
const int DEADZONE = 2;
const int MAX_READOUT = 1023;

// For non-linear scaling.
const float powBase = 1.4;
const float minUseful = -10;
const float maxUseful = 0;

float minUsefulValue = 0;
float maxUsefulValue = 1;

// Some analog inputs will not reach full readout.
const float inputRange = 0.65;

// Neutral values for analog stick.
int xZero, yZero;
int minX, maxX, minY, maxY;
// Stick values.
int xValue, yValue;


int powScale(float inputValue, float originalMin, float originalMax, float newBegin, float newEnd){
  
  // Map to useful function range.
  float normValue = (inputValue - originalMin) * (maxUseful - minUseful) / (originalMax - originalMin) + minUseful;
  normValue = pow(powBase, normValue);

  return (int)((normValue - minUsefulValue) * (newEnd - newBegin) / (maxUsefulValue - minUsefulValue) + newBegin);
}


void setup() {

  // Edges for scaling function.
  minUsefulValue = pow(powBase, minUseful);
  maxUsefulValue = pow(powBase, maxUseful);

  // Initialize analog stick.
  pinMode(X_PIN, INPUT);
  pinMode(Y_PIN, INPUT);

  // Get neutral position.
  xZero = analogRead(X_PIN);
  yZero = analogRead(Y_PIN);

  // Get possible range.
  minX = -xZero * inputRange;
  maxX = (MAX_READOUT - xZero) * inputRange;
  minY = -yZero * inputRange;
  maxY = (MAX_READOUT - yZero) * inputRange;
  
  // Initialize Button Pins.
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);

  // RX Led.
  pinMode(RX_LED, OUTPUT);
  
  // Initialize Joystick Library.
  Joystick.begin();

  // Debug.
  if(DEBUG) {
    Serial.begin(9600);
  }
}

// Joystick button to pin map.
int pinMap[BUTTON_COUNT] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 14, 15, 16, 18, 19};

// Last state of the button.
int lastButtonState[BUTTON_COUNT] = {0};

void loop() {

  // Read analog stick position.
  xValue = analogRead(X_PIN) - xZero;
  yValue = analogRead(Y_PIN) - yZero;

  // Cap to limits.
  xValue = constrain(xValue, minX, maxX);
  yValue = constrain(yValue, minY, maxY);

  // Deadzone.
  if(abs(xValue) < DEADZONE) {
    xValue = 0;
  }
  if(abs(yValue) < DEADZONE) {
    yValue = 0;
  }

  // Exponential rescale values to -127 - 127 range.
  if (xValue > 0)
    xValue = powScale(xValue, 0, maxX, 0, 127);
  else if (xValue < 0)
    xValue = -powScale(-xValue, 0, -minX, 0, 127);
    
  if (yValue > 0)
    yValue = powScale(yValue, 0, maxY, 0, 127);
  else if (yValue < 0)
    yValue = -powScale(-yValue, 0, -minY, 0, 127);

  if(DEBUG) {
    Serial.println("X =  " + String(xValue) + ", range: " + String(minX) + " - " + String(maxX) + ", corr: = " + String(xZero));
    Serial.println("Y =  " + String(yValue) + ", range: " + String(minY) + " - " + String(maxY) + ", corr: = " + String(yZero));
  }
  
  // Set the actual joystick position.
  Joystick.setXAxis((byte)xValue);
  Joystick.setYAxis((byte)yValue);

  // Read button values.
  for (int index = 0; index < BUTTON_COUNT; index++)
  {
    int currentButtonState = !digitalRead(pinMap[index]);
    if (currentButtonState != lastButtonState[index])
    {
      Joystick.setButton(index, currentButtonState);
      lastButtonState[index] = currentButtonState;
      if(DEBUG) {
        Serial.println("Button " + String(index) + " has changed state to " + String(currentButtonState));
      }
    }
  }

  delay(50);
}

