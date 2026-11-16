#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <esp_sleep.h>

// ==================================================
// WIFI
// ==================================================

const char* ssid     = "ssid";
const char* password = "password";

// ==================================================
// OPENWEATHERMAP
// ==================================================

const char* API_KEY = "API KEY";
const char* CITY    = "City,COUNTRY";

// ==================================================
// DISPLAY PINS
// ==================================================

#define TFT_CS    5
#define TFT_DC    16
#define TFT_RST   23
#define TFT_MOSI  19
#define TFT_SCLK  18
#define TFT_BL    4

#define BTN_SCREEN  35
#define BTN_REFRESH 0

SPIClass spi(VSPI);
Adafruit_ST7789 tft(&spi, TFT_CS, TFT_DC, TFT_RST);

// ==================================================
// COLORS
// ==================================================

#define DARKGREY    0x7BEF
#define GREENYELLOW 0xAFE5

// ==================================================
// WEATHER DATA
// ==================================================

struct ForecastDay
{
  long day;
  float low;
  float high;
  String condition;
};

ForecastDay forecast[6];

int forecastCount = 0;

float currentTemp = 0;
int currentHumidity = 0;
String currentCondition = "--";

long timezoneOffset = 0;

int brightnessLevel = 4;

const uint8_t brightnessTable[] =
{
    0,
    25,
    64,
    128,
    255
};

unsigned long screenButtonStart = 0;

unsigned long lastUpdate = 0;

const char* weekdayNames[] =
{
  "Sun","Mon","Tue",
  "Wed","Thu","Fri","Sat"
};

// ==================================================
// BATTERY DATA
// ==================================================

int bat32 = 0;
int bat33 = 0;
int bat34 = 0;

// ==================================================
// TEXT HELPER
// ==================================================

void drawText(
    String text,
    int x,
    int y,
    uint16_t color,
    uint8_t size)
{
  tft.setTextColor(color);
  tft.setTextSize(size);
  tft.setCursor(x, y);
  tft.print(text);
}

// ==================================================
// BATTERY VOLTAGE
// ==================================================

float getBatteryVoltage()
{
    int raw = analogRead(34);

    // Common T-Display voltage divider
    float voltage =
        ((float)raw / 4095.0) *
        2.0 *
        3.3 *
        1.1;

    return voltage;
}

int batteryPercent(float volts)
{
    if (volts >= 4.20)
        return 100;

    if (volts <= 3.30)
        return 0;

    return (int)(
        (volts - 3.30) *
        100.0 /
        (4.20 - 3.30));
}

void drawBattery(int percent)
{
    int x = 100;
    int y = 2;

    tft.drawRect(x, y, 24, 10, ST77XX_WHITE);
    tft.fillRect(x + 24, y + 3, 2, 4, ST77XX_WHITE);

    int width = map(percent, 0, 100, 0, 22);

    uint16_t color = ST77XX_GREEN;

    if (percent < 25)
        color = ST77XX_RED;
    else if (percent < 50)
        color = ST77XX_YELLOW;

    tft.fillRect(x + 1, y + 1, width, 8, color);
}

// ==================================================
// JSON FETCH
// ==================================================

bool getJson(
    String url,
    JsonDocument &doc,
    JsonDocument *filter = nullptr)
{
  HTTPClient http;

  http.useHTTP10(true);

  if (!http.begin(url))
    return false;

  int code = http.GET();

  if (code != 200)
  {
    http.end();
    return false;
  }

  DeserializationError err;

  if (filter)
  {
    err = deserializeJson(
        doc,
        http.getStream(),
        DeserializationOption::Filter(*filter));
  }
  else
  {
    err = deserializeJson(
        doc,
        http.getStream());
  }

  http.end();

  return !err;
}

// ==================================================
// FETCH WEATHER
// ==================================================

void fetchWeather()
{

  if (WiFi.status() != WL_CONNECTED)
    return;

  String base =
      "http://api.openweathermap.org/data/2.5/";

  String tail =
      String("?q=") +
      CITY +
      "&units=imperial&appid=" +
      API_KEY;

  JsonDocument doc;

  // -------------------------------------------
  // CURRENT CONDITIONS
  // -------------------------------------------

  if (getJson(base + "weather" + tail, doc))
  {
    currentTemp =
        doc["main"]["temp"]
            .as<float>();

    currentHumidity =
        doc["main"]["humidity"]
            .as<int>();

    currentCondition =
        doc["weather"][0]["main"]
            .as<String>();

    timezoneOffset =
        doc["timezone"]
            .as<long>();
  }

  // -------------------------------------------
  // FORECAST
  // -------------------------------------------

  JsonDocument filter;

  filter["list"][0]["dt"] = true;
  filter["list"][0]["main"]["temp_min"] = true;
  filter["list"][0]["main"]["temp_max"] = true;
  filter["list"][0]["weather"][0]["main"] = true;

  doc.clear();

  if (!getJson(
          base + "forecast" + tail,
          doc,
          &filter))
  {
    return;
  }

  forecastCount = 0;

  for (JsonObject item :
       doc["list"].as<JsonArray>())
  {
    long localTime =
        item["dt"].as<long>() +
        timezoneOffset;

    long day =
        localTime / 86400;

    long hour =
        (localTime % 86400) / 3600;

    float low =
        item["main"]["temp_min"];

    float high =
        item["main"]["temp_max"];

    String condition =
        item["weather"][0]["main"]
            .as<String>();

    if (forecastCount == 0 ||
        forecast[forecastCount - 1].day != day)
    {
      if (forecastCount >= 6)
        break;

      forecast[forecastCount++] =
      {
        day,
        low,
        high,
        condition
      };
    }

    ForecastDay &d =
        forecast[forecastCount - 1];

    d.low = min(d.low, low);
    d.high = max(d.high, high);

    if (hour >= 12 && hour <= 15)
      d.condition = condition;
  }
}

