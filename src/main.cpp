#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <inttypes.h>
#include <stdio.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"

Adafruit_DRV2605 drv;
char ssid[50]; 
char pass[50];
const int micPin = 36;
int soundLevel = 0;
const int threshold = 500;



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

// Process sound value to calculate sound level
int processSound(int micValue) {
  // Convert raw mic value to a usable sound level (adjust as necessary)
  return micValue > threshold ? map(micValue, threshold, 4095, 0, 255) : 0;
}

void nvs_access()
{
  // Initialize NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  // Open
  Serial.printf("\n");
  Serial.printf("Opening Non-Volatile Storage (NVS) handle... ");
  nvs_handle_t my_handle;
  err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK)
  {
    Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  }
  else
  {
    Serial.printf("Done\n");
    Serial.printf("Retrieving SSID/PASSWD\n");
    size_t ssid_len;
    size_t pass_len;
    err = nvs_get_str(my_handle, "ssid", ssid, &ssid_len);
    err |= nvs_get_str(my_handle, "pass", pass, &pass_len);
    switch (err)
    {
    case ESP_OK:
      Serial.printf("Done\n");
      // Serial.printf("SSID = %s\n", ssid);
      // Serial.printf("PASSWD = %s\n", pass);
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      Serial.printf("The value is not initialized yet!\n");
      break;
    default:
      Serial.printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
  }
  // Close

  nvs_close(my_handle);
}


void setup() {
  Serial.begin(9600);
  Serial.println("DRV2605 Output Test");

  // Connect to WiFi
  delay(1000);
  nvs_access();
  delay(1000);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());

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
  pinMode(micPin, INPUT);
}

void loop() {
  // Calculate sound level here...
  int micValue = analogRead(micPin);
  micValue = micValue - 625;
  if (micValue < 0) micValue = -micValue;

  soundLevel = processSound(micValue);

  Serial.print("Mic Value 1: ");
  Serial.println(micValue);
  
  // Generate vibration based on sound level.
  controlVibration(soundLevel);
  delay(100);
}
