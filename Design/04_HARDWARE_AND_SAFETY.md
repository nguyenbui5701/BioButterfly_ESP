# 04 — Hardware, Mechanics, Power and Safety

**State:** DESIGN_DRAFT, 2026-11-23; updated owner-hardware inputs in [12_AS_BUILT_INPUTS_AND_BLOCKERS.md](12_AS_BUILT_INPUTS_AND_BLOCKERS.md) and mandatory PCB rules [15](15_PCB_V11_ASSEMBLY_AND_POWER_RULES.md). Distinguish REFERENCE_VERIFIED (seen in original ESP-FLUTTER files), LOCAL_VERIFIED (measured on our assembled hardware) and PROPOSED (SA choice). Owner **reports physically possessing** XIAO ESP32-C3, SDS1601 servo(s), two 3.7V 220mAh cells, ESP32 gamepad and Gerber-derived PCB; owner-printed ARMV5 has mechanical fit issues. **No local powered/assembly/horn measurements have been verified yet**.

## 1. Hardware reference and decision status

| Subsystem | Reference evidence | Our decision |
| --- | --- | --- |
| MCU | Seeed XIAO ESP32-C3 | Owner reports has board; confirm revision, GPIO and antenna |
| Actuation | 2x SDS1601 reference | Owner reports SDS1601 present; exact count, **25T reference vs proposed 21T horn** and measured electrical/mechanical parameters remain pending |
| Pack | Reference depicts 2x Tattu 1S 3.7V 220mAh in series | Owner reports two **3.7V 220mAh** cells, not a verified Tattu/assembled 2S pack; charging/balancing/harness must be designed |
| Logic regulator | Buck 8.4V -> 5V shown in modular diagram | Candidate, measured load and transient validation required |
| Flight controller | Owner supplied **ESP-FLUTTER Gerber V1.1 RAR** for PCB currently being prepared for component assembly; reference PCB uses MP2307 | Four copper layers and approx 18.5 × 45mm outline observed. Gerber/drill registration, matched BOM/netlist and power tree NOT verified; see [15](15_PCB_V11_ASSEMBLY_AND_POWER_RULES.md) |
| Wing architecture | Mirrored Left/Right wing outlines | Owner supplied **ARMV5 printed** (horn and 3 of 4 rod socket fit issues) and **FC Mount V5 STL unprinted**; follow [14](14_ARMV5_HORN_AND_CARBON_FIT_RULES.md) |
| Radio pilot | ESP-NOW gamepad (owner's explicit plan); optional ELRS reference | **M0 TX gamepad + RX XIAO**; other RC sources deferred. See [13](13_ESP_NOW_DUAL_FIRMWARE_RULES.md) |

## 2. Evidence from the original Drive folder

Original reference: https://drive.google.com/drive/folders/1XTTW8Ez_hmZl5E6mPUDXgX2qnnI3Cp2e

- Datasheets/SDS1601 Servo Specs.jpg specifies PWM neutral **1520us**, refresh **333Hz**, pulse width **800–2200us**, advertised max 180-degree travel, supply **5.0–8.4V**, approximate mass **16g**, body **24.3 x 12 x 22.3mm**.
- Advertised SDS1601 speed is **0.08s per 60 degrees at 7.4V**, torque **7.0kg-cm at 7.4V**; **actual stall current, running current, heating and longevity are not provided by this image**.
- Wing Files/Left Wing.pdf and Right Wing.pdf are mirrored templates. The rod chart specifies, **per wing**, 1.5mm rods: main vein 315mm, root attachment 90mm, hindwing side attachment 35mm, leading-edge support 60mm; 1.0mm rods: leading edge 400mm, top vein 255mm, upper-mid 247mm, lower-mid 237mm, bottom 226mm, hindwing half 356mm x2. These dimensions are reference-only, not locked for our CAD.
- Print settings.jpg shows sample slicer parameters: 0.16mm layers, 0.2mm first layer, 2 walls, 15% rectilinear sparse infill and supports disabled. Printer/material-specific validation is required.
- Wiring Diagrams/Modular Version shows servo pins D0 and D1, ADC on D2, wing LED on D3, D6/D8 status LEDs, optional ELRS on D9/D10.
- IMPORTANT: the modular drawing, PCB schematic and firmware config show *apparently different resistor-divider top/bottom values*. Firmware config.h describes 49k/18k while the PCB schematic depicts 51k/18k, and modular drawing appears to label 51k/20k. **Do not select ADC calibration from any single picture.** Check actual resistor values, topology and an independent DMM reading. Full battery ADC design is BLOCKED until resolved.

Reference source: https://github.com/maximagination1/ESP-FLUTTER. Original CAD/image assets and source may be licensed separately. Do not copy files into our repo without permission/license audit.

## 3. Existing build-specific stop gates

New horn source and two-bolt coupling plan are recorded in [16_HORN_3HOLE_2BOLT_PRETASK_PLAN.md](16_HORN_3HOLE_2BOLT_PRETASK_PLAN.md): three lever holes, two installed M1.6 candidate bolts, two matching wing-root holes and a separate center shaft retention screw. Source SG90 horn STL has no confirmed SDS1601 25T spline. This is NOT a powered-test authorization.

- **SDS1601 horn:** reference datasheet image says **25T-Ø4.90mm**, but owner proposes metal **21T/~4.5mm**. Confirm real splines; never force mismatch or design root based on an assumed mating.
- **ARMV5:** two disconnected watertight parts in source STL, not a single 79.68mm-long arm; owner reports 4 intended rod sockets with 3 unusable. Do a measurement table, coupon test and new two-fastener horn pattern per Design/14.
- **PCB:** uploaded Gerber V1.1 contains four copper layers, but drill header and Gerber layer headers show different CAD-export tools. Require CAM drill-pad alignment and full component/BOM/net audit, not merely correct layer counts.
- **FC Mount:** no real print or fit validation; board outline and 3D bounding-box numbers alone do not establish connector/antenna clearance.
- **Battery:** user has two cells, but 2S topology, charging/balancing, protection and servo-current budget are not yet signed off.

## 4. Proposed body and packaging

- Two side-by-side servo mounts, symmetric film wings and carbon spars; small central frame mounts MCU, regulator, battery connector and pack securely.
- Minimize mass and moment of inertia; place center of gravity near flight-relevant centerline. Define dimensions from purchased parts, not guesses.
- Prefer **flight-weight core plus removable cosmetic shell** rather than heavy 3D-printed full body on first flight attempt.
- Ensure independent access to each horn, connector and power switch; guard pinch zones and relieve cable strain.
- Record actual wingspan, total mass, center of gravity, wing area, servo torque reserve, linkage travel and safe clearance after first mock-up.
- Mechanical calibration must use real endpoints, not advertised 0–180-degree travel.

## 5. Proposed electrical block

Battery 2S -> keyed connector -> accessible switch and suitable fuse/protection -> two isolated design branches:

1. Rated HV servo supply to both SDS1601 servos (only if rated for full 8.4V plus tolerances).
2. Validated buck converter output -> appropriate regulated input of XIAO ESP32-C3.

All control and power grounds share reference where required. Pack -> verified divider -> ADC D2. Never connect raw 2S voltage to GPIO, ADC or 3V3. Validate USB/battery backfeed protection before simultaneous connection.

**Battery safety:** A 2S configuration requires a compatible CC/CV charger with suitable cell balancing and protection. Two series cells cannot be charged safely by assuming a USB supply, a resistor or an arbitrary 8.4V-only module will control per-cell charge current/voltage. Do not run without correct pack assembly, per-cell monitoring plan, protected connectors and thermal observations.

## 6. Servo and electrical bench procedure

1. Disconnect pack. Inspect polarity, wiring, fuse, regulator, connector orientation and electrical continuity.
2. Test regulated rails with current-limited source and DMM; check undervoltage, startup, servo transients and possible USB backfeed.
3. **Wings removed:** initialize PWM to safe neutral, check one servo at a time, ramp tiny movements, verify direction and measure current.
4. Check servo pulse frequency and range for the exact batch. Servo PWM refresh (e.g. 333Hz) is NOT wing flap frequency (e.g. 1–4Hz).
5. Install linkage, check interference across manual movement. Increase amplitude/frequency gradually, record current, voltage sag, heat and vibration.
6. STOP for stall, binding, overheat, cells swelling, unexpected motion or uncommanded restarts.
7. Only after bench acceptance, run tethered tests. Free-flight requires additional risk review.

## 7. Hardware gates

| Gate | Evidence / action | Status |
| --- | --- | --- |
| HW-G0 | Real parts list, purchased servo labels, pack capacity/chemistry, physical board photos | PENDING |
| HW-G1 | Signed wiring, fuse/switch, charger/protection, real resistor divider values, pin/boot pin review | PENDING |
| HW-G2 | DMM power and ADC measurements, brownout and USB-backfeed check | PENDING |
| HW-G3 | Servo endpoint/polarity/PWM, dynamic current and temperature logs | PENDING |
| HW-G4 | Two-wing low-rate 5-minute bench test, no binding or resets; STOP test | PENDING |
| HW-G5 | Tethered lift, steering, CG, mass, battery sag and safe operating envelope | PENDING |
| HW-G6 | Explicit owner approval for controlled free flight | PENDING |

**No compile PASS implies physical safety.** Flight tests remain blocked until all relevant earlier gates pass.

## 8. Owner decisions needed for hardware freeze

- Exact ESP board, servos, cells and regulator on hand.
- The priority: stationary animation, tethered prototype, or free flight.
- Whether original noncommercial reference CAD/wing patterns may be used, or original design is required.
- Charging and balancing design and enclosure target mass.

All current values beyond reference evidence are provisional until signed off.