// ==================================================
// DRAW SCREEN
// ==================================================

void drawScreen()
{
  tft.fillScreen(ST77XX_BLACK);

  float battV = getBatteryVoltage();
  int battPct = batteryPercent(battV);

  drawBattery(battPct);
      
  drawText(
      "Dallas, TX",
      4,
      4,
      ST77XX_WHITE,
      1);

  drawText(
      String(currentTemp, 0) + "F",
      4,
      22,
      ST77XX_CYAN,
      3);

  drawText(
      currentCondition,
      4,
      58,
      ST77XX_WHITE,
      1);

  drawText(
      "Humidity: " + String(currentHumidity) + "%",
      4,
      72,
      ST77XX_WHITE,
      1);

  tft.drawFastHLine(
      0,
      92,
      135,
      DARKGREY);

  int start =
      (forecastCount > 5)
          ? 1
          : 0;

  for (int i = 0;
       i < 5 &&
       start + i < forecastCount;
       i++)
  {
    ForecastDay &d =
        forecast[start + i];

    int y =
        104 +
        (i * 24);

    drawText(
        weekdayNames[(d.day + 4) % 7],
        2,
        y,
        ST77XX_YELLOW,
        1);

    drawText(
        String((int)round(d.low)) +
        "/" +
        String((int)round(d.high)),
        38,
        y,
        ST77XX_WHITE,
        1);

    drawText(
        d.condition,
        78,
        y,
        GREENYELLOW,
        1);
  }
    
    drawText(
        "DIM/SLP",
        5,
        228,
        DARKGREY,
        1);

    drawText(
        "REFRESH",
        82,
        228,
        DARKGREY,
        1);
}

// ==================================================

void enterDeepSleep()
{
    tft.fillScreen(ST77XX_BLACK);

    drawText(
        "Sleeping...",
        25,
        110,
        ST77XX_YELLOW,
        1);

    delay(500);

    ledcWrite(
        TFT_BL,
        0);

    esp_sleep_enable_ext0_wakeup(
        GPIO_NUM_35,
        LOW);

    esp_deep_sleep_start();
}

// ==================================================
// SETUP
// ==================================================

void setup()
{
  Serial.begin(115200);

  esp_sleep_wakeup_cause_t wakeup =
      esp_sleep_get_wakeup_cause();

    if(wakeup ==
    ESP_SLEEP_WAKEUP_EXT0)
    {
        Serial.println(
            "Wake From Deep Sleep");
    }

  analogReadResolution(12);

  pinMode(BTN_REFRESH, INPUT_PULLUP);
  pinMode(BTN_SCREEN, INPUT_PULLUP);

  ledcAttach(TFT_BL, 5000, 8);

  ledcWrite(
    TFT_BL,
    brightnessTable[4]);

  spi.begin(
      TFT_SCLK,
      -1,
      TFT_MOSI,
      TFT_CS);

  tft.init(135, 240);
  tft.setRotation(2);

  tft.fillScreen(ST77XX_BLACK);

  drawText(
      "Connecting WiFi...",
      10,
      110,
      ST77XX_WHITE,
      1);

  WiFi.mode(WIFI_STA);
  WiFi.begin(
      ssid,
      password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  fetchWeather();
  drawScreen();

  lastUpdate = millis();
}

// ==================================================
// LOOP
// ==================================================

void loop()
{
    // Physical LEFT button now controls screen
    if(digitalRead(BTN_REFRESH) == LOW)
    {
        if(screenButtonStart == 0)
        {
            screenButtonStart =
                millis();
        }

        if(
            millis() -
            screenButtonStart >
            2000)
        {
            enterDeepSleep();
        }
    }
    else
    {
        if(screenButtonStart != 0)
        {
            unsigned long held =
                millis() -
                screenButtonStart;

            screenButtonStart = 0;

            if(held < 2000)
            {
                brightnessLevel--;

                if(brightnessLevel < 1)
                {
                    brightnessLevel = 4;
                }

                ledcWrite(
                    TFT_BL,
                    brightnessTable[
                        brightnessLevel]);
            }
        }
    }

  // Physical RIGHT button now refreshes
  if (digitalRead(BTN_SCREEN) == LOW)
  {
      fetchWeather();
      drawScreen();

      lastUpdate = millis();

      delay(300);
  }

  if (millis() - lastUpdate > 900000UL)
  {
    fetchWeather();
    drawScreen();

    lastUpdate = millis();
  }
}