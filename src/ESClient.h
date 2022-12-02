// Elasticsearch client for Arduino
// (c) Copyright 2022 Enrico Zimuel
// Released under MIT License

#ifndef ESClient_h
#define ESClient_h

#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>

// HTTP response from Elasticsearch
struct ESResponse {
    int statusCode;
    String body;
};

class ESClient {

public:

    const String version = "0.1.0";

    // Constructor
    ESClient(char* serverAddress, int port = 443);

    void setElasticCloudApiKey(String apiKey);
    void setBasicAuth(String username, String password);
    void setOAuth2Token(String token);  

    ESResponse index(String index, String body); 
    ESResponse get(String index, String id);
    ESResponse del(String index, String id);
    ESResponse update(String index, String id, String body);
    ESResponse search(String index, String body);
    ESResponse sendRequest(String method, String url, String body = "", String contentType = "application/json");
protected:

    WiFiSSLClient wifi;
    HttpClient client;

    String elasticCloudApiKey;
    String basicUsername;
    String basicPassword;
    String oauth2Token;
};
#endif
