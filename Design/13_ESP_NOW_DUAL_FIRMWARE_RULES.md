# 13 — ESP-NOW Gamepad to Butterfly: Control and Dual-Firmware Rules

**Status:** DESIGN_PROPOSED / 2026-11-23. **Owner intent confirmed:** an available ESP32 gamepad will transmit control to a XIAO ESP32-C3 butterfly over **ESP-NOW**. This supersedes **phone WebSocket as primary pilot source** for the next design iteration; it does NOT imply protocol approval or existing working software.

## 1. System boundary and roles

- **TX (controller):** exact ESP32 variant, board pins, joystick, buttons, battery supply and existing firmware version are **UNKNOWN** until examined. TX firmware is a **separate deliverable** from RX.
- **RX (butterfly):** XIAO ESP32-C3, two SDS1601 servos through verified PCB, battery sensing, safety supervisor and motion control.
- **ESP-NOW:** direct radio packets, no access point/internet requirement for primary pilot operation. Do not assume product-page title proves installed ESP-NOW firmware, availability of source code, antenna quality or joystick mapping.
- **Optional Web UI:** may be approved later for configuration, bench diagnostics or read-only telemetry. Its commands must not contend with active TX pilot, and coexistence must be tested if Wi-Fi AP and ESP-NOW share the radio. No web pilot fallback while ACTIVE.

## 2. Mandatory ownership separation

**TX agent (AI-SW sub-scope):** joystick read/calibration, button debouncing, ARM/STOP button mapping, packet encoding, sending cadence/tx-status diagnostics, radio peer setup, optional controller battery telemetry.

**RX agent (AI-SW sub-scope):** radio init/peer binding, strict decoder, receive-time freshness, duplicate/replay protection, pilot authority, independent failsafe timer, state machine, actuator limiter, explicit fault state.

**Shared protocol:** an SA-approved versioned wire schema produced from a single source/definition; verify both builds use same protocol revision and test vectors. Keep transmitter and receiver code paths separate in Task allowlists. Do not copy the reference's ESP-NOW structs solely because they compile.

## 3. Proposed command semantics — not yet fixed wire bytes

| Signal | Direction | Mandatory behavior |
| --- | --- | --- |
| PROTOCOL_HELLO / DEVICE_ID / REV | TX <-> RX | Version/peer identification; reject incompatible implementations |
| HEARTBEAT / CONTROL | TX -> RX | Monotonic sequence, authenticated known peer, bounded throttle/steering; updates receiver age only after validation |
| ARM_REQUEST | TX -> RX | Explicit intentional action; only at neutral throttle, healthy battery, known calibration, no active fault |
| DISARM / STOP | TX -> RX | Highest priority, idempotent; control generation invalidated; receiver must acknowledge if two-way telemetry exists |
| TELEMETRY / ACK | RX -> TX | Device-authoritative armed state, fault, battery valid/volts, radio/control age, protocol/firmware revision |
| RELEASE / THROTTLE_ZERO | TX -> RX | No sticky throttle; receiver enters idle or stop as defined by accepted state machine |
| PAIR / BIND | TX <-> RX | Deliberate binding process in DISARMED only; no silent takeover |

Before coding, define **exact** endian, alignment, packed size, max frame length, field order/type, float format or integer scaling, flags and test vectors; do not presume raw C++ struct ABI is portable. Document versioning and reject nonfinite/out-of-range numeric values.

**Suggested normalized axes:** steering -1..+1, throttle 0..1. Actual joystick axes, sign, deadzone and trims require TX board tests. No mapping until buttons/analog calibration is observed.

## 4. Core safety invariants

1. Boot/reset/reconnect on either device must **not** arm wings or apply old throttle.
2. RX is the sole authority on safety and servo outputs. TX receipt/success status does not prove servo action.
3. RX controls remain fresh only when a **valid packet from the bound peer** arrives, with correct protocol and increasing sequence. Do not reset timeout for malformed, unexpected-source or replayed packets.
4. STOP/DISARM is processed before queued axis updates; it **latches a safe state and invalidates prior generation**. Repeated STOP is safe.
5. Following link timeout, watchdog expiration, battery fault, brownout or controller restart, safe action is taken **by RX independently**, requiring a new explicit arm with verified neutral throttle to resume.
6. A brief neutral joystick position should never be interpreted as an ARM request. Button bounce/accidental simultaneous buttons must be tested.
7. Permit a single bound primary controller; reject unbound MAC, alternate session or a channel-hopping competitor. ESP-NOW encryption/peer key management should be selected based on actual module and SDK capabilities; MAC filtering alone is not a strong authentication boundary.
8. When test mode/diagnostics is enabled, servo output still obeys all physical limits and STOP priority. No 'perch exception' that ignores link-loss safety without a separately approved ADR.
9. A loss-of-link stop is not an airworthiness guarantee: stop-flapping may cause descent. **Only bench and tethered trials** until loss-of-control flight risk is explicitly assessed.

