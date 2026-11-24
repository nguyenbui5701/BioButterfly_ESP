# TASK-001 — LINK_TEST_V1 (partial implementation / host only)

Role AI-SW; approved design SHA 5f133d4e63fd2aa14085600e6742a847f151ea4e; ADR-019 approval 0cac45ae57ab9534911f239d3a526e7497de9681; base main e39157fade463f7d398db2445e8c5d98754414b7.

## Current stage
S02 portable foundation implemented. RF0 blocked: exact gamepad TX MCU/module, flash, USB/boot and preservation not verified; RX reported XIAO ESP32-C3, actual revision and USB behavior not verified. Local environment cannot clone via git (DNS failure); branch made from verified main using GitHub API. No PlatformIO installed; no target firmware or flash commands authorized yet. No TX/RX entrypoints or radio implementation in this checkpoint. Do not flash anything from this revision.

## Host validation
From experiments/espnow_link, with GCC (tested 14.2.0):
```sh
g++ -std=c++11 -Wall -Wextra -Werror -pedantic -fsanitize=undefined -Iinclude src/link_protocol.cpp test/test_host/test_main.cpp -o /tmp/bio-host-test
/tmp/bio-host-test
```
See evidence/host-test.log. Synthetic peer addresses and run IDs only. The receiver helper assumes a single fixed provisioned run, a caller that ticks at <=10ms nominal and uses monotonic local RX timestamps. Never reuse receiver/ledger across runs. The TX ledger is limited to 1200 baseline slots and rejects ACKs arriving >500ms after the corresponding accepted send attempt; the future adapter must also enforce the absolute run-end +500ms drain boundary, count callback queue drops, ensure single outstanding MAC send, schedule slots, and compute RTT quantiles. No RF timing is measured here. RF1 remains incomplete until native PlatformIO configuration/test and integration are available.

## S01 preflight requested from owner / AI-HW
- Photos of both sides of actual TX gamepad PCB, module/chip markings, USB/programming interface and BOOT/RESET controls; RX board markings/revision and USB connection.
- Confirm documented independent safe USB inputs; no attached servo, flight PCB, battery/2S, external actuator or second supply. Do not connect unknown pins.
- Determine if factory TX firmware must be preserved, and whether RX firmware must also be preserved. Owner permission to overwrite only after verified backup/recovery method. Do not erase flash or NVS. No firmware writes at this stage.
- Provide OS, PlatformIO/CLI version if installed, and the port enumeration for each board separately. On a verified safe USB input only, a read-only chip/flash ID procedure may be selected once programmer and esptool version are confirmed. Do not share real MAC addresses/keys in Git or public logs.
- Candidate RX board ID seeed_xiao_esp32c3 is listed in PlatformIO official board documentation; no actual target verification yet. Candidate SDK Espressif32 6.9.0/Arduino 2.0.17/IDF 4.4.7 is not a verified local toolchain or selected TX target. Verify/install/pin before target build. ESP-IDF reference: https://docs.espressif.com/projects/esp-idf/en/v4.4.7/esp32c3/api-reference/network/esp_now.html

## Future builder sequence (NOT executable in this revision)
After S01, actual pinned environments and S03 implementation, run pio test -e native, pio run -e link_rx, pio run -e link_tx, capture outputs and binary hashes. Only after RF0 and owner flash authorization, use the verified per-board upload/recovery commands. Provision local encrypted unicast peer MAC/PMK/LMK and fresh matching nonzero run ID on both endpoints; never commit these values. No plaintext fallback without separate SA/owner authorization. Separate 115200 serial logs; verify actual channel and roles. RF3: 10 consecutive unique DATA/ACK pairs at 0.5–1m. RF4: 1200 slots, 50ms cadence, 60s and bounded 500ms drain; >=1188 unique matched ACK, p95 <=100ms, no LINK_LOST or accepted bad/duplicate frames. RF5: three pauses >=2s, detect <=500ms from last valid DATA, resume via fresh DATA; restart both with fresh run. RF6: wrong run/version or duplicate real-radio negative test without ACK/freshness; restore configuration. Collect sanitized separate TX/RX raw logs, counts, RTT min/p50/p95/max, RX gap, timestamps, conditions and JSON/CSV summaries.

## Scope / rollback
All modifications restricted to this experiment. Original implementation from approved LINK_TEST_V1 contract; no third-party ESP-FLUTTER code/assets imported. No GPIO/ADC/PWM/servo code, no actuator tests, no dependencies added at this checkpoint. Revert experiment commits on feature branch if needed; no main or protected design/task changes, no self-merge. RF-IF-001 remains experimental and BLOCKED; not production interface approval. No LINK_TEST_PASS.
