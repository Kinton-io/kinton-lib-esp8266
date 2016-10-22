#include "kinton.hpp"

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <WiFiManager.h>

// Configuration
const char *CONFIG_FILE = "/kinton.config";
const char *FLEET_KEY = "<Your kinton FLEET_KEY>";

// Globals
WiFiClient client;
KintonMQTT kinton(client, "test_device");

/**
 * Initializes the kinton module by loading the credentials from some storage
 * provider, it could be an EEPROM, or an SD card. In this case is the ESP8266
 * file system. If no crentials are found on the filesystem, then perform
 * a registration to obtain new credentials from the API.
 */
bool init_kinton_config() {
  char device_uuid[37];
  char device_secret[65];

  // Try to load the credentials from the file system
  File kinton_config = SPIFFS.open(CONFIG_FILE, "r");
  if (kinton_config) {
    kinton_config.readBytes(device_uuid, 36);
    kinton_config.readStringUntil('\n');
    kinton_config.readBytes(device_secret, 64);

    device_uuid[36] = '\0';
    device_secret[64] = '\0';
    kinton_config.close();

    kinton.setCredentials(strdup(device_uuid), strdup(device_secret));
  } else {

    // If credentials not found, then perform the registration
    if (kinton.registerDevice(FLEET_KEY)) {

      // Save the credentials for later use
      File kinton_config = SPIFFS.open(CONFIG_FILE, "w");
      if (kinton_config) {
        kinton_config.println(device_uuid);
        kinton_config.println(device_secret);
        kinton_config.close();
      } else {
        Serial.println("Error storing credentials");
      }
    } else {
      return false;
    }
  }
}

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();

  // This is only necessary the first time to format the file system, then
  // comment these lines

  // Serial.println("Formatting FS...");
  // SPIFFS.format();
  // Serial.println("Done");

  String device_topic;
  WiFiManager wifiManager;
  wifiManager.autoConnect("Kinton AP");

  // If it's not possible to initiate the kinton connection, then halt the
  // device
  if (!init_kinton_config()) {
    goto halt;
  }

  // Subscribe to a topic with the same name as de device UUID so it's possible
  // to send data only to this device knowing its UUID
  // NOTE Every topic should be prefixed by the FLEET_KEY
  device_topic += FLEET_KEY;
  device_topic += "/";
  device_topic += kinton.getDeviceUUID();

  kinton.addTopic(device_topic.c_str());

halt:
  Serial.println("Halted");
  while (true) {
    delay(1);
  }
}

void loop() {
  kinton.loop(); // Call this function on every loop iteration
}
