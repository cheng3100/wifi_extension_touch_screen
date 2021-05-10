#pragma once
//#define NIGHTICONS
#include <Arduino.h>
#include <JsonListener.h>
#include <JsonStreamingParser.h>

typedef struct BilibiliInfoData {
	uint32_t fans_number;
	uint32_t following_number;
} BilibiliInfoData;

class BilibiliInfo: public JsonListener {
  private:
    const String host = "http://api.bilibili.com";
    const uint8_t port = 80;
    boolean isMetric = true;
    String currentKey;
    String currentParent;
    BilibiliInfoData *info;


  void doUpdate(BilibiliInfoData *info, String url);

  public:
    BilibiliInfo();
    void updateFansState(BilibiliInfoData *info, String uid);

    virtual void whitespace(char c);

    virtual void startDocument();

    virtual void key(String key);

    virtual void value(String value);

    virtual void endArray();

    virtual void endObject();

    virtual void endDocument();

    virtual void startArray();

    virtual void startObject();
};
