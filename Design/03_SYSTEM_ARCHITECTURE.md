# 03 — Firmware and System Architecture (SAD)

**State:** DRAFT / proposed architecture / 2026-11-23.  
**Baseline concept:** a small, deterministic safety-first ESP32 firmware, not an unreviewed wholesale fork of ESP-FLUTTER. No firmware exists in the target repository at the time of this proposal.

## 1. System context
~~~text
[Phone Web Cockpit] -- local Wi-Fi AP --> [ESP32 Firmware] --> [Left HV Servo] --> [Left wing]
                                            |             --> [Right HV Servo] --> [Right wing]
                                            |
                                     [Battery ADC]
                                            |
                                      [Fault/Telemetry]
Future: [ESP-NOW / ELRS RC] --- ControlSource arbitration ---^
~~~

**Trust boundaries:** phone/Wi-Fi is untrusted input; only the device may command actuators; power hardware is not controlled by HTTP; unsafe mechanical states cannot be fixed by UI alone.

## 2. Component boundaries
| Component | Responsibility | Explicitly does not own |
| --- | --- | --- |
| App/bootstrap | Initialize in safe order; boot DISARMED; version | Motion algorithm |
| Board HAL | GPIO maps, ADC, PWM servo driver, monotonic clock | Flight business logic |
| Input adapters | Decode/validate WebSocket messages; future RC packets | Write PWM directly |
| Control session manager | One active owner, monotonic seq, packet freshness, authorization | Servo mixing |
| Safety supervisor | Arm gate, STOP priority, watchdog, fault latches, battery health | HTML or wire packet parsing |
| Flight state machine | Legal transitions between disarmed, armed idle, active and failsafe | GPIO |
| Motion planner | Phase, waveform, amplitude/frequency, steering mix, soft transition | Network I/O, settings writes |
| Actuator limiter | Mechanical endpoints, neutral, trim, pulse conversion; final clamp | User authentication |
| Battery monitor | Filter and validate ADC reading, warnings, cutoff signaling | Charging or BMS hardware |
| Settings repository | Schema/version, atomic valid save, factory-safe fallback | Live throttle authority |
| Telemetry publisher | Snapshot device-truth + reason codes, rate limiting | Flight-critical scheduling |
| Web UI assets | Responsive cockpit with ack-based state | Safety enforcement |

**Rule:** no network callback, settings handler or UI request can write servo PWM or mutate the actuator driver directly. All commands pass through validation -> arbitration -> safety -> motion -> final limiter -> HAL.

## 3. Data flow and timing
~~~text
Incoming bytes
  -> strict decoder (size/type/finite/ranges)
  -> authenticated control session / owner
  -> monotonic sequence + freshness check
  -> normalized intent queue (last valid pilot axes; priority STOP)
  -> SafetySupervisor (mode/health/stop latch)
  -> MotionPlanner (fixed-tick, elapsed time)
  -> ActuatorLimiter (calibrated limits, polarity, pulse limits)
  -> Servo HAL / wing mechanics

ADC -> BatteryMonitor -> SafetySupervisor -> Telemetry
                      -> UI status + diagnostics
~~~

- **Control source:** M0 uses phone Wi-Fi only. Adding RC requires a new input adapter and source arbitration; no automatic source switching while ACTIVE.
- **Scheduling proposal:** motion tick nominally 100 Hz, pilot messages 20–50 Hz, telemetry 5 Hz, battery polling 5–10 Hz, logs on state transitions. These values are engineering starting points, not demonstrated real-time guarantees.
- **Frequency:** wing flap oscillation proposed up to 4 Hz **only after servo/mechanical characterization**; servo PWM refresh is a *different* parameter and must match servo datasheet. Do not confuse flap frequency with servo PWM rate.
- **Clock:** use monotonic time; wrap-safe unsigned delta arithmetic; limit unusually large dt, avoid numerical instability; never reset phase due to unrelated packet arrival.
- **Concurrency:** single authoritative state owner; queue or atomic snapshot of command intent. No unsynchronized servo command writers across Wi-Fi/ISR/task contexts.
- **Backpressure:** drop stale redundant axis updates, not safety STOP; bound buffer sizes and processing time to prevent UI burst starving motion.

## 4. Safety state machine
~~~mermaid
stateDiagram-v2
  [*] --> DISARMED: boot/reset
  DISARMED --> ARMED_IDLE: explicit ARM + healthy + neutral throttle
  ARMED_IDLE --> ACTIVE: fresh valid nonzero throttle
  ACTIVE --> ARMED_IDLE: valid zero throttle
  ARMED_IDLE --> DISARMED: DISARM
  ACTIVE --> DISARMED: DISARM
  ARMED_IDLE --> FAILSAFE_STOP: timeout / STOP / invalid health
  ACTIVE --> FAILSAFE_STOP: timeout / STOP / invalid health
  FAILSAFE_STOP --> DISARMED: recovery checked + explicit reset
  DISARMED --> FAULT_LATCHED: unrecoverable fault
  FAILSAFE_STOP --> FAULT_LATCHED: persistent fault
  FAULT_LATCHED --> DISARMED: fault cleared + explicit reset
