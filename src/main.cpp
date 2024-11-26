#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>

Adafruit_DRV2605 drv;

// Function to control haptic motor based on sound level.
void controlVibration(int level) {
  // If positive value, vibrate with intensity proportional to sound level.
  if (level > 0) {
    drv.setRealtimeValue(level); 
  } else {
    // Otherwise stop vibration.
    drv.setRealtimeValue(0); 
  }
  // Play vibration.
  drv.go();
}

void setup() {
  Serial.begin(9600);
  Serial.println("DRV2605 Output Test");

  // Initialize DRV.
  if (!drv.begin()) {
    Serial.println("Could not find DRV...");
    while (1);
  }
  Serial.println("Found DRV!");

  // Set the haptic mode for vibrating motor disc.
  drv.selectLibrary(1);
  // Set realtime vibration control.
  drv.setMode(DRV2605_MODE_REALTIME);
}

void loop() {
  // Calculate sound level here...
  
  // Generate vibration based on sound level.
  controlVibration(soundLevel);
}
