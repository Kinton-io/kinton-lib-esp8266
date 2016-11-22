// Copyright 2016 Diego Fernández Barrera
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>

static const uint8_t MAX_TOPICS = 32;
static const uint16_t KINTON_PORT = 1884;
static const char *KINTON_MQTT_SERVER = "mqtt.kinton.io";
static const char *KINTON_API = "http://api.kinton.io/api/fleets/";
static const char *KINTON_API_REGISTER_METHOD = "/registerMote";

class KintonMQTT {
public:
  // Attributes
  char *topics[MAX_TOPICS];
  void (*callbacks[MAX_TOPICS])(byte *payload, unsigned int length);

  // Methods
  KintonMQTT(WiFiClient client, const char *mqtt_id);
  void setCredentials(const char *device_uuid, const char *device_secret);
  bool registerDevice(const char *fleet_key);
  void on(const char *topic, void (*)(byte *payload, unsigned int length));
  const char *getDeviceUUID();
  const char *getDeviceSecret();
  bool loop();

private:
  // Attributes
  const char *kinton_ip = KINTON_MQTT_SERVER;
  const uint16_t kinton_port = KINTON_PORT;
  const char *mqtt_id;
  const char *device_uuid;
  const char *device_secret;

  PubSubClient *client;

  // Methods
  bool connect();
};
