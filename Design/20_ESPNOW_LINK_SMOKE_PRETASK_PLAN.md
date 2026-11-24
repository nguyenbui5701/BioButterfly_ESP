# 20 — TASK-001 Pre-task Design: ESP-NOW Link Smoke Test

**Date:** 2026-11-24. **Owner:** SA. **Proposed assignee:** AI-SW, AI-HW consulted for actual TX board/USB. **Priority:** P0. **Status:** PLAN_READY / OWNER_SCOPED_APPROVAL_REQUIRED.  
**Existing Design baseline inspected:** main commit 3655a652239ab3aa859e4ff9b49f038cd0832ba8. This document is a design proposal, not an active Task or approval. Owner requested the first task plan for communication between the two ESPs. After owner approves this specific plan, SA records its immutable design commit SHA in the roadmap and issues Task/TASK-001-espnow-link-smoke.md. Other design/physical gates remain DRAFT/BLOCKED.

## 1. Goal and non-goals

Prove application-layer bidirectional ESP-NOW data exchange between the owner's ESP32 gamepad (TX) and standalone XIAO ESP32-C3 (RX). Isolate radio/toolchain issues before joystick mapping, flight protocol, servo or power integration. This is a small independent communication experiment and does not implement the production pilot protocol in Design/13. No production ARM, STOP, PWM, servo, ADC, battery, PCB, CAD, Web UI, BLE, Wi-Fi AP or flight functionality. A stale-link diagnostic in this task is NOT a flight failsafe or arm authorization.

## 2. Read before implementation

- Design/AGENTS.md; 05_AI_WORK_RULES.md; 08_MULTI_AGENT_RULES.md; 11_SENIOR_SOFTWARE_AGENT.md.
- Design/01_REQUIREMENTS.md; 03_SYSTEM_ARCHITECTURE.md; 04_HARDWARE_AND_SAFETY.md; 06_DECISIONS_AND_ROADMAP.md; 12_AS_BUILT_INPUTS_AND_BLOCKERS.md; 13_ESP_NOW_DUAL_FIRMWARE_RULES.md.
- Other domain documents per AGENTS.md as applicable; mechanical work is out of scope.
- Official Espressif ESP-NOW API for actual pinned SDK: https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/api-reference/network/esp_now.html and https://docs.espressif.com/projects/arduino-esp32/en/latest/api/espnow.html. MAC-layer send status does not prove application receipt. Application ACK is required. Keep callbacks short and match their signatures to selected SDK.

## 3. Preflight — do not guess TX target

- RX is owner's standalone Seeed XIAO ESP32-C3, not the Gerber PCB. Identify USB serial/bootloader access and safe board profile (PlatformIO candidate seeed_xiao_esp32c3; verify against installed platform).
- TX is owner's ESP32 DIY gamepad. Exact module/variant, flash size, programming interface, boot/reset method, existing firmware and USB supply remain unverified. Obtain board markings or USB chip identification using read-only tooling; do not assume ESP32 DevKit or joystick GPIO. Record board IDs, flash size, toolchain and upload method.
- Before writing TX flash, determine whether factory firmware needs preservation; obtain owner permission to overwrite after providing verified backup/recovery instructions (backup and verify where supported, otherwise stop if preservation required). Never erase NVS/flash globally as convenience.
- Both boards alone, powered only from their documented safe USB/programming inputs; confirm no 2S, servo, flight PCB, external actuator or dual supply attached. No wiring between boards required. Use no GPIO writes, PWM or external pin probing in firmware. Physical upload/test is performed by owner or authorized builder, not claimed by AI without logs.
- If TX board identification or safe upload is blocked, AI-SW may implement/test the portable codec and compile RX only; TX physical test remains BLOCKED. Do not substitute a generic board and label it actual gamepad PASS.

## 4. Proposed repository isolation

