# 07 — Reference Project Audit and Reuse Boundary

**Audit date:** 2026-11-23.  
**Target repository:** https://github.com/nguyenbui5701/BioButterfly_ESP  
**Reference code:** https://github.com/maximagination1/ESP-FLUTTER  
**Reference physical assets:** https://drive.google.com/drive/folders/1XTTW8Ez_hmZl5E6mPUDXgX2qnnI3Cp2e

## 1. Reference investigation extent

Observed GitHub reference repository main HEAD tree SHA: 7ea77d8eaf7b1db67f2c018e3623eaa8998d3809. Read README, project context, PlatformIO config, multiple source/hardware modules and protocol headers. Examined relevant sections of the wing motion and Wi-Fi control code. This is a focused architecture-level review, **not** an exhaustive security audit of every source line.

The source project is ESP32 Arduino + PlatformIO, default XIAO ESP32-C3, with XIAO ESP32-S3 and esp32dev profiles marked experimental. Its user interface runs on a local AP with WebServer, WebSocketsServer and optional UDP control. It also handles ESP-NOW and optional ELRS/CRSF.

Reference README explicitly identifies its firmware license as **PolyForm Noncommercial 1.0.0**. Noncommercial hobby/education conditions apply; **commercial use requires prior written permission** according to the author's README. The repo has no separate LICENSE file apparent in the inspected tree. Before reproducing actual source, embedded UI, schematics, images or wing assets in this project, review the applicable license and asset permissions with owner. Do not treat a public GitHub repo or shared Drive folder as public-domain rights.

## 2. Code inventory and function relationships

| File | Role | SA recommendation |
| --- | --- | --- |
| ESP-FLUTTER/platformio.ini | Defines board profiles and ESP32Servo/WebSockets dependencies | Use as build-system research only; pin dependencies independently |
| ESP-FLUTTER/src/main.cpp | Calls initHardwareConfig/initWingMotion/initLedControl/initWiFiControl; loop updates motion, LEDs, radio | Our bootstrap must prioritize safe state, bounded work and fault reporting |
| ESP-FLUTTER/src/config.h | Servo pins, limits, gains, timing, battery parameters | Reference only; do not copy unsafe constants without measurements |
| ESP-FLUTTER/src/wing_motion.cpp | Phase/time flapping, mixing, trims, servo PWM and stop/center | Separate motion planner, final limiter and HAL in target |
| ESP-FLUTTER/src/wifi_control.cpp | About 4,786 lines: local HTML, control processing, telemetry, RC, preferences and battery logic | **Do not replicate large coupled module**; split interface, protocol/session, battery and telemetry |
| ESP-FLUTTER/src/flight_mode.cpp | DISARMED / ARMED / PERCH modes | Our state machine adds explicit FAILSAFE_STOP / FAULT_LATCHED semantics |
| ESP-FLUTTER/src/flight_profile.cpp | Cruise/Sport/Hover profile settings persisted in Preferences | Defer presets until stable hardware control and safety |
| ESP-FLUTTER/src/hardware_config.cpp | Runtime pin storage via Preferences | Target pin profile initially fixed by validated hardware revision |
| ESP-FLUTTER/src/hover_bias.cpp | Experimental phone pitch bias smoothing | Not MVP and not actual stabilization |
| ESP-FLUTTER/include/esp_flutter_espnow_packet.h | Versioned packed flags and mixed control/tuning packet | RC future; new protocol version/security acceptance required |
| ESP-FLUTTER/include/esp_flutter_udp_control_packet.h | Packed v1 12-byte UDP steering/throttle protocol | Do not adopt unreviewed remote command endpoint in MVP |

Read sections show:
- initWingMotion loads trim/refresh/pulse preferences, initializes safe-looking servo neutral trajectory; our behavior must be independently verified on real hardware.
- updateWingMotion uses microsecond delta time, phase and sine/triangle waveform and final servo angle constraints.
- applyPilotControlValues implements armed gate, throttle/frequency conversion and optional fixed-amplitude test mode.
- handleControl has a separate HTTP path for pilot control; logic overlaps applyPilotControlValues, which is an architectural duplication risk.
- updateWiFiControl checks a **500ms** control timeout and calls immediateStopAndCenter when armed; target should independently test all branches including source changes and session replay.
- The reference README states its Hover profile is *not* automatic stabilization or position hold. Do not label the target MVP as autonomous hover.

**Important mismatch to track:** reference config defines servo pulse defaults 500–2500us, while the SDS1601 datasheet image advertises 800–2200us. The reference firmware may support configurable ranges but our default must be bounded by the selected servo's verified limits and mechanical assembly.

## 3. Drive folder audited after connection

The shared Drive folder was accessible. Found and inventoried:
- App Screenshots: Controller Panel - Web App.JPG, Preferences Tab - Web App.JPG.
- Datasheets: SDS1601 Servo Specs.jpg, MP2307.PDF.
- PCB Files: Gerbers, PCB Assembly FIles folders, PCB imagery.
- Troubleshooting Guide: numbered symptom images 1–8 plus cheat sheet.
- Wing Files: Left Wing.pdf, Right Wing.pdf, rod length chart and carbon rod thickness schematic.
- Wiring Diagrams: modular wiring graphic and FC PCB schematic.
- Top-level: reference flight-time test video (title says 1m 9s), print settings screenshot.

