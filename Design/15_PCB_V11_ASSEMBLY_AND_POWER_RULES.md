# 15 — PCB Gerber V1.1: Assembly / Power / Cross-Discipline Verification Rules

**Status:** RULES_DRAFT / 2026-11-23. **Role:** AI-HW, with AI-3D and AI-SW handoffs.  
**Critical:** Owner already has PCB from the submitted Gerber and is preparing to populate components. Gerber **does not replace** schematic/BOM/netlist, nor prove the board safe to energize. This file records what was actually inspected and mandates stop/go conditions, not an implementation Task.

## 1. Fixed source identity and archive audit

Supplied archive **ESP-FLUTTER_PCB_GERBER_V1.1-Max_Imagination.rar**, SHA256: 55469956322e8fb065a52d4da7a66c9622db8eb9ebac841eed0d4491120ac345.

Inspected entries: 12 files total — F_Cu, In1_Cu, In2_Cu, B_Cu; F/B Mask; F/B Paste; F/B Silkscreen; Edge_Cuts; and one drill *.drl. This indicates **four copper layers**, not a validated 4-layer design stackup. PCB outline bounding envelope from metric 4.6 Gerber Edge_Cuts: **~18.5mm × 45.0mm**. Exact outline contours, board thickness, cutouts, copper-to-edge clearance, minimum trace/space, stackup, via structure and fabrication rules must be independently reviewed.

The 2026-08-24 Gerber layer headers say **KiCad Pcbnew 10.0.4**; the drill file says **Flux PCB Export 1**. They may represent a converted/common design, but do not assume matching drill origin or revision. Drill file declares two 0.30mm tools and one 0.60mm tool; no larger mounting drill is apparent in that supplied drill text. No separate NPTH or pick-and-place/BOM/schematic file was part of this upload. These are **inspection observations, not an electrical PCB audit PASS**.

## 2. Mandatory checks before populating, soldering or battery connection

1. Compare fabricated PCB silkscreen revision to supplied Gerber V1.1 and any source production data. **Do not interchange V1.0/V1.1 footprints or BOMs**.
2. Open all four copper and non-copper layers in a professional CAM viewer. Match drill hits against pads/slots, inspect board outline, copper/edge/thermal relief, polarity markers, via connectivity and manufacturing stackup.
3. Obtain original **schematic and exact revision BOM** or reverse-engineer **with documented net trace evidence** before assigning component values/part numbers. Reference images are not a verified netlist.
4. Produce a **placement table**: refdes, functional block, manufacturer/MPN, package, value/tolerance, orientation/polarity, top/bottom, pad numbering, source evidence and approval status.
5. Do not infer a component solely from footprint appearance. Verify connector pinout and **which 2S voltage rail** every servo pin receives.
6. Check regulator topology (reference MP2307) and actual device variant: inductor, switch node, input/output capacitors, feedback resistor network, enable, thermal layout, power orientation and test pads. Never assume default 5V merely because the schematic label says 5V.
7. Verify battery voltage divider network and ADC pin: the reference's modular drawing, PCB schematic and firmware config disagree on values; measure/trace actual Rtop/Rbottom and calibrate with an independent DMM before firmware cutoffs.
8. Check servo-signal PWM pin map for actual XIAO ESP32-C3, correct power/ground orientation and boot GPIO voltage. Reserve antenna clearance and ensure no solder joint collides with mount.
9. Validate reverse polarity, pack keying, switch, fuse/protection and connection of two 1S cells as a balanced **2S pack** before powered assembly. No charging circuit is proven by this Gerber.
10. Obtain real assembly inspection evidence (magnification, bridge/short checks, polarity, no power). Do not power the board simply because component placement looks like a photo.

## 3. Power-on must have a staged authorization gate

**PCB-A0 DOCUMENTS:** board revision, source archive SHA, schematic/netlist or traced equivalent, BOM/assembly drawing, solder plan. BLOCKED until coherent.

**PCB-A1 VISUAL/CONTINUITY:** all parts identified, polarity/orientation checked; no short between VBAT/5V/3V3/GND, no foreign particles, connectors verified, resistance-to-ground logged. A multimeter beep alone is insufficient for full net verification.

