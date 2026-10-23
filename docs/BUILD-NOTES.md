# Build notes

[Project overview](../README.md) · [Sketch](../firmware/SmartHomePrototype/SmartHomePrototype.ino) · [Control flow](CONTROL-FLOW.md) · [Limitations](SECURITY-AND-LIMITATIONS.md)

## Pinned toolchain

| Component | Tested version / identifier |
|---|---|
| Arduino CLI | 1.4.1 |
| ESP8266 core | `esp8266:esp8266@3.1.2` |
| Board | NodeMCU 1.0 (ESP-12E Module), `esp8266:esp8266:nodemcuv2` |
| Xtensa tool package | 3.1.0-gcc10.3-e5f9fec (GCC 10.3.0) |
| DHT11 | `DHT11@2.1.0` |
| LCD | `LiquidCrystal I2C@1.1.2` |
| Firebase | `Firebase Arduino Client Library for ESP8266 and ESP32@4.4.17` |

Package/library identifiers were checked against the downloaded Arduino indices used for verification. These are the tested pins, not a latest-version recommendation. `Wire`, `Servo`, `ESP8266WiFi` and `ESP8266HTTPClient` come with the core. Do not substitute a similarly named DHT or Firebase library: this sketch uses DHT11's `setDelay`, `readTemperatureHumidity` and `getErrorString`, plus Firebase's `Firebase_ESP_Client.h` and addon headers.

## Setup from the repository root

Prerequisite: Arduino CLI 1.4.1 on `PATH`. The following is a **fresh-machine procedure not exercised end-to-end in this revision**. The actual revised-source build reused the previously downloaded, pinned Windows environment; no package downloads or reinstalls were needed.

The commands below work in a POSIX shell or PowerShell. They download package indices, the pinned core/tools and libraries; they do not contact a Firebase project or upload firmware.

```sh
arduino-cli core update-index --additional-urls "https://arduino.esp8266.com/stable/package_esp8266com_index.json"
arduino-cli core install "esp8266:esp8266@3.1.2" --additional-urls "https://arduino.esp8266.com/stable/package_esp8266com_index.json"
arduino-cli lib update-index
arduino-cli lib install "DHT11@2.1.0"
arduino-cli lib install "LiquidCrystal I2C@1.1.2"
arduino-cli lib install "Firebase Arduino Client Library for ESP8266 and ESP32@4.4.17"
```

Copy the template **without changing its placeholders**, choosing your shell:

```sh
# Linux / macOS / Git Bash
cp firmware/SmartHomePrototype/config.example.h firmware/SmartHomePrototype/config.h
```

```powershell
# Windows PowerShell
Copy-Item firmware/SmartHomePrototype/config.example.h firmware/SmartHomePrototype/config.h
```

```bat
:: Windows Command Prompt
copy firmware\SmartHomePrototype\config.example.h firmware\SmartHomePrototype\config.h
```

Then compile (no device or working credentials required):

```sh
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 --warnings all --jobs 4 firmware/SmartHomePrototype
```

`config.h` is the header used by the sketch and is ignored by Git; an `.env` file is not used. Never put real configuration in the example header or commit local credentials. No upload or device-discovery step is part of this procedure.

### Windows long-path caveat

The verified Windows environment needed existing 8.3 aliases and a per-command override for GCC's **entire** compiler root, after long paths caused internal-header lookup failures. That private machine path is deliberately not included here. With an existing compiler installation, PowerShell can resolve its alias:

```powershell
# Replace this example with your installed bin directory; retain its trailing slash.
$bin = 'C:\short-arduino-data\packages\esp8266\tools\xtensa-lx106-elf-gcc\3.1.0-gcc10.3-e5f9fec\bin'
$shortBin = (New-Object -ComObject Scripting.FileSystemObject).GetFolder($bin).ShortPath.Replace('\', '/') + '/'
arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 --warnings all --jobs 4 --build-property "compiler.path=$shortBin" firmware/SmartHomePrototype
```

This generic alias-resolution example was not executed as a fresh-machine recipe. It requires existing aliases; it does not create them or change Windows policy. Prefer a short workspace/data root, and verify the actual local path rather than copying someone else's override. Compiler headers and firmware control logic were not patched to fix path lookup.

## Revised-source build evidence — 23 October 2026

- Result: **compile and link succeeded, exit 0**, 90.504 seconds within a 180-second bound.
- Source: [SmartHomePrototype.ino](../firmware/SmartHomePrototype/SmartHomePrototype.ino), SHA-256 `428ff893a61211b55988e3ff4b26b32eae79fbe66d6f852819478ab1ebdc268e`.
- `config.h` was byte-identical to [config.example.h](../firmware/SmartHomePrototype/config.example.h), placeholders only.
- Target/toolchain: the exact pins above; `--warnings all --jobs 4`; Windows compiler-root alias override as described above.
- Existing task-local `elf2bin.py` / `sizes.py` launch adaptations were reused unchanged for hidden, captured, time-bounded helper processes. No compiler binaries, library code or firmware behavior were altered by those launch adaptations.
- Full diagnostics and binary artifacts were retained privately; neither raw logs nor device binaries are published. No upload, runtime, hardware or Firebase operation was tested.

The original public baseline remains in history at `7e4b3e906b3258f8c598791642f85d772f904d30`. The revised source corrects three pin/attach comments, removes unused `yes`/`no` strings, fixes nested indentation and distinguishes a cooling-read error from a successful ≤24°C exit. Thresholds, GPIO polarity, servo writes and blocking delays are unchanged.

### Warnings and build metrics

There were **603 warning lines**, not 603 distinct defects: dependency unused-parameter diagnostics and LiquidCrystal I2C's AVR-only architecture metadata remain. No warning suppression was added. LCD runtime compatibility is not established by compilation.

| Build metric | Reported usage |
|---|---|
| RAM globals/constants | 38,464 / 80,192 bytes (47%) |
| Instruction RAM, including reserved instruction cache | 62,435 / 65,536 bytes (95%) |
| Flash code | 514,076 / 1,048,576 bytes (49%) |

These are build-tool figures, not measured runtime headroom or performance.

## Local source checks

With Python 3, run from the repository root:

```sh
python tests/check_source.py
```

Two narrow source-contract checks verify GPIO comments and the guarded exit diagnostic. Both failed against their corresponding baseline defects before the fixes, then passed. They are not firmware execution or full runtime coverage. A private token comparison also confirmed that executable changes are limited to the unused declarations and serial-message guard.

## Interfaces — conceptual, not validated wiring

| Signal | GPIO / NodeMCU label | Code interface |
|---|---|---|
| DHT11 data | 2 / D4 | `DHT11 dht11(2)` |
| Active-low detection input | 16 / D0 | `INPUT`, sampled with `digitalRead` |
| LED control | 13 / D7 | HIGH in the cooling branch; LOW on either exit |
| Fan-driver control | 12 / D6 | Same command levels as LED; not a GPIO power supply |
| Servo signal | 0 / D3 | `attach(servoPin)`; writes 0° / 165° |
| LCD | SDA/SCL not explicit in sketch | I2C, `0x27`, 16×2; core/library defaults apply |

GPIO0 and GPIO2 boot behavior, input electrical polarity, logic levels, drivers, power and safe states require hardware validation. See [limitations](SECURITY-AND-LIMITATIONS.md).
