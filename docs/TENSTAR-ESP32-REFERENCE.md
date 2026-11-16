# ESP32 Project Starter Reference

Copy this section into a new chat whenever starting a new ESP32 display project.

---

# Hardware

Board:

```text
Tenstar ESP32 Display Board
```

Arduino IDE Board Profile:

```text
ESP32 Dev Module
```

Display:

```text
ST7789
135x240 TFT LCD
```

Libraries:

```text
Adafruit GFX Library
Adafruit ST7789 Library
ArduinoJson
WiFi
HTTPClient
WiFiClientSecure
```

---

# Display Configuration

```cpp
#define TFT_CS     5
#define TFT_DC    16
#define TFT_RST   23
#define TFT_MOSI  19
#define TFT_SCLK  18
#define TFT_BL     4
```

Display object:

```cpp
SPIClass spi(VSPI);

Adafruit_ST7789 tft(
    &spi,
    TFT_CS,
    TFT_DC,
    TFT_RST);
```

Display setup:

```cpp
spi.begin(
    TFT_SCLK,
    -1,
    TFT_MOSI,
    TFT_CS);

tft.init(135, 240);

tft.setRotation(2);
```

Screen dimensions:

```text
135 x 240
```

Orientation:

```text
USB connector at bottom
Rotation = 2
```

---

# Buttons

Hardware buttons:

```cpp
#define BTN_MODE    35
#define BTN_SCREEN   0
```

BTN_MODE:

```text
Left button
```

BTN_SCREEN:

```text
Right button
```

Button setup:

```cpp
pinMode(
    BTN_MODE,
    INPUT_PULLUP);

pinMode(
    BTN_SCREEN,
    INPUT_PULLUP);
```

Buttons are:

```text
Active LOW
```

Example:

```cpp
if(digitalRead(BTN_MODE) == LOW)
{
    // button pressed
}
```

---

# Battery Monitoring

Battery ADC:

```cpp
GPIO34
```

Typical voltage function:

```cpp
float getBatteryVoltage()
{
    long sum = 0;

    for(int i=0; i<16; i++)
    {
        sum += analogRead(34);
        delay(2);
    }

    float raw =
        sum / 16.0;

    return
        (raw / 4095.0)
        *
        2.0
        *
        3.3;
}
```

Battery range:

```text
4.20V = Full
3.30V = Empty
```

Battery percentage:

```cpp
int batteryPercent(float volts)
{
    if(volts >= 4.20)
        return 100;

    if(volts <= 3.30)
        return 0;

    return
        (int)(
            (volts - 3.30)
            *
            100.0
            /
            (4.20 - 3.30));
}
```

---

# Brightness Control

Backlight pin:

```cpp
TFT_BL = 4
```

PWM setup:

```cpp
ledcAttach(
    TFT_BL,
    5000,
    8);
```

Brightness table:

```cpp
int brightnessLevel = 4;

const uint8_t brightnessTable[] =
{
    0,
    25,
    64,
    128,
    255
};
```

Apply brightness:

```cpp
ledcWrite(
    TFT_BL,
    brightnessTable[
        brightnessLevel]);
```

Brightness levels:

```text
100%
50%
25%
10%
```

---

# Deep Sleep

Include:

```cpp
#include <esp_sleep.h>
```

Sleep function:

```cpp
void enterDeepSleep()
{
    tft.fillScreen(
        ST77XX_BLACK);

    drawText(
        "Sleeping...",
        20,
        110,
        ST77XX_YELLOW,
        1);

    delay(500);

    ledcWrite(
        TFT_BL,
        0);

    esp_sleep_enable_ext0_wakeup(
        GPIO_NUM_0,
        LOW);

    esp_deep_sleep_start();
}
```

Wake:

```text
BTN_SCREEN (GPIO0)
```

Behavior:

```text
Short Press
    Brightness Cycle

Long Press (>2 sec)
    Deep Sleep
```

---

# UI Style Guide

Current visual style:

```text
Black background
White text
Cyan titles
Green battery/status
Yellow highlights
Dark Grey separators
```

Colors:

```cpp
#define DARKGREY 0x7BEF
#define GREENYELLOW 0xAFE5
```

Common title:

```cpp
drawText(
    "TITLE",
    2,
    2,
    ST77XX_CYAN,
    1);
```

Footer divider:

```cpp
tft.drawFastHLine(
    0,
    220,
    135,
    DARKGREY);
```

Footer labels:

```cpp
MODE
DIM/SLP
```

---

# Preferred Architecture

For larger projects:

```text
Project.ino
│
├── setup()
├── loop()
├── WiFi
├── Buttons
└── Power

DataEngine.h
│
├── API Access
├── JSON Parsing
├── Models
└── Calculations

DisplayUI.h
│
├── Screens
├── Drawing
├── Icons
└── Widgets
```

---

# Network Pattern

WiFi setup:

```cpp
WiFi.mode(
    WIFI_STA);

WiFi.begin(
    ssid,
    password);

while(
    WiFi.status() !=
    WL_CONNECTED)
{
    delay(500);
}
```

For HTTPS:

```cpp
WiFiClientSecure client;
client.setInsecure();
```

---

# OpenSky Notes

Current project uses:

```text
OAuth2 Client Credentials
```

Credentials:

```cpp
OPENSKY_CLIENT_ID
OPENSKY_CLIENT_SECRET
```

Authentication flow:

```text
Fetch Access Token
        ↓
Store accessToken
        ↓
Bearer Authorization
        ↓
OpenSky API Request
```

Typical radar configuration:

```text
10 mile range
30 second updates
Aircraft trails
Vector aircraft icons
List view
Radar view
```

---

# Display Helper

Standard helper:

```cpp
void drawText(
    const String& s,
    int x,
    int y,
    uint16_t color,
    uint8_t size)
{
    tft.setTextColor(color);
    tft.setTextSize(size);
    tft.setCursor(x, y);
    tft.print(s);
}
```

---

# Things The AI Should Assume

Unless otherwise specified:

```text
Board = Tenstar ESP32 Display Board
Display = ST7789 135x240
Rotation = 2
Board Profile = ESP32 Dev Module
Battery = 1-cell LiPo
Battery ADC = GPIO34
Backlight = GPIO4
Left Button = GPIO35
Right Button = GPIO0
Buttons = Active LOW
Framework = Arduino IDE
Graphics Library = Adafruit GFX + ST7789
```

When generating code, prefer:

```text
Battery monitoring
Brightness control
Deep sleep
Button navigation
Low memory usage
Modular architecture
```