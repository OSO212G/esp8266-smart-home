# Security and limitations

This is prototype source, not a production installation or a safety-certified controller. Compilation/linking does not repair its runtime behavior.

## Configuration and cloud access

Only a non-working placeholder header is supplied. Keep local `config.h` and real credentials out of version control. Do not publish network identifiers, passwords, cloud configuration or serial output containing private information.

The code attempts anonymous Firebase sign-up. No validated database rules, device authorization model, quota behavior or successful cloud write is demonstrated. Use controlled, authorized test resources and review access rules before any network experiment. Compilation did not contact a real Firebase project.

## Preserved defects and unknowns

- **Unbounded network wait:** setup waits for Wi-Fi indefinitely, before LCD/servo initialization completes. No timeout or offline recovery is implemented.
- **Invalid readings:** telemetry writes precede checking the DHT read result. Failed readings can upload initialized or invalid values, and the first result does not gate the fan decision.
- **Blocking control:** the cooling loop does not service normal motion polling or outer telemetry writes while temperature stays above 24°C. A read error breaks the loop and turns outputs off without a defined safe-state policy. A further 15-second delay and normal pacing delay input handling.
- **Servo behavior:** the active-low branch repeatedly commands 0° and commands 165° after its interval. The no-motion branch does not explicitly restore the servo. Physical movement and safe defaults were not tested.
- **Source cleanup, not hardware validation:** GPIO13/D7, GPIO12/D6 and servo GPIO0/D3 comments now match the declarations. The cooling-exit diagnostic distinguishes sensor error from a successful reading at or below 24°C. These corrections do not establish safe wiring or repair the blocking/error policy. See [interfaces](BUILD-NOTES.md#interfaces--conceptual-not-validated-wiring) and [control flow](CONTROL-FLOW.md).
- **Display and sensor timing:** some messages exceed 16 columns; stale text is not consistently cleared. DHT11 `setDelay(100)` compiles but timing and accuracy were not verified.
- **Hardware design:** there is no validated schematic, driver/power design, calibration, boot-pin check or tested actuator safe state. Do not power a fan or motor from GPIO or connect mains/unattended loads based on this source.
- **Build warnings and memory:** dependency warnings, AVR-only LCD metadata and high reported instruction RAM usage remain. Runtime compatibility and memory stability are unproven.

Before any hardware use, resolve the relevant defects and validate wiring, power, drivers, boot-pin behavior and safe states in controlled tests. Claims of working hardware, robust control or secure cloud integration require separate supporting tests for network failures, invalid sensor readings, persistent heat, motion changes during blocking waits, servo defaults and authorized cloud rules. None of these runtime tests is claimed here.
