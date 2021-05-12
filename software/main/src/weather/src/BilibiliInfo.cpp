#include "ESPWiFi.h"
#include <WiFiClient.h>
#include "BilibiliInfo.h"

#define BILI_DEBUG 0

#if BILI_DEBUG == 1
#define DP(fmt, args...) Serial.printf(fmt, ##args)
#else
#define DP(fmt, args...)
#endif


BilibiliInfo::BilibiliInfo() {

}

void BilibiliInfo::updateFansState(BilibiliInfoData *info, String uid) {
  this->infoSource = FANS_INFO;
  doUpdate(info, "/x/relation/stat?vmid=" + uid);
}

void BilibiliInfo::updateVideoState(BilibiliInfoData *info, String bvid) {
  this->infoSource = VIDEO_INFO;
  doUpdate(info, "/x/web-interface/archive/stat?bvid=" + bvid);
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
  // Serial.printf("Key: %s, Parent: %s, value: %s\n", currentKey.c_str(), currentParent.c_str(), value.c_str());
  DP("Key: %s, Parent: %s, value: %s\n", currentKey.c_str(), currentParent.c_str(), value.c_str());
	
  if (infoSource == FANS_INFO 
	  &&currentParent == "data") {
	  if (currentKey == "following") {
		this->info->following_number = value.toInt();
	  } else if (currentKey == "follower") {
		this->info->fans_number = value.toInt();
	  }
  
  } else if (infoSource == VIDEO_INFO
		      && currentParent == "data") {
	  if (currentKey == "view")  {
		this->info->view_number = value.toInt(); 
	  } else if (currentKey == "favorite") {
		this->info->fav_number = value.toInt();
	  } else if (currentKey == "coin") {
		this->info->coins_number = value.toInt();
	  } else if (currentKey == "like") {
		this->info->like_number = value.toInt();
	  } else if (currentKey == "danmaku") {
		this->info->danmaku_number = value.toInt();
	  } else if (currentKey == "share") {
		this->info->share_number = value.toInt();
	  } else if (currentKey == "reply") {
		this->info->reply_number = value.toInt();
	  }
  
  }
}

void BilibiliInfo::endArray() {
	DP("End array\n");
}


void BilibiliInfo::startObject() {
	DP("Start obj\n");
  currentParent = currentKey;
}

void BilibiliInfo::endObject() {
  DP("end obj\n");
  currentParent = "";
}

void BilibiliInfo::endDocument() {
  DP("end doc\n");

}

void BilibiliInfo::startArray() {
  DP("start array\n");
}