## 5. Timing and RF quality: proposed targets, not measured

- Prototype TX control cadence **20–50 Hz**; select real value based on measured TX/RX latency, task load, RF interference, power and servo response. Do not build around hard-coded phone polling.
- Initial RX command timeout **<= 500ms** proposal inherited from earlier safety requirements, to be risk reviewed and measured on target. Validate more conservative timeout with lab data; don't reduce or increase casually.
- Track most recent **valid** RX packet time on RX monotonic clock; record min/mean/max/p99 delivery age where measurable, dropped packets, sequence discontinuities, active link faults and re-bind count.
- TX on button RELEASE should send neutral promptly, but reliable stop must **not** depend on a final packet arriving.
- Bound callback work and queues; radio callback should validate/copy a small packet and return, not perform servo control, flash writes or lengthy JSON/network operations.
- Avoid heavy Serial prints or web UI transmissions inside critical motion/ESP-NOW callbacks; measure jitter.

## 6. ESP-NOW + Wi-Fi AP coexistence policy

- ESP-NOW and Wi-Fi AP operation may compete for channel/radio resources; require explicit channel plan when web configuration is included.
- No automatic AP channel switch or STA reconnection while armed. Bench-test ESP-NOW packet loss while AP/web diagnostics are active; UI should be disabled/restricted if it degrades control.
- If chosen ESP32 gamepad firmware uses Wi-Fi/BT differently, do not assume its presence implies ESP-NOW firmware support. Confirm board, library and firmware source first.
- Require explicit ADR before supporting multiple pilot input sources. The previous WebSocket primary-pilot control proposal is **SUPERSEDED FOR PILOT INPUT ONLY**, not deleted from history.

## 7. Pairing and replay gates

- Document factory bind, recovery bind, controller replacement, firmware mismatch and lost-key behavior.
- Pairing only in DISARMED with explicit owner action; new device cannot send ARM without fresh bind session.
- Use sender boot nonce/session epoch or an equivalent robust sequence reset protocol to handle controller reboot/32-bit wrap; stale packets from previous sessions must not be accepted.
- Neither RSSI nor tx send result is a substitute for fresh valid receiver control.
- Keep secrets/keys and permanent peer identities out of committed examples, test logs and public screenshots; provide provisioning workflow.

## 8. Verification matrix (software and hardware separated)

| Test | Expected result | Evidence |
| --- | --- | --- |
| Wrong peer/MAC, wrong key, corrupted packet | Rejected; no motion or timeout reset | RX logs + test packet |
| Duplicate, reverse sequence, old generation | Rejected; cannot restart after STOP | Host test vectors + RX logs |
| TX power off / battery removed while active | RX stops within approved deadline; latched | Real RX bench test (wings removed first) |
| TX reboot with nonzero joystick | No auto-arm on reconnect | TX/RX bench logs |
| RX reboot/brownout | DISARMED; outputs initialized to approved safe state | RX physical measurement |
| STOP during high-rate traffic | STOP wins; no later queued axes resume | Race tests + oscilloscope/serial |
| Joystick center/release, analog drift | Neutral or debounced zero; not accidental ARM | TX calibration logs |
| RF congestion / AP telemetry traffic | Link quality bounded, timeout remains effective | Measured p99 gaps + loss |
| Both firmware versions disagree | Reject control safely, indicate mismatch | Automated handshake test |
| Valid unpaired gamepad tries control | Rejected until explicit pairing | Bench test |

## 9. Required interfaces for AI-SW and AI-HW

- From **AI-HW**: exact TX controller MCU/pins/supply/joystick lines, RX board/PCB rev, antenna/ground keepouts, power/ADC calibration and servo signal/power.
- From **AI-3D**: servo direction, measured neutral/travel, mechanical envelope and stop constraints.
- From **AI-SW**: separate TX/RX firmware artifact versions + shared packet schema and state-transition tests.
- **SA**: agree protocol/timeout/binding semantics and pin each signed contract revision before implementation Tasks.

## 10. Explicit STOP conditions

Unknown controller pinout/firmware; contradictory wiring vs PCB; non-working STOP; rising stale/control losses; unstable AP/ESP-NOW coexistence; open/provable replay hole; servo spline/limit unresolved; unsafe 2S power. Do **NOT** bypass failsafe for successful radio-demo video.

**Current:** DESIGN_PROPOSED / CONTROLLER_FIRMWARE_UNKNOWN / TX_RX_PROTOCOL_UNAPPROVED / BENCH_NOT_RUN.