Single PR; branch feature/task-001-sw-espnow-link-smoke, based on main containing active approved Task.
Allowed future implementation paths ONLY under experiments/espnow_link/:
- platformio.ini: separate envs link_tx and link_rx, source filters, explicitly pinned platform/core/dependencies and exact board IDs; document verified commands. No guessing TX profile or incompatible dependency.
- include/link_protocol.h, src/link_protocol.cpp: shared fixed-wire codec and pure validation helpers; src/tx.cpp, src/rx.cpp: independent entry points; optional minimal helpers inside src/ and include/.
- test/: host/native codec tests and link-state tests; README.md: board preflight, provision/config, build/flash/test commands, serial workflow, troubleshooting; .gitignore for local secrets and .pio/build; evidence/: sanitized logs, run manifests and concise results.
- Do not edit Design/, Task/, Mechanical/, Hardware/, any other path or existing implementation. No wholesale third-party code import, build artifacts, real MACs/keys or device-specific secrets in Git. SA owns design/task.
- Example commands to document and validate once profiles exist: pio run -e link_rx; pio run -e link_tx; pio test -e native; pio run -e link_rx -t upload --upload-port <RX_PORT>; pio run -e link_tx -t upload --upload-port <TX_PORT>; pio device monitor --port <PORT> --baud 115200. Actual board USB CDC and boot behavior must be verified. Separate monitors; correct port labels.

## 5. Proposed bench-only wire contract (LINK_TEST_V1)

This intentionally distinct test protocol cannot be interpreted as a pilot command. Fixed 16-byte wire frame, explicit little-endian encoding (no raw struct cast), all fields unsigned:
| Offset | Bytes | Field | Meaning |
| --- | --- | --- | --- |
| 0 | 2 | magic | 0xB17E (wire 7E B1) |
| 2 | 1 | version | 1 |
| 3 | 1 | kind | 1 DATA, 2 ACK |
| 4 | 4 | run_id | nonzero manually provisioned session identifier |
| 8 | 4 | seq | DATA starts at 1, strictly increases within run |
| 12 | 4 | pattern | deterministic synthetic payload = seq XOR 0xA5A5A5A5 |

ACK echoes all fields except kind = 2. Example DATA for run_id=0x11223344, seq=1: 7E B1 01 01 44 33 22 11 01 00 00 00 A4 A5 A5 A5. ACK replaces byte 3 with 02.

- Both devices in Wi-Fi STA mode, no router connection, no AP, one explicitly configured identical allowed 2.4GHz channel (proposed default 6). Print actual channel, peer provision status, build and role at startup; verify channel after init. Use unicast fixed peers with local provisioning; real peer addresses not in commits.
- Configure encrypted unicast with locally provisioned PMK/LMK if supported by both verified targets. If an isolated, unencrypted bench diagnostic is necessary, document explicit owner/SA authorization and label NO_AUTHENTICATION, never approve it as a production pilot link. No public example keys or claims that MAC filtering is authentication.
- Both sides validate exact length, magic, version, kind, source peer, configured run_id, payload pattern and sequence. RX counts strictly new DATA only; duplicate/stale frames do not refresh age or count. TX accepts ACK only for actually sent seq in current run with exact echo; duplicates do not inflate delivery count. No retries for baseline measurement; late ACKs categorized with documented window.
- Each fresh run provisions a new run_id on both ends explicitly. On reset either endpoint ends its run; restart both with fresh run_id before resuming measurement. No automatic flight session, production pairing or anti-replay claim.
- Callbacks copy bounded metadata and bytes to queue and return; process in loop. Maintain bounded outstanding ACK tracking, queue-drop counters and send callback errors. No delay-heavy callback, per-frame serial flooding or endless retries.
- Target sending period 50ms (20Hz). Synthetic counter only; no joystick mapping. Schedule independent of serial output and ACKs, avoid overlapping outstanding MAC send operations (record skipped slots; do not hide errors).
- RX states WAITING / LINK_OK / LINK_LOST for diagnostics only. Age starts on valid DATA; duplicates/malformed packets do not refresh age. Proposed internal stale threshold 450ms, loop checks at <=10ms nominal; acceptance actual detection <=500ms on receiver monotonic clock from last accepted DATA. Returning valid data may restore diagnostic LINK_OK within the same run only if no endpoint rebooted; it cannot trigger any actuator.
- TX records RTT using local clock from accepted send attempt to matching ACK; no subtraction of unrelated ESP clocks or claim of one-way latency. Report RSSI only if SDK exposes valid receive metadata.

