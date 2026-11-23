# 06 — Architecture Decisions, Design Gates, Roadmap and Review Ledger

**State:** DESIGN_DRAFT. **Last reviewed:** 2026-11-23.  
**No approval has yet been given for the BioButterfly_ESP design.** Entries marked PROPOSED are not finalized design decisions. Do not issue Task files or implement firmware against this draft without owner sign-off.

## 1. Design decision records

| ID | Proposed decision | State | Reason / conditions |
| --- | --- | --- | --- |
| ADR-001 | Start with XIAO ESP32-C3 | PROPOSED | Default target in reference; physical board/pins not yet verified |
| ADR-002 | Arduino framework + PlatformIO, pinned build | PROPOSED | Repeatable ESP32 toolchain, host tests separate from hardware |
| ADR-003 | Mobile-first on-device web UI hosted from local Wi-Fi AP | PROPOSED | Zero cloud dependency and easy smartphone control |
| ADR-004 | One pilot control source and WebSocket v1 | PROPOSED | Deterministic authority/telemetry; session + anti-replay required |
| ADR-005 | Two digital servo wing actuation | PROPOSED | Reference concept; SDS1601 datasheet seen but part not confirmed |
| ADR-006 | Device-side fail-closed safety state machine, boot DISARMED | PROPOSED | Safety must not depend on phone, Wi-Fi or UI refresh |
| ADR-007 | Separate flight-weight frame and optional removable aesthetic shell | PROPOSED | Reduce first prototype mass and simplify access |
| ADR-008 | Phase 0 bench validation before tethered/free flight | PROPOSED | No assumption that working firmware produces flyable vehicle |
| ADR-009 | Reimplement modular firmware contracts rather than wholesale copying ESP-FLUTTER | PROPOSED | Reference license is noncommercial and original code has broad coupling |
| ADR-010 | Use original wing reference as study material, create original approved CAD for our geometry | PROPOSED | Mass/scale and asset licensing decisions remain open |
| ADR-011 | No other control transports (ESP-NOW, ELRS/CRSF) in M0 | PROPOSED | Minimize initial safety/latency/control arbitration risk |
| ADR-012 | SA + 3 senior specialist AI roles (3D/HW/SW), shared interface/handoff contracts and isolated PR ownership | PROPOSED | Design/AGENTS.md and 08–11 define working protocols; no specialists automatically instantiated |

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
| D1: Host-only logic foundation | Build skeleton, shared types, input schema, state machine, fake clock tests | Unit-test proofs incl STOP, timeout, stale sequence and invalid input |
| D2: ESP32 board bootstrap | Pinned PlatformIO build, HAL, safe disarmed boot, serial diagnostics | Valid actual board/pin map; boot no-motion evidence |
| D3: Power/servo integration | Servo neutral/endpoints, battery monitor, calibration storage | HW-G0 to HW-G3, measured servo current/voltage/temperature |
| D4: Control communications + UI | AP, WebSocket, pilot session, mobile cockpit, telemetry | End-to-end timeout, disconnect and emergency STOP proofs |
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

## 8. Next SA checkpoint

Present current Design draft to owner for approval. On explicit approval, commit updated ADR states and approved baseline, then create the **first** implementation Task file under Task/ with exact files-to-read, scope, expected files, test evidence and stop conditions. Do not issue Task at this draft stage.