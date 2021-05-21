#include <simpleDSTadjust.h>

// Setup
#define WIFI_SSID ""
#define WIFI_PASS ""
#define WIFI_HOSTNAME "SunWifiX"

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320

typedef struct wifiId_t {
	String ssid;
	String pass;
	String hostName;
} wifiId;

wifiId wifiIdArray[] = {
	{"", "", "SunWifi"},
	{"", "", "SunWifi"}
};

const int UPDATE_INTERVAL_SECS = 6 * 60 * 60; // Update every 6 hours
const int UPDATE_SWITCH_SCREEN_SECS = 60;		  // Update every 30 seconds`
const int SLEEP_INTERVAL_SECS = 0;        // Going to sleep after idle times, set 0 for insomnia

#define SCREEN_COUNT (6)

// bilibili info
#define BILI_UID ""
#define BILI_BVID ""

// OpenWeatherMap Settings
// Sign up here to get an API key: https://docs.thingpulse.com/how-tos/openweathermap-key/
String OPEN_WEATHER_MAP_APP_ID = "";
/*
  Go to https://openweathermap.org/find?q= and search for a location. Go through the
  result set and select the entry closest to the actual location you want to display
  data for. It'll be a URL like https://openweathermap.org/city/2657896. The number
  at the end is what you assign to the constant below.
*/
String OPEN_WEATHER_MAP_LOCATION_ID = "2657896";
String OPEN_WEATHER_MAP_LOCATION = "Shanghai";

// String DISPLAYED_CITY_NAME = "Zurich";
String DISPLAYED_CITY_NAME = "Shanghai";
/*
  Arabic -> ar, Bulgarian -> bg, Catalan -> ca, Czech -> cz, German -> de, Greek -> el,
  English -> en, Persian (Farsi) -> fa, Finnish -> fi, French -> fr, Galician -> gl,
  Croatian -> hr, Hungarian -> hu, Italian -> it, Japanese -> ja, Korean -> kr,
  Latvian -> la, Lithuanian -> lt, Macedonian -> mk, Dutch -> nl, Polish -> pl,
  Portuguese -> pt, Romanian -> ro, Russian -> ru, Swedish -> se, Slovak -> sk,
  Slovenian -> sl, Spanish -> es, Turkish -> tr, Ukrainian -> ua, Vietnamese -> vi,
  Chinese Simplified -> zh_cn, Chinese Traditional -> zh_tw.
*/
String OPEN_WEATHER_MAP_LANGUAGE = "en";

// Adjust according to your language
const String WDAY_NAMES[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
const String MONTH_NAMES[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
const String SUN_MOON_TEXT[] = {"Sun", "Rise", "Set", "Moon", "Age", "Illum"};
const String MOON_PHASES[] = {"New Moon", "Waxing Crescent", "First Quarter", "Waxing Gibbous",
                              "Full Moon", "Waning Gibbous", "Third quarter", "Waning Crescent"
                             };

#define CST_OFFSET +8
#define UTC_OFFSET +0
#define TZ_OFFSET CST_OFFSET
// struct dstRule StartRule = {"CEST", Last, Sun, Mar, 2, 3600}; // Central European Summer Time = UTC/GMT +2 hours
// struct dstRule EndRule = {"CET", Last, Sun, Oct, 2, 0};       // Central European Time = UTC/GMT +1 hour

// China Standard Time have no dst time
struct dstRule StartRule = {"CST", Last, Sun, Mar, 2, 0}; // Central European Summer Time = UTC/GMT +2 hours
struct dstRule EndRule = {"CST", Last, Sun, Mar, 2, 0};       // Central European Time = UTC/GMT +1 hour
// Settings for Boston
// #define UTC_OFFSET -5
// struct dstRule StartRule = {"EDT", Second, Sun, Mar, 2, 3600}; // Eastern Daylight time = UTC/GMT -4 hours
// struct dstRule EndRule = {"EST", First, Sun, Nov, 1, 0};       // Eastern Standard time = UTC/GMT -5 hour

// values in metric or imperial system?
const boolean IS_METRIC = true;

// Change for 12 Hour/ 24 hour style clock
bool IS_STYLE_12HR = false;

// change for different NTP (time servers)
#define NTP_SERVERS "0.ch.pool.ntp.org", "1.ch.pool.ntp.org", "2.ch.pool.ntp.org"
// #define NTP_SERVERS "us.pool.ntp.org", "time.nist.gov", "pool.ntp.org"

// August 1st, 2018
#define NTP_MIN_VALID_EPOCH 1533081600

// Pins for the ILI9341
// #define TFT_CS D9
// #define TFT_RESET D8
// #define TFT_DC D2
// #define TFT_LED D3
//
// #define HAVE_TOUCHPAD
// #define TOUCH_CS D10
// #define TOUCH_IRQ  D4

// for wemos d1
// #define TFT_CS D10 // #define TFT_RESET D4
// #define TFT_DC D2
// #define TFT_LED D3
//
// #define HAVE_TOUCHPAD 0
// #define TOUCH_CS D8
// #define TOUCH_IRQ  D9

// for node mcu
#define TFT_CS D1 // #define TFT_RESET D4
#define TFT_RESET D0
#define TFT_DC D2
#define TFT_LED D4

#define HAVE_TOUCHPAD 1
#define TOUCH_CS D8
#define TOUCH_IRQ  D3

/***************************
   End Settings
 **************************/
