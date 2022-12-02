/*
  Simple sketch to read from Elastic Cloud the aggregated
  average of temperature in the last hour

  In this example we used the ArduinoJson library to get
  specific fields of the JSON response from Elasticsearch
*/

#include <ArduinoJson.h>
#include <ESClient.h> // The Arduino Elaticsearch client
#include "elasticsearch_config.h"

// WiFi settings
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

// Elastic settings
char serverAddress[] = ELASTIC_ENDPOINT;

ESClient client = ESClient(serverAddress); // Elasticsearch client
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(9600);

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

  //timeClient.begin();
  
  client.setElasticCloudApiKey(ELASTIC_CLOUD_API_KEY); // Elastic Cloud API key
}

void loop() {
  ESResponse result;

  String body = R""""(
  {
    "query":{
      "range": { 
        "@timestamp": { 
          "gte": "now-1h" 
        } 
      } 
    },
    "aggs": {
      "avg_temp": { "avg": { "field": "temp" } }
    },
    "size" : 0
  }
  )"""";

  Serial.println("Get the average temperature of the last hour");
  result = client.search("temperature", body);

  int statusCode = result.statusCode;
  String response = result.body;

  Serial.print("GET Status code: ");
  Serial.println(statusCode);
  Serial.print("GET Response: ");
  Serial.println(response);
  
  StaticJsonDocument<1024> json;
  DeserializationError error = deserializeJson(json, response);
  if (error) {
    Serial.println("deserializeJson error:");
    Serial.println(error.f_str());
    return;
  }

  const String avg = json["aggregations"]["avg_temp"]["value"];
  Serial.println("The average temperature of the last hour is: " + avg);

  Serial.println("Wait 10 seconds");
  delay(10000);
}