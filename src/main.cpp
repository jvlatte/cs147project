#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>

Adafruit_DRV2605 drv;

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
  // Set internal trigger mode.
  drv.setMode(DRV2605_MODE_INTTRIG); 
}

void loop() {
  // Try preset effects 1 to 10. 
  for (uint8_t effect = 1; effect <= 10; effect++) { 
    Serial.print("Playing effect #");
    Serial.println(effect);

    // Set effect for slot 0.
    drv.setWaveform(0, effect); 
    // End waveform with 0 to stop.
    drv.setWaveform(1, 0);     
  
    // Play pattern then wait a second before the next effect.
    drv.go(); 
    delay(1000); 
  }
}
