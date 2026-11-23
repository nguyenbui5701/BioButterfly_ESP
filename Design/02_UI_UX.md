# 02 — UI/UX Design: Mobile Web Cockpit

**State:** DRAFT; 2026-11-23. **Interface target:** mobile browser connected directly to the butterfly's local Wi-Fi AP; desktop responsive secondary. The UI is a **pilot instrument**, not decorative animation. Flight control must remain safe if the UI freezes.

## 1. Information hierarchy
1. **Always visible:** connectivity, safety state, active control source, battery reading + validity, STOP button.
2. **Primary actions:** ARM/DISARM, two-axis virtual joystick (X = steering, Y = throttle), reconnect.
3. **Secondary:** flight profile, tuning, calibration, diagnostics.
4. **Settings** are disabled while armed unless expressly whitelisted and safe.

## 2. Proposed screen structure

### Screen A — Pilot cockpit (portrait first)
~~~text
+------------------------------------+
| BIO BUTTERFLY         [LINK: LIVE] |
| DISARMED  [source: Phone]   7.9 V  |
| [FAULT / WARNING BANNER IF ANY]    |
+------------------------------------+
|        VIRTUAL JOYSTICK             |
|              ↑ Throttle            |
|              |                     |
|   left <----(●)----> right          |
|         release -> ZERO            |
+------------------------------------+
| [ ARM (hold) ] [ DISARM / STOP ]   |
| Flight: Cruise   Freq: 0.0 Hz      |
| Batt: 7.9V (measured)  Link: 32ms  |
+------------------------------------+
|  PILOT | TUNE | SETTINGS | DEBUG   |
+------------------------------------+
~~~

**Placement:** STOP is large, high contrast, touch accessible and never hidden behind scrolling, modal, keyboard or joystick. On mobile, reserve a fixed safe-area location. Avoid decorative color as the only signal: use text + icon + color.

### Screen B — Tuning (DISARMED by default)
- Select waveform (triangle/sine), preview predicted wing path without energizing actuators.
- Frequency, amplitude, left/right trim, neutral, steering gain, mechanical travel limits (only after calibrated).
- Range labels show units (Hz, degrees, microseconds, volts). Each change shows requested vs **device-confirmed** setting.
- **Save** only after device acknowledges validation; **Reset to safe defaults** requires confirmation.
- Experimental profiles clearly marked; no statement that Hover is stabilized.

### Screen C — Calibration / maintenance
- Wizard: inspect wings clear -> select L/R -> set mechanical midpoint -> verify polarity -> adjust min/max slowly -> save -> run low-rate test.
- Servo motion during calibration requires explicit, momentary hold-to-move with a hard STOP.
- Display board profile, pin labels from verified hardware manifest, calibration version, fault history.
- Do not expose hazardous arbitrary PWM writes or unknown GPIO switching in ordinary pilot UI.

### Screen D — Diagnostics
- Live: mode, source, control packet age, last command sequence, battery raw/filtered voltage, last fault, actual servo command angle, requested frequency.
- Charts/log export (optional); display **NOT MEASURED** when a sensor does not exist rather than inventing measured servo angles.
- Firmware/build identifier and test gate status.

## 3. UI states and behavior
| Device state | Pilot view | Permitted actions |
| --- | --- | --- |
| BOOTING | Connecting/safe | Observe only |
| DISARMED | Stationary; ARM available when preconditions pass | Calibrate, choose profile, arm |
| ARMED_IDLE | Armed, throttle=0 | Move joystick, STOP |
| ACTIVE | Wings moving, conspicuous active banner | Pilot control, STOP |
| FAILSAFE_STOP | Red fault/banner with cause and next action | STOP, observe; re-arm only through recovery flow |
| FAULT_LATCHED | Hardware/voltage issue; motion disabled | Diagnostics; recover after fault cleared |
| DISCONNECTED | Telemetry frozen **and labeled stale** | UI sends local zero on release best-effort; cannot claim device has stopped |

