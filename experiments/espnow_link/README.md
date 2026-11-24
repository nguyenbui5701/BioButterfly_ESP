# TASK-001 — LINK_TEST_V1

AI-SW; approved design SHA 5f133d4e63fd2aa14085600e6742a847f151ea4e; ADR-019 0cac45ae57ab9534911f239d3a526e7497de9681. Base main e39157fade463f7d398db2445e8c5d98754414b7. PR #1 remains draft.

## Preflight / owner evidence
TX: owner esptool 4.8.1 output ESP32-D0WD-V3 rev v3.1, WROOM-32 boot banner, 40 MHz crystal, 4MB flash manufacturer 0x5E device 0x4016; 3.3V strap. COM5 observed. Two complete 4194304-byte reads at 57600 baud, SHA256 identical (owner console output); backup binaries/hash stay local, recovery write NOT_RUN. Owner approves overwrite after RF0.
RX: owner esptool 4.8.1 output ESP32-C3 QFN32 rev v0.4, embedded XMC 4MB manufacturer 0x46 device 0x4016, 40 MHz crystal, COM3 observed. Owner identifies standalone Seeed XIAO ESP32-C3 and waives backup, approves overwrite after RF0. Owner confirms independent USB only, no battery/2S, flight PCB or servo.
TX strapping: hold inner-left red button (GPIO12 inferred), outer-right red button (GPIO2 inferred) and module BOOT, press/release EN. Boot log 0x03 DOWNLOAD_BOOT / waiting for download. Sometimes must do this during Connecting. For normal SPI boot, hold inner-left red only during EN reset and release after reset; do not hold BOOT/outer-right. Normal boot with GPIO12 high previously failed (invalid header). No soldering or GPIO probing. TX serial backup was unreliable at 115200; 57600 succeeded twice.
RX: hold BOOT, press/release RESET, release BOOT; check COM again. USB CDC enabled in RX build. No pins/servo outputs used. Antenna must be connected as specified by Seeed before RF testing.

## Build first — NO FLASH until RF0 complete
Toolchain pins: PlatformIO Espressif32 6.9.0, Arduino package 3.20017.0 (Arduino 2.0.17, IDF 4.4.7); native 1.2.1, Unity 2.6.0. Board targets esp32dev (verified WROOM-32 hardware characteristics, not vendor schematic) and seeed_xiao_esp32c3. IDF API https://docs.espressif.com/projects/esp-idf/en/v4.4.7/esp32c3/api-reference/network/esp_now.html. Source written against IDF 4.4 callback API; target compile NOT_RUN in AI environment. No Internet/git checkout or PlatformIO in AI container. Initial GCC host subset evidence in evidence/host-test.log pertains to older source revision; current native suite still NOT_RUN.

Windows PowerShell from this directory:
```powershell
python -m pip install platformio==6.1.18
python -m platformio --version
python test/provision.py
git check-ignore include/local_config.h
python -m platformio test -e native
python -m platformio run -e link_rx
python -m platformio run -e link_tx
python -m platformio pkg list -e link_rx
python -m platformio pkg list -e link_tx
Get-FileHash .pio/build/link_rx/firmware.bin,.pio/build/link_tx/firmware.bin -Algorithm SHA256
git rev-parse HEAD
```
Provision script asks for TX/RX STA MAC from the owner's own esptool outputs (never paste keys or MACs in PR); creates ignored include/local_config.h with random 16-byte PMK and LMK. Run once, both builds share this config. Keep header and binaries private; don't commit. Init compares actual STA MAC to expected and fails closed if mismatched; only encrypted unicast is implemented. No AP, router, plaintext fallback or automatic send. Native tests do not need local config. Windows native requires g++ on PATH (e.g. MSYS2 UCRT64 GCC); use `g++ --version` to check. If missing, report and install verified host toolchain before native testing. Target build script embeds exact Git HEAD and rejects dirty scoped source. Capture complete build/test logs OUTSIDE repo to avoid dirty source. Do not upload if any build failed or security checks unresolved.

## Read-only security precheck
Before write, enter each bootloader and read eFuse summary only (no burn/write commands).
```powershell
python -m espefuse --chip esp32 --port COM5 --before no_reset summary
python -m espefuse --chip esp32c3 --port COM3 --before no_reset summary
```
Ports may change; verify each. Keep raw output private; report only security status (FLASH_CRYPT_CNT or SPI_BOOT_CRYPT_CNT, ABS_DONE_0/1 or SECURE_BOOT_EN and download restrictions). Stop if security enabled/uncertain, no force. RF0 still pending local provision, pinned toolchain verification and security review.

