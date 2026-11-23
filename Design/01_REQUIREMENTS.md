# 01 — Product Requirements (PRD / SRS)

**Document state:** DRAFT for owner/SA sign-off; 2026-11-23.  
**Product:** BioButterfly_ESP — experimental, **ESP-NOW gamepad-controlled**, two-servo bionic butterfly. The earlier phone-first M0 proposal has been superseded by the owner's 2026-11-23 direction.  
**Scope convention:** MUST = launch gate; SHOULD = after gate; MAY = future. Numerical targets below are *proposed acceptance thresholds*, not measured performance.

## 1. Vision and problem statement
Build an understandable, maintainable, testable **ESP-NOW gamepad + XIAO ESP32-C3** bionic butterfly platform that starts with a **safe bench prototype** and graduates to tethered/free-flight experiments only after evidence-based verification. Prioritize reproducible control and safety over copying a large existing firmware. A beautiful body, lightweight wing structure and comfortable control UI are product goals, but flight and hover stability cannot be promised by firmware alone.

## 2. Users and operating context
- **Builder / maintainer:** assembles the body and power system, configures board/servo calibration, inspects logs and runs tests.
- **Pilot:** operates the existing ESP32 gamepad linked by ESP-NOW to the XIAO ESP32-C3 receiver; needs explicit ARM/DISARM/STOP and device-authoritative connection/battery indicators. Optional phone UI is not the M0 pilot.
- **SA / AI Code / reviewer:** works from versioned contracts and safety test evidence.
- **Environment:** indoor test rig first; only afterward a clear controlled test area with bystanders excluded. This is an experimental RC device, not an autonomous vehicle.

## 3. Proposed release scope

### M0 — Bench-capable MVP (MUST)
| ID | Requirement | Acceptance / proof |
| --- | --- | --- |
| FR-001 | Boot into DISARMED, wings stationary/neutral | Power-cycle 10 times with no unsolicited sustained flap; record logs/video |
| FR-002 | Two independent servo channels with explicit polarity/trim | Each channel tested separately and then together; travel stays within measured mechanical limits |
| FR-003 | Parameterized periodic wing motion | Sine or triangle command, configurable frequency/amplitude/neutral, bounded and smooth under bench load |
| FR-004 | ESP-NOW TX gamepad / RX butterfly link | Two versioned builds, paired authorized peer, neutral/ARM/STOP inputs, channel match, rejection of invalid/stale frames |
| FR-005 | Arm only after explicit pilot action and healthy preconditions | Invalid or stale commands cannot arm; throttle neutral must precede ARM |
| FR-006 | Stop/disarm on lost control | Missing fresh valid pilot commands causes safe neutral/stop within **500 ms target**, verified by packet-loss test |
| FR-007 | Manual stop control independent of joystick position | Physical pilot action triggers software hard-stop immediately; no auto-resume from delayed packets |
| FR-008 | Battery measurement and warnings | Voltage calibrated against DMM; invalid ADC reading is a fault, not a fake percentage |
| FR-009 | Central bounds validation | Out-of-range/NaN/infinite settings rejected with explicit error; safe servo clamp always last |
| FR-010 | RX-authoritative diagnostic telemetry | Device mode, peer/link age, commanded flap, servo output, valid measured Vbat, faults and version exposed to TX/serial; web UI optional |
| FR-011 | Persistent calibration with versioned defaults | Safe reboot and invalid/corrupt settings fallback; no unsafe auto-arm |
| FR-012 | Reproducible **TX and RX** builds and tests | Pinned dependencies, versioned shared packet schema, test log artifacts, both exact board profiles selected intentionally |
| FR-013 | Verifiable servo-arm/rod mechanical interfaces | No fixed 21T horn assumption before SDS1601 spline check; four rod sockets and two-fastener joint measured/tested |
| FR-014 | PCB V1.1 as-built verification | Gerber/drill alignment, revision-matched component/BOM/net validation and safety checks before battery hookup |

### M1 — Controlled tuning and tethered/flight experiments (SHOULD)
- Optional disarmed-only web configuration/telemetry UI and profile presets (Cruise / Sport; Hover explicitly **experimental**, not closed-loop position holding).
- Differential left/right wing steering with configurable gain and bounded compensation.
- Configurable in-app calibration wizard in DISARMED state.
- Battery sag characterization and current/temperature checks under peak servo load.
- Tethered trials followed by controlled free-flight trials with logs; flight performance declared only with measured data.

### M2 — Extensions (MAY; separate design/ADR)
- Alternative RC sources such as ELRS/CRSF, LED wing art, IMU-based stabilization, autonomous behavior, OTA updates.
- Cloud access, user accounts, video streaming and remote Internet flight are **out of MVP scope**.
- ESP-NOW remains the M0 primary source; no concurrent Web UI/other RC pilot without a separate one-owner arbitration ADR.