~~~

Note: BOOTING is a startup operational phase with PWM neutral/disabled until initialization confirms safe outputs; it is **not** a state in which commands can arm.

### Transition invariants
- STOP/DISARM is highest priority and *idempotent*. It invalidates the prior active session/control generation and its queued packets.
- ARMED_IDLE never means auto-flapping. Enter ACTIVE only on new fresh pilot command.
- On safety timeout: command zero flap and chosen validated neutral/torque-off behavior; latch until explicit recovery.
- Reconnect and automatic firmware restart always require a **fresh explicit arm**, never implicit session restore.
- A missing sensor/invalid voltage is UNKNOWN, and health policy blocks arm where battery monitoring is required.
- No pending configuration write may delay STOP or the motion tick.

## 5. Proposed v1 command/telemetry contract
Full pilot JSON examples appear in 02_UI_UX.md. The following fields/behaviors are normative for implementation planning:

| Field | Type and range | Validation |
| --- | --- | --- |
| v | integer = 1 | Reject unsupported version |
| type | fixed enum | Reject unknown control type |
| session | opaque active-controller token | Reject absent/expired/other owner |
| seq | unsigned increasing 32-bit | Prevent replay; handle rollover or re-establish session cleanly |
| x | finite float -1..1 | No implicit parsing to zero |
| y | finite float 0..1 | 0 is neutral; no sticky control |
| state | device enum | Device emits; UI cannot choose directly |
| battery_v | finite volts or null | Validity flag separate, no fabricated reading |
| fault | enumerated reason or null | Retain transition evidence |

Recommended message types: pilot.control, pilot.arm, pilot.disarm, pilot.stop, pilot.release, settings.get, settings.update, telemetry, command.ack, error. Control message max payload and rate limits are set before implementation.

**Transport decision (proposal):** WebSocket on local AP for pilot input and telemetry, optional HTTP only for delivering static assets; neither uses cloud services. Enforce a one-pilot lease, reject extra pilot sessions, and reject cross-session replay. The demo reference's UDP/HTTP control endpoints are **not automatically adopted**. WPA2 AP alone is not authorization if others share credentials; session + control authorization and safe defaults are required. Device secrets must not be committed.

### Latency and timeout acceptance
- Target failsafe: no fresh valid controls for 500ms -> transition and stop, independent of browser tab state.
- UI can send immediate release/STOP, but firmware deadline is authoritative.
- Log min/mean/max timing from target board under load; do not claim hard real-time by design alone.

## 6. Settings contract and firmware lifecycle
- Settings structure includes schema_rev, crc/integrity check (or equivalent), board_id, servo_left/right polarity, measured safe limits, battery topology and tuning.
- Version migrations must be explicit; invalid persistent data falls back to **safe disarmed defaults** and blocks uncalibrated flight.
- Pin assignments and battery chemistry are compile-time board-hardware profiles initially; changing these from UI is out of MVP.
- Flash update and rollback are postponed until post-MVP. Avoid OTA while motion is active.
- Firmware build info includes Git SHA, target board, dependency versions, settings schema and build timestamp.

## 7. Architecture Decision Records (proposed)
- **ADR-001:** target board candidate XIAO ESP32-C3; approve only after hardware identification.
- **ADR-002:** ESP32 Arduino + PlatformIO (reuse tooling familiarity but pin deps); retain one verified target env.
- **ADR-003:** browser-local AP cockpit and WebSocket v1; no cloud or Internet flight.
- **ADR-004:** fail-closed state machine with one control owner and independent watchdog.
- **ADR-005:** modular firmware; conceptual learning from ESP-FLUTTER, no direct copy without license decision.
- **ADR-006:** mechanical angle envelope is *measured* on assembled prototype, not guessed from reference constants.

All ADRs are provisional until recorded as ACCEPTED in 06_DECISIONS_AND_ROADMAP.md.

## 8. Test architecture
1. **Host unit tests:** input parsing, stale sequence, invalid numeric, control arbitration, arm gate, STOP precedence, recovery and settings migration with fake clock.
2. **Target hardware tests:** GPIO/ADC/PWM pin verification, servo endpoints, AP reconnect, packet loss, memory usage, brownout behavior, actual control-loop timing.
3. **Integration:** end-to-end Cockpit -> Device ACK -> command state -> telemetry and faults; simulate page hide and Wi-Fi loss.
4. **Mechanical:** no-bind/no-strike, dynamic current, wing symmetry, vibrational stress, heat; later tethered lift/steering and optional free flight.
5. **Evidence:** keep logs, videos, screenshots, test commands, SHA and board profile in PR; unsupported claims marked NOT TESTED.

**Non-goal:** autonomous stabilization and safe free flight are not implemented merely by adding an IMU or a "Hover" profile.
