# 06 — Architecture Decisions, Design Gates, Roadmap and Review Ledger

**State:** DESIGN_DRAFT. **Last reviewed:** 2026-11-23.  
**No approval has yet been given for the BioButterfly_ESP design.** Entries marked PROPOSED are not finalized design decisions. Do not issue Task files or implement firmware against this draft without owner sign-off.

## 1. Design decision records

| ID | Proposed decision | State | Reason / conditions |
| --- | --- | --- | --- |
| ADR-001 | Start with XIAO ESP32-C3 | PROPOSED | Default target in reference; physical board/pins not yet verified |
| ADR-002 | Arduino framework + PlatformIO, pinned build | PROPOSED | Repeatable ESP32 toolchain, host tests separate from hardware |
| ADR-003 | Mobile-first local AP/web pilot concept | SUPERSEDED_DRAFT | Owner now intends ESP-NOW gamepad as M0 pilot; web may later support disarmed-only config/telemetry |
| ADR-004 | WebSocket v1 as pilot source | SUPERSEDED_DRAFT | ESP-NOW TX/RX bind/session/version proposal now takes precedence; no second active pilot |
| ADR-005 | Two SDS1601 servo wing actuation | OWNER_HARDWARE_REPORTED / VALIDATION_PENDING | SDS1601 present; 25T reference spline vs proposed metal horn 21T unresolved |
| ADR-006 | Device-side fail-closed safety state machine, boot DISARMED | PROPOSED | Safety must not depend on phone, Wi-Fi or UI refresh |
| ADR-007 | Separate flight-weight frame and optional removable aesthetic shell | PROPOSED | Reduce first prototype mass and simplify access |
| ADR-008 | Phase 0 bench validation before tethered/free flight | PROPOSED | No assumption that working firmware produces flyable vehicle |
| ADR-009 | Reimplement modular firmware contracts rather than wholesale copying ESP-FLUTTER | PROPOSED | Reference license is noncommercial and original code has broad coupling |
| ADR-010 | Use original wing reference as study material, create original approved CAD for our geometry | PROPOSED | Mass/scale and asset licensing decisions remain open |
| ADR-011 | Phone-only M0, defer ESP-NOW/ELRS | SUPERSEDED_DRAFT | ESP-NOW selected by owner; ELRS and other controllers remain out of M0 |
| ADR-012 | SA + 3 senior specialist AI roles (3D/HW/SW), shared interface/handoff contracts and isolated PR ownership | PROPOSED | Design/AGENTS.md and 08–11 define working protocols; no specialists automatically instantiated |
| ADR-013 | Primary M0 radio control = ESP-NOW gamepad TX -> XIAO ESP32-C3 RX | OWNER_DIRECTION / TECH_DESIGN_PENDING | Actual TX board/firmware unknown; design/approval gates per 13; optional web is not active pilot |
| ADR-014 | Preserve existing ARMV5, FC Mount V5 and PCB V1.1 as immutable source baselines | OWNER_INPUTS_RECORDED / DESIGN_PENDING | SHA and failures in 12; horn compatibility, rod-fit and PCB electrical audit blocked |
| ADR-015 | No PCB power/print/coupling work without domain gates and physical evidence | PROPOSED | 14 and 15 gate physical assembly; owner authorization is separate from doc commits |
| ADR-016 | **Fully 3D-printed** SG90-style derived horn directly mating SDS1601 spline; **3 collinear lever holes**, 2 aligned wing-arm holes, 2 installed bolts per wing | **OWNER_CONCEPT_CONFIRMED / DIMENSIONS_PENDING** | Owner explicitly selected all-plastic horn (no metal insert) and straight-line hole geometry. Tentative H1/H3 7mm apart, M1.6/Ø1.8mm. Actual spline-tooth fit, print process, fatigue, retention screw and bolt length NOT approved; Design/16 |

**Approval mechanics:** Owner explicitly approves/rejects each major decision, SA updates state to ACCEPTED, DEFERRED or REJECTED, includes date and approval note, then commits Design baseline SHA. Approval of a draft is NOT inferred from silence or Git commits.

## Agent team and design handoff gate

- Read [AGENTS.md](AGENTS.md) before assigning an agent.
- **AI-3D** mechanical/print; **AI-HW** electrical/power; **AI-SW** firmware/UI; **SA** coordinates and reviews all.
- All three agents follow [08_MULTI_AGENT_RULES.md](08_MULTI_AGENT_RULES.md) with MECH-IF-001/002, ELEC-IF-001, SW-IF-001 versioned handoffs.
- Agent rules creation **does not approve MVP, hardware, interface revisions, tests, PRs or new Task**.
- Cross-domain changes must go to SA for impact analysis before code/CAD/schematic change.

