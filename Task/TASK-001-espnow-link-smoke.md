# TASK-001 — ESP-NOW Bidirectional Communication Smoke Test

**Version:** 1.0 | **Issued:** 2026-11-24 | **Owner:** AI-SW | **SA:** design/review owner | **Priority:** P0  
**Status:** TASK_READY / IMPLEMENTATION_AUTHORIZED_WITHIN_SCOPE  
**Base branch:** main (fetch latest containing this task)  
**Feature branch:** feature/task-001-sw-espnow-link-smoke  
**Approved design SHA:** 5f133d4e63fd2aa14085600e6742a847f151ea4e  
**Owner approval record:** 0cac45ae57ab9534911f239d3a526e7497de9681 (Design/06_DECISIONS_AND_ROADMAP.md, ADR-019)  
**PR:** pending; one focused PR, title [TASK-001][AI-SW] ESP-NOW bidirectional link smoke test.  
**Peer consultation:** AI-HW for board identity, USB/programming safety and factory-firmware preservation; AI-3D not required.

## 1. Goal

Implement and verify a small isolated **two-way ESP-NOW DATA/ACK link** between the owner's actual ESP32 gamepad TX and standalone XIAO ESP32-C3 RX. Establish whether both real boards can exchange valid data reliably and report link interruption. This is NOT production flight firmware or a complete safety supervisor. No prerequisite closure of the unrelated horn/PCB/power gates is implied.

## 2. Mandatory reading / precedence

