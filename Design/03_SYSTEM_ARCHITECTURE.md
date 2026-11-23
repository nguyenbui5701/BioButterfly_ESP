# 03 — System Architecture: ESP-NOW Gamepad / XIAO Receiver (DRAFT)

**State:** DESIGN_DRAFT / owner-proposed control path recorded 2026-11-23.  
**Supersedes the original phone-first M0 proposal.** The owner has an ESP32 gamepad and explicitly plans **ESP-NOW** communication with XIAO ESP32-C3. Do not start implementation until design is approved and hardware/firmware Task is issued. Specific ESP-NOW, STL and PCB rules: [12](12_AS_BUILT_INPUTS_AND_BLOCKERS.md), [13](13_ESP_NOW_DUAL_FIRMWARE_RULES.md), [14](14_ARMV5_HORN_AND_CARBON_FIT_RULES.md), [15](15_PCB_V11_ASSEMBLY_AND_POWER_RULES.md).

## 1. System actors and proposed block diagram

Text architecture:

**ESP32 Gamepad (TX)** -> joystick/buttons -> input filtering and explicit STOP/ARM -> versioned ESP-NOW unicast packets -> **XIAO ESP32-C3 (RX)** -> strict radio protocol decoder -> bound peer/session/sequence -> Safety Supervisor -> motion planner + final calibrated clamp -> two SDS1601 servo drivers -> wing-root assemblies.

Battery pack (two actual 3.7V 220mAh cells, series connection still to be verified) -> protected wiring and regulator -> MCU/servo supply. Battery ADC and power faults feed **Safety Supervisor** and telemetry; a functional charging/balancing system is not demonstrated.

An optional later web interface may offer **disarmed configuration and diagnostics**, but is not the primary pilot in M0 and must not take over control while ARMED/ACTIVE. Its use with ESP-NOW requires channel/coexistence evidence and a new accepted design decision.

## 2. Two firmware images are mandatory

| Firmware | Responsibilities | Must NOT do |
| --- | --- | --- |
| Gamepad TX | Identify exact ESP32 board and joystick/button GPIO, debouncing/deadzone/calibration, explicit ARM/STOP button intent, peer binding, send cadence, optional receive ACK/telemetry, safe TX reboot | Assume unknown gamepad wiring/firmware, send auto-ARM on startup, assume packet sent means action taken |
| Butterfly RX | Strict ESP-NOW decode, bound-peer validation, sequence generation, source ownership, failsafe watchdog, battery monitor, state machine, wing motion planner, servo HAL, diagnostics | Accept arbitrary peers, treat web and RC as two concurrent pilots, write servo PWM from packet callbacks, replay stale throttle |

A **single versioned protocol contract** is maintained by AI-SW under SA approval. Both firmware images must have their own board-target build, version and test artifacts. Reuse of reference ESP-FLUTTER code subject to source/license review, not a blanket import.

## 3. Module ownership

- **Hardware abstraction:** stable, verified GPIO/PWM/ADC API, board identification, safe defaults, monotonic clock.
- **ESP-NOW RX adapter:** validate frame length, protocol revision, sender identity/cryptographic bind where supported, sequence, bounds and numeric finiteness.
- **Pilot session / arbitration:** one bound gamepad, explicit pairing and session epoch; no concurrent WebSocket pilot.
- **Safety supervisor / state machine:** central authority for ARM, STOP/DISARM, timeout, fault and recovery.
- **Wing motion planner:** bounded sine/triangle phase, left/right mixing from verified mechanical travel; deterministic timing.
- **Servo limiter / HAL:** last clamp after trim/mix; validated pulse range, polarity, neutral, travel and powered bench behavior.
- **Battery monitor:** real Rtop/Rbottom divider, calibration and validity; never infer pack cell balancing from total-pack voltage.
- **Settings storage:** versioned, integrity checked, safe migration; never restore ARM or nonzero throttle.
- **Diagnostics/telemetry:** device-authoritative state, packet age, accepted control source and faults; logs with firmware SHA.
- **TX input adapter:** local calibration of actual joystick/buttons; neutral on power-up, no sticky axis.
- **Optional UI:** only explicit, lower-priority configuration/telemetry in DISARMED after separate approval.

No radio ISR/callback directly changes GPIO, sets active mode or performs file/NVS operations.

## 4. Control state machine (proposed; under safety review)

- **BOOTING**: hardware outputs in safe state; no arm, no accepted axes.
- **DISARMED**: no active flap; require valid calibration and healthy power to move.
- **ARMED_IDLE**: intentionally armed, neutral throttle; wing flap stopped.
- **ACTIVE**: fresh, authorized nonzero throttle, within safe envelope.
- **FAILSAFE_STOP**: missing fresh valid RX packets, STOP, TX restart or critical fault; stop/neutral/torque-off behavior must be validated on real mechanism. Clear stale queued commands.
- **FAULT_LATCHED**: persistent unsafe condition; recovery only after cause cleared and owner-approved reset/explicit rearm.

