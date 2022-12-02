// Elasticsearch client for Arduino
// (c) Copyright 2022 Enrico Zimuel
// Released under MIT License

#include "ESClient.h"

// Constructor
ESClient::ESClient(char* serverAddress, int port):
  client(wifi, serverAddress, port)
{
}

void ESClient::setElasticCloudApiKey(String apiKey){
  elasticCloudApiKey = apiKey;
}

void ESClient::setBasicAuth(String username, String password){
  basicUsername = username;
  basicPassword = password;
}

void ESClient::setOAuth2Token(String token){
  oauth2Token = token;
}
 
ESResponse ESClient::sendRequest(String method, String url, String body, String contentType){
#ifdef LOGGING
    Serial.println("Sending " + method + " " + url);
    Serial.println("Content-type: " + contentType);
#endif

    client.beginRequest();
    if (method == "get") { 
      client.get(url);
    } else if (method == "post") {
      client.post(url);
    } else if (method == "put") {
      client.put(url);
    } else if (method == "delete") {
      client.del(url);
    }
    // Elastic Cloud API key
    if (elasticCloudApiKey.length() > 0) {
      client.sendHeader("Authorization", "ApiKey " + elasticCloudApiKey);
    }
    // Basic authentication
    if (basicUsername.length() > 0 && basicPassword.length() > 0) {
      client.sendBasicAuth(basicUsername, basicPassword);
    }
    // OAuth2 token
    if (oauth2Token.length() > 0) {
      client.sendHeader("Authorization", "Bearer " + oauth2Token);
    }
    // Add x-elastic-client-meta header
    client.sendHeader("x-elastic-client-meta", "es=" + version + ",ino=1.0.0");

    // Add the body, if not empty
    if (body.length() > 0) {
#ifdef LOGGING
      Serial.println("Adding body: " + body);
#endif
      client.sendHeader("Content-Type", contentType);
      client.sendHeader("Content-Length", body.length());
      client.beginBody();
      client.print(body);
    }
    client.endRequest();

    ESResponse result;
    result.statusCode = client.responseStatusCode();
    result.body = client.responseBody();
#ifdef LOGGING
    Serial.println("Response status code: " + result.statusCode);
    Serial.println("Response body: " + result.body);
#endif    
    return result;
}

// Elasticsearch Index API
// @see https://www.elastic.co/guide/en/elasticsearch/reference/current/docs-index_.html
ESResponse ESClient::index(String index, String body){
    return sendRequest("post", "/" + index + "/_doc", body);
}

// Elasticsearch Get API
// @see https://www.elastic.co/guide/en/elasticsearch/reference/current/docs-get.html
ESResponse ESClient::get(String index, String id){
    return sendRequest("get", "/" + index + "/_doc/" + id);
}

// Elasticsearch Delete API
// @see https://www.elastic.co/guide/en/elasticsearch/reference/current/docs-delete.html
ESResponse ESClient::del(String index, String id){
    return sendRequest("delete", "/" + index + "/_doc/" + id);
}

// Elasticsearch Update API
// @see https://www.elastic.co/guide/en/elasticsearch/reference/current/docs-update.html
ESResponse ESClient::update(String index, String id, String body){
    return sendRequest("post", "/" + index + "/_update/" + id, body);
}

// Elasupdateticsearch Search API
// @see https://www.elastic.co/guide/en/elasticsearch/reference/current/search-search.html
ESResponse ESClient::search(String index, String body){
    return sendRequest("post", "/" + index + "/_search", body);
}