First read current Design/AGENTS.md, Design/05_AI_WORK_RULES.md (scoped-approval clarification), Design/08_MULTI_AGENT_RULES.md and this Task. Confirm ROLE=AI-SW, TASK=001, exact approved SHA and permitted paths in your first response.
Read the immutable approved snapshot at 5f133d4e63fd2aa14085600e6742a847f151ea4e:
- [20_ESPNOW_LINK_SMOKE_PRETASK_PLAN.md](https://github.com/nguyenbui5701/BioButterfly_ESP/blob/5f133d4e63fd2aa14085600e6742a847f151ea4e/Design/20_ESPNOW_LINK_SMOKE_PRETASK_PLAN.md) — normative technical design, full RF0–RF6 and acceptance.
- Design/11_SENIOR_SOFTWARE_AGENT.md; 01_REQUIREMENTS.md; 03_SYSTEM_ARCHITECTURE.md; 04_HARDWARE_AND_SAFETY.md; 06_DECISIONS_AND_ROADMAP.md; 12_AS_BUILT_INPUTS_AND_BLOCKERS.md; 13_ESP_NOW_DUAL_FIRMWARE_RULES.md.
- Current approval record: [ADR-019](https://github.com/nguyenbui5701/BioButterfly_ESP/blob/0cac45ae57ab9534911f239d3a526e7497de9681/Design/06_DECISIONS_AND_ROADMAP.md).
- Official Espressif ESP-NOW documentation for exact pinned SDK: https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/api-reference/network/esp_now.html and https://docs.espressif.com/projects/arduino-esp32/en/latest/api/espnow.html.

Follow AGENTS.md read order for other applicable docs. The pinned plan's historical PENDING status is superseded by ADR-019 only; technical requirements are unchanged. If rules conflict, stop affected work and raise to SA, never broaden the scope. Do not import ESP-FLUTTER code/assets; only study reference if needed, subject to licensing.

## 3. Preflight and implementation sequence

S01 — Identify actual TX chip/module, flash size, USB/programming/boot method and existing firmware using owner photos or read-only tooling; verify actual XIAO ESP32-C3 target profile, USB CDC/boot behavior and safe independent USB power. Record exact board IDs and SDK versions. No assumed generic DevKit or joystick pins.
S02 — Implement portable codec and link-state/counter helpers, native tests and golden vector. RX build may proceed independently if TX identity remains blocked; explicitly mark TX actual target build/test BLOCKED.
S03 — Implement distinct TX and RX firmware environments with pinned PlatformIO toolchain and board profiles. Use same local channel, run ID and peer provisioning. Encrypted unicast is expected where supported; unencrypted bench mode requires separate explicit owner/SA permission, marked NO_AUTHENTICATION.
S04 — Run host tests and both actual-target builds. Submit logs, exact build SHA and upload/recovery steps.
S05 — Owner/builder executes physical RF3–RF6 after RF0 and safe flash authorization. AI-SW provides exact commands and explains how to collect separate serial logs and results. If hardware not accessible, submit honest NOT_RUN/BLOCKED evidence and keep task open.
S06 — Submit one PR containing scoped source/test/report; SA reviews exact PR head separately. No self-merge or task cleanup by AI-SW.

**Physical stop condition:** before flashing TX, check factory firmware preservation; obtain owner's permission to overwrite after providing verified backup/recovery instructions (backup/verify where supported; if preservation required but unavailable, stop). Never erase entire flash or NVS as routine. USB power only from documented safe inputs, no attached servo, flight PCB, battery 2S or external actuator, no dual power. No GPIO writes/PWM/ADC/external pin probing. Physical hardware claims must come from real logs.

## 4. Strict writable path allowlist

Only create/edit files within experiments/espnow_link/:
- platformio.ini: link_tx, link_rx and native test environments; explicit source filters and pinned platform/core/dependencies.
- include/link_protocol.h, src/link_protocol.cpp, src/tx.cpp, src/rx.cpp; minimal related helpers under include/ and src/.
- test/ (portable codec, link state, timer, accounting tests); README.md (preflight, provision, commands, recovery, troubleshooting); .gitignore (local secrets and build outputs); evidence/ (sanitized raw logs, manifests, summary).
Everything else READ ONLY: Design/, Task/, Hardware/, Mechanical/, existing source or unrelated paths. No committing real MAC addresses, peer identities/keys, binaries, local secrets or fabricated logs. No Web UI, AP, BLE, joystick, real ARM/DISARM/STOP, flight session/pairing protocol, servo, PWM, ADC, 2S, CAD or PCB work.

## 5. Protocol contract — LINK_TEST_V1

Fixed 16 bytes, explicit little-endian encoding, unsigned fields, no raw C++ struct casting. Offset 0 magic u16=0xB17E, 2 version u8=1, 3 kind u8=1 DATA or 2 ACK, 4 run_id u32 nonzero, 8 seq u32 starting 1 strictly increasing, 12 pattern u32=seq XOR 0xA5A5A5A5. ACK echoes all but kind.
Golden DATA vector run_id=0x11223344, seq=1: 7E B1 01 01 44 33 22 11 01 00 00 00 A4 A5 A5 A5; ACK byte 3=02.

- Both in Wi-Fi STA mode without router or AP; same allowed 2.4GHz channel (default proposal 6), actual channel verified; unicast known local peers. Locally provision keys/peer/run IDs without exposing secrets.
- Validate exact length/magic/version/kind/peer/run/pattern and sequence; RX counts only strictly new valid DATA; invalid and duplicate frames do not ACK or refresh last-valid age. TX counts only unique matching ACK for actually attempted/sent seq of current run.
- 50ms nominal period, no retries for baseline. Bounded outstanding tracking, bounded callback queues, account for skipped slots, queue drops and send failures. MAC callback success is not application ACK. Keep callbacks short; no logging flood.
- RX WAITING/LINK_OK/LINK_LOST diagnostics only; proposed stale threshold 450ms, nominal check <=10ms; measured detection <=500ms. Valid same-run data may restore diagnostic link after pause; after either reboot end run, explicitly provision fresh run_id on both endpoints. No automatic pilot behavior.
- RTT measured on TX local monotonic clock from accepted send attempt to matching ACK; no cross-clock one-way latency claim. Report actual valid RX gaps and timestamps. See full pinned design for remaining normative details.

## 6. Acceptance gates (all required; initial status NOT_RUN)

| ID | Acceptance | Evidence required |
| --- | --- | --- |
| RF0 | Actual TX/RX identities, safe USB, preservation and programming procedure, local peer setup, pinned SDK | Board marking/identification, verified commands, owner permission where overwrite required |
| RF1 | Host unit tests pass (golden bytes, roundtrip, malformed lengths/types/version, peer/run/pattern, duplicate/reverse, invalid ACK, timeout, counter accounting) | Actual native test command + raw output |
| RF2 | Both exact actual-board target builds pass, distinct role banners, no actuator/ADC/PWM code | Build logs and code audit; firmware SHA/toolchain |
| RF3 | 10 consecutive unique correct DATA/ACK pairs at 0.5–1m, same channel, no duplicate counts | Separate TX and RX logs |
| RF4 | 60s nominal 20Hz, exactly 1200 scheduled slots, >=1188 unique matched ACKs (>=99% of ALL slots), RTT p95 <=100ms; no payload mismatch, LINK_LOST, accepted duplicate/wrong frames | Raw logs, counts including skipped/attempted/send-error/MAC-success/RX-unique/ACK-unique/queue-drop, loss, RTT min/p50/p95/max, maximum RX gap, conditions and bounded 500ms drain; late/ignored separate |
| RF5 | 3 TX pause/resume cycles >=2s, RX loss detected <=500ms from last valid DATA, no false freshness, recovery by fresh DATA; restart both and establish fresh run | Receiver monotonic timestamps, actual detection delays and restart logs |
| RF6 | Real-radio wrong run/version or duplicate frame rejected without ACK/age refresh; optional channel mismatch if safe | TX/RX logs showing rejection and subsequent known-config restoration; report unexecuted host/physical cases separately |

No rounding up failed measurements, retries hidden in baseline, denominator based only on successfully sent frames or acceptance threshold changes without approval. If any required gate fails or is blocked, report cause, next action and actual scope; do not claim LINK_TEST_PASS. Full detailed plan at pinned Design/20 governs.

## 7. Local run commands and builder handoff

From repository root, cd experiments/espnow_link. Verify commands against actual selected toolchain and board profiles, then document in README:
- pio test -e native
- pio run -e link_rx
- pio run -e link_tx
- pio run -e link_rx -t upload --upload-port <RX_PORT>
- pio run -e link_tx -t upload --upload-port <TX_PORT>
- pio device monitor --port <PORT> --baud 115200 (separate monitors; board-specific CDC if needed)
These are proposed commands, not claims of successful build/upload. Never guess COM port, flash size or board identity. Provide owner with actual run order and a concise checklist for RF3–RF6, including 60s baseline start/end, pause/resume, restart and negative test procedure.

## 8. PR evidence / definition of done

- Task ID, role, pinned design SHA, exact implementation commit SHA, board/environment/build dependency versions, file list and test results PASS/FAIL/BLOCKED/NOT_RUN by RF gate.
- Raw sanitized TX/RX serial logs with timestamps, machine-readable summary CSV/JSON and reproducible accounting (denominator, counts, p95 calculation, RX gap and detection time), test run configuration and conditions.
- Recovery/reflash instructions, no secrets, licensing statement and known limitations. RF-IF-001 experimental handoff records radio channel, protocol rev, toolchain, actual board IDs, measured link results and unresolved issues; it is not a production interface approval.
- Create one PR against main and report URL. AI-SW must not modify Design or Task, change thresholds or merge its own PR.
- SA reviews exact head in isolated ReviewPR/PR-N, posts review decision and findings. If missing required evidence, keep Task open; if BLOCKER/MAJOR, retain review checkout and append remediation to this task. If approved with all required RF gates supported, clean exact review checkout and remove assigned task via SA commit, preserving audit history. Owner decides merge.

Next task planning only after SA review and LINK_TEST_PASS. Mechanical/PCB/battery/flight gates remain blocked regardless of RF result.