**PCB-A2 CURRENT-LIMITED BENCH:** with owner authorization, use isolated current-limited lab supply *instead of LiPo* and no servos installed. Check regulator output, startup, idle current, ripple/temperature and ADC pin voltage. Define acceptable thresholds from actual part datasheets before powering.

**PCB-A3 ESP32 BOOT:** board boots without servo/battery wing connection; verify programming interface, pin states, ADC consistency with DMM and no USB backfeed when test power changes. STOP if abnormal current or heat.

**PCB-A4 ONE SERVO:** no wing load, validated voltage/pulse range, current monitoring, known mechanical neutral/STOP, individually verify polarity and signal. Requires AI-HW + AI-SW + owner.

**PCB-A5 TWO SERVOS & FRAME:** connect second servo and only then frame; check voltage sag, temperature and signal integrity; do not install wings until MECH gate.

**PCB-A6 WINGED BENCH / TETHER:** only after MECH-G3/4, actual battery protection check and accepted control/failsafe protocol. Never classify as airworthy.

For every gate produce PASS/FAIL/BLOCKED/NOT_RUN with photos/logs/instrument identity, raw observations, test duration, supply voltage/current-limit, circuit revision and firmware SHA. Any defect or undervoltage => STOP and root-cause, not repeat with larger current.

## 4. Specific LiPo and charging requirements

- Owner has **two 1S 3.7V 220mAh cells**, but actual series harness and balance connector are **not confirmed**. Nominal 2S is 7.4V; fully charged 2S typical LiPo is 8.4V only when cells are correct and balanced.
- Require charger explicitly rated for **2S LiPo**, with correct **per-cell balancing**, current configuration compatible with the *actual cell's* manufacturer charge rating, cell-pair health and polarity. Servo 8.4V compatibility does not make 8.4V charging safe.
- A 2S BMS/protection PCB (if selected) is not automatically a charger; a charger is not always a protective disconnect. Document both functions, relevant datasheets and failure modes.
- No ad-hoc current limiting resistor, unregulated USB module or trial-and-error charge-current increase. No powered tests with swollen/damaged cells, unknown wiring or a loose pack.
- Battery-monitor ADC may provide telemetry and cutoff guidance, but cannot individually balance series cells. Establish undervoltage/brownout and safe-shutdown limitations explicitly.

## 5. Interfaces to other agents

**ELEC-IF-001** to AI-SW: exact board rev, firmware GPIO/ADC pin, servo pins, voltage and logic levels, PWM refresh/pulse/neutral bounds, divider ratio + calibration method, boot pin states, fault semantics, verified vs provisional flags.

**MECH-IF-001** to AI-3D: physical board outline/orientation and thickness, installed component heights both sides, mounting/retaining details, antenna/USB/connector keepouts, PCB location, supply switch and pack removal.

**SW-IF-001** from AI-SW: boot-safe pin timing, expected servo signal range and armed/disarmed/watchdog behavior.

Cross-discipline edits require CHANGE_REQUEST to SA, not unapproved modification of mechanical CAD or firmware.

## 6. Assembly blockers

| ID | Blocker | Needed evidence |
| --- | --- | --- |
| PCB-B01 | Missing coherent schematic/BOM and footprint reference | PCB V1.1 refdes->part table, source provenance and confirmation of orientation |
| PCB-B02 | Mixed Gerber/drill generator headers | CAM viewer registration check and drill-to-pad match |
| PCB-B03 | Battery voltage-divider disagreement | Physical resistor/net trace and DMM calibration |
| PCB-B04 | Undefined 2S pack and protection/charging | Cell/charger specifications and harness/connector plan |
| PCB-B05 | Servo 25T/21T interface and mechanical bounds | AI-3D joint inspection; avoid powered servo with incompatible horn |
| PCB-B06 | PCB-to-FC Mount fit unverified | Dimensions, installed component keepouts and dry fit with AI-3D |
| PCB-B07 | Unknown gamepad electronics | Board ID, pinout, power/joystick map from owner before transmitter firmware design |

**No fabrication, soldering, powering, wing movement or flight step is authorized by this document itself.** Its role is to define prerequisites and review evidence.

**Current:** PCB_V11_ARCHIVE_INSPECTED / PCB_A0_BLOCKED / ELECTRICAL_SIGNOFF_PENDING.