- ARM requires hold >= 1 s (UX proposal) plus device-side neutral-throttle gate. Holding is *not* a substitute for backend safety.
- A STOP is a separate explicit command; joystick pointerup/cancel, touchcancel, blur, visibilitychange and network disconnect trigger best-effort neutral/stop sends.
- Device-side timeout controls actual failsafe. UI cannot assume messages were received.
- On page reload/reconnect, show DISARMED/UNKNOWN until authoritative fresh device snapshot; **never replay previous joystick state**.
- For any control reject: show rejection reason without changing UI to a false accepted state.
- One active pilot session; secondary clients get observer-only access unless takeover is performed while DISARMED.

## 4. Interaction and accessibility
- One-handed portrait layout supported at 360px CSS viewport width; landscape offers larger joystick.
- Touch targets >=44x44 CSS px; high contrast, status text and icons; supports mouse/keyboard for bench tests.
- Pointer move can request 20–50 Hz commands (proposal; adapt to measured network performance). Axis values normalized: x [-1,+1], y [0,+1].
- Throttle springs to zero on pointer release; never sticky throttle in MVP.
- Heartbeat while actively controlling; rate capped/coalesced so telemetry cannot starve motion updates.
- Live status has timestamp/age. Stale after threshold chosen by device protocol; stale status must not look healthy.
- No phone accelerometer/tilt pilot mode in MVP; requires a separate gated user-study/testing path.

## 5. Proposed logical API contract (NOT yet implemented)
All v1 messages are validated on device. Client timestamps are informational only; freshness is based on server receipt and monotonic time.

**Control message:**
~~~json
{
  "v": 1, "type": "pilot.control", "session": "opaque-token",
  "seq": 1024, "x": -0.2, "y": 0.55
}
~~~

**Safety action (separate from control):**
~~~json
{ "v": 1, "type": "pilot.stop", "session": "opaque-token", "seq": 1025 }
~~~

**Arm request and acknowledgement (illustrative):**
~~~json
{ "v": 1, "type": "pilot.arm", "session": "opaque-token", "seq": 1026 }
{ "v": 1, "type": "command.ack", "seq": 1026, "accepted": false, "reason": "THROTTLE_NOT_ZERO" }
~~~

**Snapshot:**
~~~json
{
  "v": 1, "type": "telemetry", "state": "DISARMED",
  "source": "WIFI", "battery_v": 7.8, "battery_valid": true,
  "link_age_ms": 34, "fault": null, "flap_hz": 0.0,
  "left_cmd_deg": 90.0, "right_cmd_deg": 90.0,
  "fw": "0.1.0", "settings_rev": 2
}
~~~

Protocol design gates:
- Exact transport (WebSocket vs HTTP/UDP), heartbeat, anti-replay reset/rollover, takeover and session handshake are formalized in 03_SYSTEM_ARCHITECTURE.md before coding.
- One command source at a time and explicit per-message response codes.
- Reject extraneous critical fields, version mismatch, malformed numeric values and oversized payloads; never parse arbitrary strings into 0 silently.
- Stop must work independently of UI display state, and a disconnected client cannot directly assert device outcome.
- Credentials must not be the known public demo password. Device provisioning and local session protection are part of implementation design review.

## 6. UI acceptance (demonstration)
- 360px viewport, portrait/landscape: STOP visible without scrolling.
- Joystick drag/release/cancel/phone lock, page reload: no ghost throttle. Device-side failsafe evidence required.
- Simulated disconnect and stale telemetry: show disconnected/unknown, not ARMED_OK.
- Battery ADC invalid -> "Battery unavailable / cannot arm" instead of a percentage.
- Tuning values are round-tripped and not optimistically declared saved.
- Accessibility: touch targets, contrast and labels verified manually.

## 7. Delivery boundary
This file defines a target UX only; **no code, binary UI bundle or API implementation is authorized** until owner design approval and a Task is issued.
