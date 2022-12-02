/*
  Simple sketch to send temperature from the internal sensor
  of Arduino NANO RP2040 to Elaticsearch using Elastic Cloud

  In this example we used the ArduinoJson library to create
  the body of the request
*/
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <ESClient.h> // The Arduino Elaticsearch client
#include <Arduino_LSM6DSOX.h> // for Arduino NANO RP2040
#include "elasticsearch_config.h"

// WiFi settings
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

// Elastic settings
char serverAddress[] = ELASTIC_ENDPOINT;

// NTP server settings
WiFiUDP ntpUDP;
// By default 'pool.ntp.org' is used with 60 seconds update interval and
NTPClient timeClient(ntpUDP);

ESClient client = ESClient(serverAddress); // Elasticsearch client
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(9600);

  // For Arduino NANO RP2040
  if (!IMU.begin()){
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);  // print the network name (SSID);
  
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
  }

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  timeClient.begin();
  
  client.setElasticCloudApiKey(ELASTIC_CLOUD_API_KEY); // Elastic Cloud API key
}

void loop() {
  timeClient.update();
  
  float temperature = 0;

  if (IMU.temperatureAvailable()) {
    IMU.readTemperatureFloat(temperature);

    Serial.print("LSM6DSOX Temperature = ");
    Serial.print(temperature);
    Serial.println(" °C");
  }

  ESResponse result;
  
  StaticJsonDocument<200> doc;
  doc["@timestamp"] = timeClient.getEpochTime(); // we need to send a timestamp value
  doc["temp"] = temperature;
 
  String body;
  serializeJson(doc, body);

  // Send the temperature to Elastic Cloud
  result = client.index("temperature", body);
  
  int statusCode = result.statusCode;
  String response = result.body;

  Serial.print("GET Status code: ");
  Serial.println(statusCode);
  Serial.print("GET Response: ");
  Serial.println(response);

  Serial.println("Wait 10 seconds");
  delay(10000);
}