## 6. Verification plan (one focused task, no powered flight hardware)

| Gate | Required evidence / acceptance |
| --- | --- |
| RF0 | Exact actual TX and RX board identity, safe USB and firmware preservation confirmed; local provisioning and pinned SDK documented |
| RF1 | Native unit tests PASS: golden bytes, roundtrip DATA/ACK, malformed/truncated/oversize, wrong version/kind, wrong peer/run/pattern, duplicate/reverse seq, invalid ACK, timer expiry and counter accounting; mock receive checks are labeled HOST |
| RF2 | Both real board firmware builds PASS with exact commands, versions and SHA; no servo/PWM/ADC libraries or code; correct distinct role banners |
| RF3 | Physical smoke: two standalone USB boards 0.5–1m apart, matching channel; 10 consecutive unique DATA/ACK pairs, matching seq/run/pattern; no duplicate counts |
| RF4 | Physical steady run: 60s at nominal 20Hz = 1200 scheduled slots; >=99% unique matching application ACKs relative to **all 1200 scheduled slots** (>=1188), no payload mismatches, no LINK_LOST or accepted duplicate/wrong frames; report skipped/attempted, send-error, MAC-success, RX-unique, ACK-unique, loss, RTT min/p50/p95/max and maximum valid RX gap. Proposed RTT p95 <=100ms in this bench only. Wait bounded 500ms drain for outstanding ACKs; report late/ignored separately. Include distance and conditions |
| RF5 | Three controlled TX pause/resume cycles (no reboot needed), pause >=2s; RX reports LINK_LOST <=500ms since last valid RX, no false freshness from invalid frames; fresh packets restore diagnostic link. Log monotonic transition timestamps and detection delay. Separately restart TX/RX and establish a fresh explicitly provisioned run; no automatic active pilot state |
| RF6 | Negative real-radio test: wrong run/version or duplicate frame from authorized bench TX, and optional channel mismatch if safe to configure; RX rejects invalid frames without refreshing age or ACKing them. Host-only cases not physically executed must be marked accordingly. Restore known configuration after test |

The quantitative targets are proposed smoke-test acceptance, not a reliable flight range specification. No long-range or high-interference certification, no one-way latency claim, no actual flight safety acceptance. If RF4/RF5 fail, investigate and report rather than relaxing threshold or hiding packet drops. Full production pairing/authentication, nonce/sequence rollover, ARM/STOP races and watchdog/servo safe pose remain later tasks under Design/13.

## 7. Evidence and PR review

- Raw TX/RX serial logs with timestamps, sanitized identifiers, exact Git commit/board/SDK/config and run parameters; aggregate CSV/JSON summary with denominator, counts, RTT and RX gap/timeout; screenshots optional, not replacement for raw logs.
- Report each RF gate PASS / FAIL / BLOCKED / NOT_RUN. If local build or hardware absent, report honestly; do not fabricate outputs.
- PR title: [TASK-001][AI-SW] ESP-NOW bidirectional link smoke test. Include exact approved design SHA, tests, changes, limitations, evidence, recovery/reflash instructions and RF-IF-001 experimental findings.
- SA reviews exact PR HEAD using isolated ReviewPR/PR-N where possible, posts formal comment, checks scope, correctness and evidence. No full link PASS or task deletion with missing required RF gates. On BLOCKER/MAJOR retain task and checkout, append fixes. On nonblocking complete review clean assigned task and SA checkout per Design/05; owner decides merge.
- Handoff: LINK_TEST_PASS permits planning next joystick/button mapping and production protocol tasks; it does not close PCB, servo, CAD, battery or flight gates.

## 8. Gate before Task issuance

Owner requested planning the first communication task. SA proposes scoped approval of this document ONLY, leaving all other Design drafts and hardware gates unchanged. After approval of exact plan commit, update 06_DECISIONS_AND_ROADMAP.md to record scoped design SHA and owner decision, then create Task/TASK-001-espnow-link-smoke.md; do not launch implementation from this pre-task document alone.