## Upload after explicit RF0 closure
Close monitors; use confirmed manual boot procedures. TX must stay at verified 57600 speed. Platform config specifies no_reset before/after; inspect verbose upload command. On TX use three-button reset during Connecting if needed. RX manually enter BOOT mode and check new port.
```powershell
python -m platformio run -e link_rx -t upload --upload-port COM3 -v
python -m platformio run -e link_tx -t upload --upload-port COM5 -v
```
No erase_flash or erase NVS. After successful write and verify, manually reset RX normally, TX with inner-left red only (GPIO12 low at reset). Check ports again. Stop on write error, mismatch, unexpected hardware behavior; no force.

TX rollback only if required and authorized and security precheck permits: verify local backup size/hash against saved independent original; manual download mode; command from local backup directory:
```powershell
python -m esptool --chip esp32 --port COM5 --baud 57600 --before no_reset --after no_reset write_flash --flash_mode keep --flash_freq keep --flash_size keep 0x0 TX_factory_backup.bin
```
No preliminary erase; full overwrite is destructive and ONLY for rollback. Read back full flash and compare original SHA256, then normal TX reset as above. Recovery procedure not physically validated yet. RX backup explicitly waived. No eFuse changes.

## Physical RF3–RF6
Fit RX antenna according to Seeed instructions; both boards USB only, 0.5–1m apart. Open separate monitors, ideally store raw logs outside repo (PlatformIO log2file writes local files; use outside repo or ignored local/ directory). In each monitor send `info` followed by LF to get exact build SHA, role, SDK, verified channel and encryption readiness even if startup banner missed. If INIT_FAILED, mismatch or channel not OK, stop. Record conditions and firmware hashes. No secrets in logs.
One run per boot. Reset both before every new mode; choose a fresh nonzero 8-hex-digit run ID on both (never reuse). Start RX first then TX with exactly same command: `start XXXXXXXX s` (replace X with same actual hex ID). After TX_DONE, send `end` to RX, then `dump` to both for full timestamped slot/receive traces. No serial commands during measurement. Capture both logs including startup and transitions.
- RF3 mode s: 10 scheduled slots; require 10 consecutive matched sequences and RX valid data. Check no accepted duplicates.
- RF4 mode b: 1200 slots at 50ms over 60s plus bounded 500ms drain. TX starts 1s after command. Require >=1188 unique matching ACK / ALL 1200 scheduled slots, p95 <=100ms, no LINK_LOST during baseline interval and no accepted malformed/duplicate frames. RX intentionally continues watchdog after final slot: terminal LINK_LOST after cessation is recorded, not hidden; distinguish terminal loss from in-run loss using receiver trace, sequence and relative local time. If ambiguous, gate remains unproven. No cross-clock one-way latency. TX_SLOT outcome S accepted send, K skipped, E send API error, N intentional invalid version in negative mode. Trace timestamps use local monotonic millis. RTT nearest-rank min/p50/p95/max; no matching ACK means RTT unavailable. Check TX scheduled=skipped+attempted; attempted=send_error+accepted sends; count MAC callbacks and queue drops, pending, late/outside and unexpected separately; report loss=(1200-ack_unique)/1200. RX unique, max_gap_ms and all rejections reported. No retries in baseline. Trace overflow or unexplained counters require investigation.
- RF5 mode p: 230 scheduled slots, three 50-slot intentional pauses (slots 21–70,91–140,161–210; 2.5s each). Verify three in-run loss/recovery transitions, actual detection delay <=500ms, fresh data recovery; inspect local RX gaps/timestamps. Terminal loss after run is separate. Then reset both, new ID, mode s; record both boot banners and fresh successful DATA/ACK.
- RF6 mode n: 90 slots, valid 1–20, wrong-version 21–70, valid 71–90. RX rejects wrong version (Result enum version=3), no ACK/age refresh during invalid stream; TX unexpected ACK must be zero; compare rejection timestamps and last_valid_ms, verify recovery after slot 70. Wrong peer/run/duplicate covered by host only unless separately executed. Channel mismatch optional, NOT_RUN. No persistent changes to known config.
Save separate raw logs, sanitized copies for evidence, and computed JSON/CSV summary before SA final review. Don't claim RF PASS from source or compile.

## Scope
Only experiments/espnow_link changes. No original ESP-FLUTTER code/assets copied. No production flight pairing, ARM/STOP, servo/PWM/ADC, joystick, GPIO writes, battery or PCB. New dependencies are pinned Arduino/ESP-IDF and Unity for host only; transitive package list to be captured from actual build. RF-IF-001 experimental; no production approval. Revert scoped commits on feature branch if needed; no force push, self-merge or Design/Task edits.