## 2. Proposed project phases and implementation order

| Phase | Outcome | Hard gate before advancing |
| --- | --- | --- |
| D0: Discovery / design | Requirements, UI, architecture, power and safety design reviewed | Owner design approval, reference/license decision |
| D1: Host-only logic foundation | **TX/RX ESP-NOW wire contract** + fake-clock state machine + invalid/replay test vectors | Unit-test proof STOP, timeout, pairing/replay, version and numeric validation |
| D2: Dual ESP32 bootstrap | **Two separate builds**: real gamepad TX and XIAO ESP32-C3 RX; safe receiver boot | Both actual board pin maps/versions and no-motion reset evidence |
| D3: PCB/arm input verification then power/servo | Inspect four-layer Gerber V1.1 + BOM/net and horn/rod fit; only then servo neutral/endpoints, battery monitor | PCB-A0..A4, MECH-G0..G3, HW-G0..G3 and measured servo current/voltage/temperature |
| D4: ESP-NOW control link | TX/RX pairing, peer/session, joystick and button mapping, telemetry/diagnostics (web optional later) | Radio mismatch/restart/loss, duplicate seq, watchdog and STOP proofs |
| D5: Two-wing bench tuning | Flap waveform, bounded steering, tuning UI and profiles | HW-G4 + repeatability, no overload, control evidence |
| D6: Tethered flight investigation | Lift/steering/CG/weight measurements and hypothesis log | HW-G5; reject unsafe combinations, revise mechanical design |
| D7: Controlled free-flight candidate | Limited flight trials and tuning | HW-G6 owner approval and completed risk review |
| D8: Future features | RC transmitter, LED effects, IMU stabilization, OTA | New ADR and individual Task approvals |

Each implementation phase should be split into small tasks with one testable outcome. No automatic jump from build PASS to flight. Phases are **sequence suggestions, not schedule promises**.

## 3. Task issuance and review transitions

DESIGN_DRAFT -> DESIGN_REVIEW -> DESIGN_APPROVED -> TASK_READY -> AI_CODE_IN_PROGRESS -> PR_REVIEW -> (CHANGES_REQUIRED -> PR_REVIEW)* -> SA_APPROVED -> OWNER_MERGE_DECISION -> DONE.

- SA creates one task file in Task/ only when prior gates and approved Design SHA are known.
- Implementation branch belongs to AI Code; PR cannot modify fundamental architecture without an ADR.
- SA reviews in isolated ReviewPR/PR-N checkout, comments on PR and keeps findings auditable.
- No blocker/major finding: post SA_APPROVED, clean **only** the isolated ReviewPR checkout, remove the assigned Task file via Git commit and record review outcome here, as requested by owner. The deleted task still exists in Git history.
- Blocker/major finding: post CHANGES_REQUIRED, keep review checkout and Task, append numbered fixes and regression criteria in the same Task, commit the update; repeat review when AI Code pushes.
- If the environment cannot actually checkout locally, disclose read-only review limitations; do not report build PASS. If unable to post PR comment, do not declare review completed.

## 4. Risks and controls

| Risk ID | Risk | Priority | Control |
| --- | --- | --- | --- |
| R-01 | Servo stall / heating, wing linkage collision | CRITICAL | HW-G3/G4; current logging and measured endpoint clamp |
| R-02 | 2S LiPo charging/balancing and power errors | CRITICAL | Correct 2S charger/protection, physical review, controlled bench source |
| R-03 | Old packets resume unexpected flapping | CRITICAL | Stop latch, fresh-session sequence, no auto-rearm, deterministic watchdog |
| R-04 | Brownout / reboot into active servo motion | HIGH | Safe boot initialization, hardware neutral/power tests |
| R-05 | ADC battery scaling wrong between reference wiring/config | HIGH | Measure actual divider/DMM, reject invalid readings |
| R-06 | UI state does not match device state | HIGH | Ack/device snapshot, stale/disconnected banner, no optimistic arm |
| R-07 | Original code/CAD licensing restricts distribution | HIGH | Reuse review before copying source, PolyForm noncommercial conditions |
| R-08 | Wing weight / aerodynamic performance insufficient | HIGH | Mass and lift budget, test data before making flight claims |
| R-09 | Overly large coupled firmware slows AI Code/review | MEDIUM | Focused modules and small PR tasks |
| R-10 | Worktree collision or accidental PR cleanup data loss | HIGH | Dedicated ReviewPR, worktree-safe removal, no deletion of unrelated paths |

