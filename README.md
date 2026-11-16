# ESP32 Weather Station

A portable weather station built on an ESP32 and ST7789 display using data from OpenWeatherMap.

The project displays current weather conditions and a multi-day forecast while operating from battery power. It includes battery monitoring, adjustable display brightness, deep sleep mode, manual refresh controls, and automatic weather updates.

---

## Features

### Current Conditions

- Current temperature
- Current weather conditions
- Relative humidity
- Location display
- Battery level indicator

### Forecast

- Five-day forecast
- Daily low temperature
- Daily high temperature
- Weather conditions
- Day-of-week display

### Power Management

- Battery voltage monitoring
- Graphical battery level indicator
- Multiple brightness levels
- Deep sleep mode
- Wake-on-button press

### Connectivity

- Wi-Fi auto-connect
- OpenWeatherMap integration
- Manual refresh
- Automatic refresh every 15 minutes

---

## Hardware

### Tested Hardware

- ESP32 Dev Module
- ESP32 T-Display
- Tenstar ESP32 Display Board
- ST7789 135x240 LCD

### Display Pins

```cpp
#define TFT_CS     5
#define TFT_DC    16
#define TFT_RST   23
#define TFT_MOSI  19
#define TFT_SCLK  18
#define TFT_BL     4
```

### Buttons

```cpp
#define BTN_SCREEN   35
#define BTN_REFRESH   0
```

---

## Display Layout

### Current Conditions

```text
Dallas, TX

72°F

Clear

Humidity: 48%
```

### Forecast

```text
Mon  58/75  Clear
Tue  61/78  Clouds
Wed  65/82  Rain
Thu  67/84  Clear
Fri  69/86  Clear
```

---

## Controls

### DIM / SLEEP Button

Short Press

```text
100%
50%
25%
10%
```

Long Press (2 Seconds)

```text
Deep Sleep
```

Press the button again to wake the device.

### REFRESH Button

Short Press

```text
Immediately download
latest weather data
```

---

## Battery Indicator

Battery level is displayed using a graphical battery icon:

```text
[████████]
```

Color coding:

### Green

```text
50% - 100%
```

### Yellow

```text
25% - 49%
```

### Red

```text
0% - 24%
```

---

## Automatic Updates

The weather data automatically refreshes every:

```text
15 minutes
```

Manual refresh is available at any time using the refresh button.

---

## Weather Data Provider

This project uses:

### OpenWeatherMap

Current conditions and forecast data are retrieved from OpenWeatherMap.

Website:

https://openweathermap.org

Please review OpenWeatherMap API terms and limits before deploying multiple devices.

---

## Project Structure

```text
Weather.ino

├── Wi-Fi Management
├── OpenWeatherMap API
├── JSON Parsing
├── Forecast Processing
├── Battery Monitoring
├── Display Rendering
├── Brightness Control
└── Deep Sleep Management
```

---

## Future Enhancements

Potential future additions:

- Weather icons
- Wind speed and direction
- Sunrise and sunset times
- Air quality index
- UV index
- Severe weather alerts
- GPS location support
- Multiple city support
- Historical weather logging
- SD card data storage

---

## Screenshots

Add photos of the device in operation here.

### Main Weather Screen

```text
Dallas, TX

72°F

Clear

Humidity: 48%

Mon 58/75 Clear
Tue 61/78 Clouds
Wed 65/82 Rain
Thu 67/84 Clear
Fri 69/86 Clear
```

---

## License

MIT License

Copyright (c) 2026 Christian Brannon

See the LICENSE file for details.