## 4. Non-functional requirements
| ID | Requirement | Definition of done |
| --- | --- | --- |
| NFR-01 | Safety defaults | No flap on cold boot, restart, reset, reconnect, brownout recovery, malformed commands |
| NFR-02 | Timing | Time-based wave synthesis (monotonic clock) independent of network request cadence; jitter logged and bounded by bench data |
| NFR-03 | Robustness | Nonblocking main control loop; reconnect cannot resurrect stale throttle |
| NFR-04 | Security | Bound ESP-NOW peer/session, protocol version and anti-replay; no committed keys; if future AP exists, use unique secret + disarmed-only permissions |
| NFR-05 | Maintainability | Separate **gamepad TX / XIAO RX builds**, shared wire schema, control decoding, safety state machine, motion, servo HAL, telemetry and storage |
| NFR-06 | Observability | Serial log levels, standardized fault codes and minimal structured telemetry without noisy logs in hot path |
| NFR-07 | Portability | One verified board profile (candidate XIAO ESP32-C3); other boards are unverified until a separate gate |
| NFR-08 | Size/power | Flight mass and current budget to be measured, not guessed; no release claim before verified BOM + balance |

## 5. Critical business and safety rules
1. **No arm if:** throttle nonzero, battery invalid/critical, calibration unavailable, servo fault known, emergency latch set, or controller untrusted.
2. **Disarm wins:** DISARM/STOP takes priority over every other command and cannot be superseded by packets already in flight.
3. **Loss of control:** transition to FAILSAFE_STOP and cancel motion; require explicit release + re-arm. A disconnect must never keep flapping indefinitely.
4. **Input ownership:** one active pilot source; source switching requires DISARMED. Old sequence numbers, expired timestamps, wrong schema and nonfinite inputs are rejected.
5. **Configuration:** settings affecting mechanical range and pulse timing may be edited only while DISARMED and with physical clearance.
6. **Battery:** an ADC divider is telemetry only, **not** a substitute for cell protection, balancing, charging safety or regulator current capacity.
7. **Flight readiness:** a passing firmware compile does not mean bench-ready; a bench pass does not mean airworthy.

## 6. Preliminary acceptance suite
- **AC-01:** first power-up without any pilot input — remain DISARMED and safe.
- **AC-02:** valid arm request with neutral throttle succeeds; with 5%+ throttle is refused.
- **AC-03:** joystick release to zero stops motion; delayed control packets never restart without new explicit command.
- **AC-04:** simulate ESP-NOW TX power loss/reboot, channel mismatch, wrong peer, packet loss, invalid frame and stale/replayed sequence; each triggers documented safe RX behavior.
- **AC-05:** battery ADC disconnected/saturated/out-of-range — fail closed, flag error.
- **AC-06:** stop/arm command races — STOP has priority.
- **AC-07:** 5-minute no-flight bench exercising does not hang or reset, **subject to hardware thermal/current limits**.
- **AC-08:** verify mechanical endpoints manually before increasing frequency/amplitude; save measured range.
- **AC-09:** test firmware artifacts include commit SHA, board, BOM, tests, logs, approval and known limitations.
- **AC-10:** UI does not show armed/healthy from stale telemetry; disconnected state is obvious.

## 7. Not yet confirmed by owner
- Owner reports owning XIAO ESP32-C3 and SDS1601 servo(s); actual board revision, servo batch spline and safe mechanical/pulse range remain unmeasured.
- Owner reports two 3.7V 220mAh cells; actual series harness, regulator, charger/2S balance/protection and maximum measured load remain unverified.
- Owner supplied ARMV5 and FC Mount V5 STL plus Gerber V1.1. ARMV5 has known owner-reported horn/rod fit failures and FC Mount V5 is unprinted; total mass, linkage and flight vs display goal remain open.
- The linked Google Drive reference folder was subsequently browsed: wing-outline PDFs, wiring images, SDS1601 datasheet image, PCB folders and app screenshots are identified in 07_REFERENCE_AUDIT.md. However, not all source CAD/PCB contents were validated, and **none** of the actual intended hardware assembly has yet been verified.
- Exact gamepad ESP32 variant, joystick/buttons and preloaded firmware are not known; reference code/CAD license and intended distribution require owner decision.
- Proposed metal horn is 21T/~4.5mm while reference SDS1601 advertises 25T/Ø4.90mm; this is a hard fit blocker.

These are **blocking for hardware pinout and final flight release**, but not for a preliminary software architecture.

## 8. Traceability
FR-001,005,006,007,009 -> 03_SYSTEM_ARCHITECTURE.md state machine + 05_AI_WORK_RULES.md gates.  
FR-002,003,008 -> 04_HARDWARE_AND_SAFETY.md.  
FR-004,010,011,012 -> 03_SYSTEM_ARCHITECTURE.md + 13_ESP_NOW_DUAL_FIRMWARE_RULES.md; 02_UI_UX.md is a future optional UI draft.
FR-013 -> 14_ARMV5_HORN_AND_CARBON_FIT_RULES.md; FR-014 -> 15_PCB_V11_ASSEMBLY_AND_POWER_RULES.md.  
All FR/NFR -> explicit implementation tests after design approval; no Task file yet.

**Current owner inputs and source SHA:** see [12_AS_BUILT_INPUTS_AND_BLOCKERS.md](12_AS_BUILT_INPUTS_AND_BLOCKERS.md). All old phone-pilot requirements are superseded where inconsistent with ESP-NOW M0. No Task/ issued by this amendment.

**Approval gate:** owner reviews scope, hardware assumptions and safety requirements; SA records approval in 06_DECISIONS_AND_ROADMAP.md before issuing any coding task.