## 5. Required owner design sign-off input

**Needed before marking DESIGN_APPROVED:**
- Confirm or adjust current ESP-NOW gamepad primary M0 scope; two firmwares and optional web diagnostics only.
- Exact gamepad MCU, button/joystick GPIO and currently installed firmware/source (currently unknown).
- Servo spline/horn compatibility (reference SDS1601 25T Ø4.90 vs proposed 21T ~4.5) and actual two-fastener joint dimensions.
- Four carbon socket fit measurement and print parameters; first physical FC Mount fit plan.
- Gerber V1.1 schematic/BOM/assembly/net confirmation, 2S charge/balance and PCB power safety gate.
- MVP target: stationary/showpiece, bench/tethered, or controlled free-flight ambition.
- Actual board, servo model, pack and buck, if already purchased.
- License intent: hobby/noncommercial only, or possibly commercial distribution.
- Physical style: follow original wing outline or original redesign; expected weight/size constraints.
- Confirm safety-first UI architecture and one-controller-at-a-time policy.

If hardware remains unknown, owner may approve **software design only** and explicitly leave wiring/flight tasks BLOCKED; record the partial scope precisely.

## 6. Release definitions

- **SOFTWARE_PASS** = documented build + tests on named revision. Does not prove output signal, mechanics or flying.
- **BENCH_PASS** = witness results of specific powered safety test at listed board, pack, servo and load; no broader safety claims.
- **TETHER_PASS** = tested tethered lift/steering evidence for specific geometry and environment.
- **FLIGHT_CANDIDATE** = owner approves bounded flight test after all prior gates.
- **REJECT** = safety invariant failed; stop and root-cause before retest.
- **NOT_RUN** = no evidence; never presented as pass.

## 7. SA review audit ledger

| PR | Task | Base/head reviewed | Result | Comment link | Cleanup commit | Date |
| --- | --- | --- | --- | --- | --- | --- |
| None yet | - | - | - | - | - | 2026-11-23 |

Review ledger entries are immutable audit facts; corrections append new records rather than quietly overwriting previous outcome.

## 8. Rules-first update log — 2026-11-23

- Owner-reported available hardware and uploaded source hashes recorded in [12_AS_BUILT_INPUTS_AND_BLOCKERS.md](12_AS_BUILT_INPUTS_AND_BLOCKERS.md).
- New domain gate rules: [13_ESP_NOW_DUAL_FIRMWARE_RULES.md](13_ESP_NOW_DUAL_FIRMWARE_RULES.md), [14_ARMV5_HORN_AND_CARBON_FIT_RULES.md](14_ARMV5_HORN_AND_CARBON_FIT_RULES.md), [15_PCB_V11_ASSEMBLY_AND_POWER_RULES.md](15_PCB_V11_ASSEMBLY_AND_POWER_RULES.md).
- This updates the DRAFT architecture and agent rules; it **does not count as approval of final dimensions, firmware, electronics or powered testing**.
- Explicit user instruction: **WRITE/PUSH RULES FIRST, DO NOT CREATE TASK YET**. Task/ remains absent. This instruction is independent of eventual design approval.

## Horn pre-task design review checkpoint

- [16_HORN_3HOLE_2BOLT_PRETASK_PLAN.md](16_HORN_3HOLE_2BOLT_PRETASK_PLAN.md) records the new uploaded STL SHA, geometric analysis and two fastening alternatives.
- SA preliminary candidate: two M1.6 through-bolts per wing with Ø1.8mm clearance and three provisioned horn holes. No physical strength or fit PASS.
- **Closed owner design-choice gate:** fully printed horn with a **direct SDS1601 mating spline** and 3 collinear holes. Do not ask whether to use metal or a triangular pattern again. **Open technical gates:** servo spline measurements, printer/nozzle/material capability, printed micro-spline fit/fatigue, shaft-center retainer and real fastener stack.
- **No Task** and no derived STL should be created just from this preliminary plan.

## 9. Next SA checkpoint

Present current Design draft to owner for approval. On explicit approval, commit updated ADR states and approved baseline, then create the **first** implementation Task file under Task/ with exact files-to-read, scope, expected files, test evidence and stop conditions. Do not issue Task at this draft stage.
