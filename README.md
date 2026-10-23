# ESP8266 Smart Home Prototype

Developed by Abdulla Salem.

An ESP8266 prototype combining temperature/humidity sensing, an active-low motion/proximity input, an I2C LCD, a servo, LED/fan outputs and Firebase telemetry code. **Compile-verified source, not a production home controller.** Known application defects are preserved and documented.

## Implemented behavior

- DHT11 temperature/humidity readings and a 16×2 LCD display.
- Active-low digital detection with servo commands to 0° and, after an interval, 165°.
- Within the active-low detection branch, LED/fan outputs turn on above 25°C, followed by a blocking wait until temperature is at most 24°C or a sensor error occurs.
- Wi-Fi connection, attempted anonymous Firebase sign-up and writes to `sensor/temperature` and `sensor/humidity`.

These describe the source, not demonstrated physical operation. No LDR input, remote application, energy measurement or verified air-conditioner control is implemented in this sketch.

## Verification

The unchanged sanitized sketch compiled and linked on **23 October 2026** for `esp8266:esp8266:nodemcuv2`, using Arduino CLI **1.4.1**, ESP8266 core **3.1.2**, DHT11 **2.1.0**, LiquidCrystal I2C **1.1.2** and Firebase Arduino Client Library for ESP8266 and ESP32 **4.4.17**. Configuration contained placeholders only. Dependency warnings remain, including AVR-only LCD metadata; reported instruction RAM usage was 95%.

A successful build does **not** prove sensor accuracy, wiring, actuator behavior, cloud operation, electrical safety or runtime stability. No firmware upload, hardware operation or real cloud connection was performed in that verification. See [build notes](docs/BUILD-NOTES.md) for dependencies, GPIO assignments and reproduction limits.

## Compile-only setup

```text
firmware/SmartHomePrototype/
  SmartHomePrototype.ino
  config.example.h
```

Copy `config.example.h` to `config.h` in the same directory, leaving placeholders unchanged. `config.h` is ignored and is the configuration header actually used by the sketch; an `.env` file is not used. With the pinned dependencies installed, run from the repository root:

```sh
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 --warnings all firmware/SmartHomePrototype
```

No hardware or usable credentials are needed for compilation. This is not an upload command or a fresh-machine installation guarantee.

## Limitations

Unbounded Wi-Fi connection wait, invalid sensor uploads, blocking cooling/15-second waits and incorrect pin comments remain. Read [security and limitations](docs/SECURITY-AND-LIMITATIONS.md). Do not connect mains appliances or unattended loads based on this source.

## Dependencies

ESP8266 Arduino core (including Wire, Servo, ESP8266WiFi and ESP8266HTTPClient), DHT11, LiquidCrystal I2C and Firebase Arduino Client Library for ESP8266 and ESP32 are separate third-party dependencies. Their work is acknowledged; their packages are not bundled. Exact tested versions and required APIs are in the build notes.
