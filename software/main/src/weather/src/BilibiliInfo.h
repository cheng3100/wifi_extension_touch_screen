#pragma once
//#define NIGHTICONS
#include <Arduino.h>
#include <JsonListener.h>
#include <JsonStreamingParser.h>

typedef struct BilibiliInfoData {
	uint32_t fans_number;
	uint32_t following_number;
	uint32_t view_number;
	uint32_t like_number;
	uint32_t coins_number;
	uint32_t fav_number;
	uint32_t share_number;
	uint32_t reply_number;
	uint32_t danmaku_number;
} BilibiliInfoData;

typedef enum BiliInfoSource {
	FANS_INFO,
	VIDEO_INFO
} BiliInfoSource;

class BilibiliInfo: public JsonListener {
  private:
    const String host = "api.bilibili.com";
    const uint8_t port = 80;
    boolean isMetric = true;
    String currentKey;
    String currentParent;
    BilibiliInfoData *info;
	uint32_t infoSource;


  void doUpdate(BilibiliInfoData *info, String url);

  public:
    BilibiliInfo();
    void updateFansState(BilibiliInfoData *info, String uid);
    void updateVideoState(BilibiliInfoData *info, String bvid);

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