**Do not claim actual flight endurance of our butterfly** from the video filename. All-up mass, current draw, wing load, cell condition and environmental conditions differ. The MP2307 PDF was identified but not fully qualified for our PCB, and the source Gerber assembly was not electrically reviewed. Neither is an approved BOM.

### Specific visible findings
1. **SDS1601** advertisement: 5.0–8.4V, 16g, neutral 1520us, 333Hz, pulses 800–2200us. Stall current absent; cannot size 2S battery or conductors from torque alone.
2. **Modular reference**: 2x 1S 220mAh drawn in series, 2S battery feed to servos, buck to XIAO input, optional receiver and LEDs.
3. **Wiring inconsistency**: the battery voltage divider labels in modular graphic, PCB schematic and source config do not match exactly. Confirm actual Rtop/Rbottom, ADC input voltage and calibration with a DMM on the assembly.
4. **Wing PDF outlines**: mirrored left/right wing shape. A separate per-wing carbon rod list exists; requires scale and mechanical fit check.
5. **Web screenshots**: reference pilot screen shows battery status, joystick, modes and arm/disarm; Preferences screenshot shows servo trim, steering gain, upper/lower amplitude, servo refresh and pulse limits. Our UI deliberately moves STOP into a persistent higher-priority area, makes stale telemetry explicit and restricts unsafe live calibration.
6. **License**: original firmware explicitly noncommercial; images/wing assets require their own permission assessment before redistribution.

## 4. Target build-vs-buy and reuse plan

| Area | Plan | Approval condition |
| --- | --- | --- |
| Control architecture | Original modular implementation inspired by algorithms but no copying without license | SA architecture + owner approval |
| UI | Original responsive cockpit with separate safe controls and telemetry | UI review and device failsafe proof |
| Servo motion | Independently implemented with synthetic host tests | Measured pulse range, endpoint and timing |
| Battery circuit | Design based on selected actual pack/regulator/ADC | Signed HW-G1/G2, not schematic screenshots alone |
| Wing geometry | Original CAD adapted after measured parts/mass | Owner license choice; CAD fit/weight proof |
| Original PCB/Gerbers | Do not import into code repo by default | Hardware/schematic/license review |
| RC extension | Separate post-MVP development | New ADR and control arbitration test |
| Pilot scripts/firmware copy | Prohibited unless explicitly licensed for intended use | Owner signs off rights and attribution plan |

## 5. Open verification gaps

- Actual component inventory and PCB revision for this build.
- Exact selected 2S cells, charger/balancer, connector, continuous and peak current.
- Electrical verification of reference schematic, especially divider resistor values and buck implementation.
- Servo current draw, PWM compatibility and physically measured endpoints.
- Complete safety review of reference firmware and third-party dependencies (not performed).
- Rights to republish reference CAD/wing PDFs and schematic images.
- Flight-ready mass, thrust/lift estimates and center of gravity.

## 6. As-built owner evidence vs original reference

On 2026-11-23 the owner supplied a **Gerber V1.1 RAR and two STL files**, not the same thing as an approved editable source project. Their content and SHA-256 are recorded in [12_AS_BUILT_INPUTS_AND_BLOCKERS.md](12_AS_BUILT_INPUTS_AND_BLOCKERS.md).

- Reference servo sheet also lists **spline 25T-Ø4.90mm**, potentially incompatible with the proposed **metal 21T/~4.5mm** horn. Actual purchased servo markings and mating test are required, not just the datasheet.
- Uploaded ARMV5 STL is a print-layout mesh with **two disconnected closed components**; the owner's printed arm has horn and carbon-rod fit problems.
- FC Mount V5 STL remains **unprinted**; no PCB-to-mount fit conclusion exists.
- The Gerber bundle includes four copper layers; the PCB outline bounding rectangle is **~18.5 × 45.0mm**; the separate drill file includes 0.30/0.60mm tools. Layer and drill-file generator metadata differ, so perform real CAM alignment verification before assembly.
- Supplied transmitter is an **ESP32 gamepad** but exact board/pins/firmware are unknown. Owner plans ESP-NOW primary pilot; former WebSocket phone pilot code is a reference pattern only.
- Third-party file rights apply independently to Gerber/wing STL/CAD; DO NOT upload reference assets into public GitHub merely because the user has physical copies.

See 13 for TX/RX protocol, 14 for the arm fit checks, 15 for the PCB assembly/power audit.

## 7. Research-to-task boundary

These references inform DRAFT architecture only. Any subsequent Task must name exact reference files and **state which content may be copied, adapted or merely studied**. AI Code must not import a complete third-party firmware or UI bundle merely to get the project running quickly.

**Final audit disposition:** REFERENCE_RESEARCH_COMPLETE for initial architecture; HARDWARE_VERIFICATION_PENDING and LICENSE_REUSE_DECISION_PENDING.
