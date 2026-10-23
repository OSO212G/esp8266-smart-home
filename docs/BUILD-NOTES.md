# Build notes

## Compile-only baseline — 23 October 2026

The sanitized sketch compiled and linked successfully with placeholder configuration. No source changes were needed for that successful build. No new build was run for this documentation revision.

| Component | Tested version / target |
|---|---|
| Board FQBN | `esp8266:esp8266:nodemcuv2` |
| Arduino CLI | 1.4.1 |
| ESP8266 Arduino core | 3.1.2 |
| Xtensa tool package | 3.1.0-gcc10.3-e5f9fec (GCC 10.3.0) |
| DHT11 | 2.1.0 |
| LiquidCrystal I2C | 1.1.2 |
| Firebase Arduino Client Library for ESP8266 and ESP32 | 4.4.17 |

`Wire`, `Servo`, `ESP8266WiFi` and `ESP8266HTTPClient` came from the ESP8266 core. Firebase must supply `Firebase_ESP_Client.h`, `addons/TokenHelper.h` and `addons/RTDBHelper.h`; DHT11 must supply `setDelay`, `readTemperatureHumidity` and `getErrorString`. Similarly named libraries are not necessarily compatible. These are tested versions, not a claim that they are current recommendations.

Sketch SHA-256: `f2df879f82dfab5d9fc3d76862824d657d9e9de2ad559b4fc265c3c4749a7238`.

The build used a `config.h` byte-identical to `config.example.h`. Toolchain packages and generated binaries are not included.

## Reproduction

Make the pinned platform and libraries available in an existing Arduino CLI installation. Copy `firmware/SmartHomePrototype/config.example.h` to `firmware/SmartHomePrototype/config.h` without replacing placeholders, then run:

```sh
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 --warnings all firmware/SmartHomePrototype
```

Compilation needs neither hardware nor a real Firebase project. Fresh-machine installation, package availability and cross-platform reproduction were not tested.

The successful Windows build needed existing short-path aliases and a per-command `compiler.path` override because GCC internal include lookup failed under long paths. That machine-specific override is not portable. Use an appropriately short local workspace/toolchain root and establish the correct compiler path if the issue occurs. Neither firmware nor compiler headers were patched.

## Warnings and resources

The build recorded 603 warning lines, including dependency unused-parameter diagnostics and LiquidCrystal I2C's AVR-only architecture metadata. No warning suppression was added; compilation does not establish LCD runtime compatibility.

| Build metric | Reported usage |
|---|---|
| RAM globals/constants | 38,432 / 80,192 bytes (47%) |
| Instruction RAM, including reserved instruction cache | 62,435 / 65,536 bytes (95%) |
| Flash code | 514,060 / 1,048,576 bytes (49%) |

These build-tool figures do not establish runtime headroom or stability.

## GPIO assignments — wiring untested

| Signal | GPIO in sketch | NodeMCU label |
|---|---|---|
| DHT11 | 2 | D4 |
| Active-low motion/proximity input | 16 | D0 |
| LED output | 13 | D7 |
| Fan-driver control output | 12 | D6 |
| Servo signal | 0 | D3 |

The LCD is constructed as `LiquidCrystal_I2C(0x27, 16, 2)`; SDA/SCL are not explicitly assigned. LED/fan comments incorrectly mention GPIO4/GPIO5, and a servo comment mentions pin 18. Use the assignments above to inspect the code, not as a validated wiring diagram. Polarity, logic levels, power/driver design, boot-pin behavior and safe states remain unverified.
