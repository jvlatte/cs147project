#include <Arduino.h>
#include <Wire.h>
#include <cmath>
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
const int micPin = 26;
const int micPin2 = 36;
int soundLevel = 0;
const int threshold = 500;
double last_val = -1;

const char *serverName = "http://3.16.38.103:5000";


// Function to control haptic motor based on sound level.
void controlVibration(int level) {
  // If positive value, vibrate with intensity proportional to sound level.
  if (level > 0) {
    drv.setRealtimeValue(level); 
  } else {
    // Otherwise stop vibration.
    drv.setRealtimeValue(0); 
  }

  // drv.setRealtimeValue(20); 

  // Play vibration.
  drv.go();
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

// Send sound data to AWS
void sendToAWS(int level) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"soundLevel\": " + String(level) + "}";
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    } else {
      Serial.printf("Error in sending POST: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
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
  pinMode(micPin2, INPUT);
}

void loop() {
  // Calculate sound level here...
  double micValue_analog = analogRead(micPin2);
  // double current = micValue_analog;


  sendToAWS(micValue_analog);



  // Generate vibration based on sound level.
  // if (micValue_analog > 20){
  //   controlVibration(micValue_analog);
  // }
  // else {
  //   controlVibration(0);
  // }

  if (last_val != -1) {
    if (abs(micValue_analog - last_val) > 5) {
      if (micValue_analog > 23) {
        controlVibration(micValue_analog);
      } else {
        controlVibration(micValue_analog * 2.5);
      }
    }
    else {
      controlVibration(0);
    }
    last_val = micValue_analog;
  }
  else {
    controlVibration(0);
    last_val = micValue_analog;
  }

  delay(50);
}