Allowed nominal paths: BOOTING -> DISARMED -> ARMED_IDLE -> ACTIVE -> ARMED_IDLE. DISARM from any active state has priority. Timeout/critical fault -> FAILSAFE_STOP, never an automatic ACTIVE resume. Recovery -> DISARMED after explicit safe action and verified neutral. Boot/reconnect does not restore an ARMED state.

**Caveat:** abrupt stop during free-flight may cause descent. Failsafe behavior has to be physically risk-assessed before free flight; this state machine is not an airworthiness certificate.

## 5. Critical invariants and protocol ownership

1. **STOP wins:** highest-priority idempotent command, invalidates queued controls from previous generation; never overtaken by a later stale control packet.
2. **Freshness:** count only valid received commands with monotonic receiver timestamp, bound peer, known session and increasing sequence. Invalid packets do not refresh watchdog.
3. **Arm gate:** an explicit button action plus zero throttle, no known faults, verified battery state and calibration. Do not auto-arm when pairing or waking controller.
4. **Link watchdog:** proposed target <=500ms from last valid accepted command on RX. Final timing, STOP pose and re-arm details require bench evidence; no claims of test PASS.
5. **Single source:** ESP-NOW TX is sole M0 pilot. Any Wi-Fi AP UI, USB debug or other RC source cannot bypass safety or change active authority without separate ADR and DISARMED transition.
6. **Parameter validation:** frame size/type and numeric range before motion; final output clamp after waveform+steering+trim.
7. **Fault source:** battery/servo power errors cannot be fixed by web-interface state or joystick retry.
8. **Reboot:** RX and TX boot into neutral safe state; sequence reset handled through new session bind epoch or equivalent anti-replay handshake.
9. **No secrets in Git:** provisioning procedure for paired peer/key material; do not assume MAC address filtering alone proves authorization.
10. **Settings:** safe physical limits changed only DISARMED; physical source of truth is verified MECH-IF-002 and ELEC-IF-001.

## 6. Timing and throughput

- ESP-NOW TX control cadence **20–50Hz proposed**, to be measured based on real gamepad input and radio performance.
- RX motion planner tick **100Hz proposed**; servo PWM refresh is determined by actual SDS1601 unit electrical validation and is NOT flap frequency.
- Telemetry can run at lower cadence; logging and NVS writes must never starve servo updates or watchdog.
- Use a bounded queue/snapshot and monotonic clock with wrap-safe unsigned arithmetic. Avoid long blocking delays, allocations or burst logging in radio callbacks.
- Validate RF coexistence/channel planning if later AP diagnostics are added; official Espressif ESP-NOW documentation notes peer channel must match local Wi-Fi channel and encryption requires configured keys.
- Record loss, jitter, delay and reset evidence on real target with board/firmware version. No numerical performance assurance from architecture alone.

## 7. Cross-domain interface readiness

| Interface | Producer | Required before |
| --- | --- | --- |
| JOINT-IF-001, ROD-IF-001, MECH-IF-002 | AI-3D, owner physical measurements | Servo-arm coupling, physical flap limits and printed wing tests |
| PCB-IF-001, ELEC-IF-001 | AI-HW, PCB V1.1 verification | GPIO assignment, ADC safety and powered RX servo driver |
| PWR-IF-001 | AI-HW + owner | Battery power/charge/servo dynamic load |
| RF-IF-001 | AI-SW (TX/RX) + AI-HW controller inspection | ESP-NOW pairing, bind, packet version and watchdog integration |
| SYS-IF-001 | SA | Any Task creation/approval and cross-agent design freeze |

- Preserve supplied original STL and Gerber unchanged until a Task permits a derived revision and license/ownership allows its reuse.
- If an interface field is TBD or only owner reported, mark BLOCKED for any downstream physical step. Host-only safety unit tests may later be authorized with mocks, not actual servos.

## 8. Required test sequence

1. Host-only protocol and state-machine tests: wrong peer/version/size, nonfinite input, duplicate and stale seq, STOP/ARM race, TX reboot, watchdog, settings corruption and neutral/rearm.
2. Firmware compile for **both TX and RX** using verified target board configs, with exact SHA and dependency pins.
3. RX without servos: pins safe on reset/boot and regulator/ADC validated under AI-HW gate.
4. TX/RX table-top link test: binding, channel, neutral, ACK and link loss.
5. RX one servo with wings removed and current monitoring, after AI-HW and owner gate.
6. Static linkage/wing-fit and then dual-wing bench, after AI-3D measured horn/rod verification.
7. Tethered and then controlled flight trials only after all relevant independent gates and explicit owner GO.

## 9. Documents retained for future work

[02_UI_UX.md](02_UI_UX.md) contains the *old phone pilot cockpit and illustrative WebSocket packet examples*; they are **not an M0 protocol or authorization source**. Any optional configuration UI must be redesigned with a new disarmed-only interface after this architecture is approved.

**Current:** DESIGN_DRAFT / OWNER_SELECTED_ESPNOW_INTENT / TX_RX_IMPLEMENTATION_NOT_AUTHORIZED / HW_AND_MECH_BLOCKERS_OPEN.
