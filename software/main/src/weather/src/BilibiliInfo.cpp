#include "ESPWiFi.h"
#include <WiFiClient.h>
#include "BilibiliInfo.h"

BilibiliInfo::BilibiliInfo() {

}

void BilibiliInfo::updateFansState(BilibiliInfoData *info, String uid) {
  doUpdate(info, "/x/relation/stat?vmid=" + uid);
}

void BilibiliInfo::doUpdate(BilibiliInfoData *info, String path) {
  unsigned long lostTest = 10000UL;
  unsigned long lost_do = millis();

  this->info = info;
  JsonStreamingParser parser;
  parser.setListener(this);
  Serial.printf("[HTTP] Requesting resource at http://%s:%u%s\n", host.c_str(), port, path.c_str());

  WiFiClient client;
  if(client.connect(host, port)) {
    bool isBody = false;
    char c;
    Serial.println("[HTTP] connected, now GETting data");
    client.print("GET " + path + " HTTP/1.1\r\n"
                 "Host: " + host + "\r\n"
                 "Connection: close\r\n\r\n");

    while (client.connected() || client.available()) {
      if (client.available()) {
        if ((millis() - lost_do) > lostTest) {
          Serial.println("[HTTP] lost in client with a timeout");
          client.stop();
          ESP.restart();
        }
        c = client.read();
        if (c == '{' || c == '[') {
          isBody = true;
        }
        if (isBody) {
          parser.parse(c);
        }
      }
      // give WiFi and TCP/IP libraries a chance to handle pending events
      yield();
    }
    client.stop();
  } else {
    Serial.println("[HTTP] failed to connect to host");
  }
  this->info = nullptr;
}

void BilibiliInfo::whitespace(char c) {
  Serial.println("whitespace");
}

void BilibiliInfo::startDocument() {
  Serial.println("start document");
}

void BilibiliInfo::key(String key) {
  currentKey = String(key);
}

void BilibiliInfo::value(String value) {
  // Serial.printf("Key: %s, value: %s\n", currentKey.c_str(), value.c_str());
  //uint64_t timestamp;

  if (currentParent == "data") {

	  if (currentKey == "following") {
		this->info->following_number = value.toInt();
	  } else if (currentKey == "follower") {
		this->info->fans_number = value.toInt();
	  }
  
  }
}

void BilibiliInfo::endArray() {

}


void BilibiliInfo::startObject() {
  currentParent = currentKey;
}

void BilibiliInfo::endObject() {
  currentParent = "";
}

void BilibiliInfo::endDocument() {

}

void BilibiliInfo::startArray() {

}

