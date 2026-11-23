# 17 — Bambu Lab A1 / SDS1601 25T–Ø4.9mm Printed Horn Validation Plan

**Status:** DESIGN_RULES / OWNER_INPUTS_RECORDED / PRINT_CAPABILITY_UNVERIFIED  
**Date:** 2026-11-24 | **Discipline:** AI-3D + AI-HW, SA sign-off  
**Scope:** Only plan/review an **entirely 3D-printed horn** that mates directly with the SDS1601 servo spline. Three straight-line mounting holes on the horn; two matching holes on ARMV5; two arm-to-horn screws installed per wing. No Task, geometry changes, STL exports or powered tests are authorized by this document.

## 1. Owner-confirmed facts vs unverified details

| Parameter | Current status | Implication |
| --- | --- | --- |
| Servo spline tooth count | **25T — OWNER_CONFIRMED**, consistent with reference SDS1601 sheet | Design as 25-tooth *female* internal spline, NOT SG90 circular bore or prior 21T idea |
| Servo spline outer diameter | **4.9mm nominal — OWNER_CONFIRMED**, consistent with reference sheet | Diameter alone does **not** define involute/tooth angle, root diameter, height, fit or tooth clearance |
| Printer | **Bambu Lab A1 — OWNER_CONFIRMED** | FDM; test process resolution and dimensional quality using actual printer |
| Nozzle installed | **UNKNOWN**; official A1 guide lists 0.4mm supplied, optional 0.2/0.6/0.8mm | Do not assume the owner still uses the stock 0.4mm |
| Filament | **UNKNOWN** (e.g. PLA or PETG) | Material strength/creep/fatigue and dimension compensation unknown |
| Print profile | UNKNOWN: layer height, speed, flow, filament dryness, cooling, seam orientation | No manufacturability or fit pass yet |
| Physical servo spline shape | UNKNOWN: true major/minor dimensions, tooth shape, engaged depth, center retention screw | Must measure/scan/check mating reference; no automatic 4.9mm smooth hole |
| Desired horn mounting geometry | **OWNER_CONFIRMED:** 3 collinear holes, two outer holes used, 2 bolts, arm has two matching holes | **Owner-approved P7:** 3.5mm adjacent and 7mm H1-H3 outer spacing, H2 unused; previous -5/-8.5/-12mm absolute locations are NOT approved; M1.6 remains a candidate |

**Approximation for feature-scale discussion, not a printable tooth spec:** 4.9mm × π / 25 ≈ **0.616mm circumferential pitch at nominal 4.9mm OD**. This is **pitch along outside circumference**, not tooth thickness, tooth root size or permissible print error. Those require real tooth profile.

## 2. New verified parametric source constraint (2026-11-24)

The user's screenshot shows **mezger/customizable-servo-arm**. Read [18_OPENSCAD_CUSTOMIZER_25T_REVIEW.md](18_OPENSCAD_CUSTOMIZER_25T_REVIEW.md) and [upstream OpenSCAD source](https://github.com/mezger/customizable-servo-arm/blob/main/customizable-servo-arm.scad). Its Futaba 3F preset is **25T, Ø5.92mm**, while the SG90 preset is **21T, Ø4.90mm**. **Neither matches owner's 25T, Ø4.90mm SDS1601.** Do not print either preset directly. Use measured custom female tooth geometry; a nominal diameter parameter change alone cannot prove spline fit. LGPL-2.1 license applies if modifying/distributing the original OpenSCAD.

Screenshot `arm_hole_distance=6mm` would mean 12mm outer installed screw pitch, but **owner explicitly approved P7** on 2026-11-24: **3.5mm adjacent / 7mm outer (H1/H3)**. P12 is not selected. Source generator auto-counts holes based on arm length; implement **exactly three specified hole centers**, independent of overall arm length. Determine H1's offset from axis by CAD/physical collision review, not from the old 5mm illustration.

## 3. Verified manufacturer printer data

Source: Bambu Lab official A1 Quick Start Guide (specifications, not independent test of the user's machine):  
https://cdn1.bambulab.com/documentation/quick-start-a75adcb1d5d5e/Quick%20Start%20Guide%20for%20A1.pdf

- Process: FDM, 1.75mm filament.
- Factory-included nozzle: **0.4mm**; official optional nozzles **0.2mm, 0.6mm, 0.8mm**.
- Printer lists PLA/PETG/TPU/PVA as suitable, while other advanced filaments are less suited per official guide. This does **not** prove any listed material will withstand SDS1601 repetitive torque.
- **Engineering recommendation:** compare 0.2mm nozzle for spline fidelity; 0.4mm nozzle can be attempted as a control specimen only if tiny inner teeth actually survive slicing. The current installed nozzle must be checked before making test profiles.
- If no 0.2mm nozzle exists, do **not** promise successful spline printing from 0.4mm. Review Bambu Studio slice preview for missing/fused teeth; note actual current nozzle in report.

## 4. Mandatory parametric geometry, not a circular drilled hole

1. Take the uploaded `SG90_one_horn_without_holes.stl` as a **shape reference** (source SHA in Design/16); keep unchanged and create native editable parametric spline model as a **derived** design only after permission/Task.
2. Obtain/measure actual SDS1601 output: tooth count, shaft major/minor diameter, tooth height/shape or verified OEM matching horn reference, engagement depth, original center retention screw/head, axial stack and any undercut/shoulder.
3. Reconstruct **female 25T mating surface**; record tooth profile construction method and dimensional uncertainty. The nominal 4.9mm servo spline diameter is NOT an internal hole diameter to drill.
4. Make horn hub wall thickness, lever root fillet, retention screw bore/counterbore, engagement depth and rotation datum explicit.
5. Check the internal teeth in both CAD and **Bambu Studio sliced preview**; reject slicing that deletes teeth, closes gaps or leaves unbonded thin fragments.
6. Keep hub axis oriented for high-fidelity XY tooth outline (test nominal hub-axis approximately vertical during coupon print), with entry chamfer/elephant-foot compensation; layer direction and lever attachment must also be checked for cyclic torque and root fatigue.
7. Do not force a too-tight print onto the servo, heat the servo spline, cut the servo's teeth or use glue to conceal a failed mating design.

## 5. Coupon-first print matrix (small test pieces)

**Do NOT start with the entire wing / horn.** Coupon is a short, sacrificial spline hub including center retainer clearance. An additional fastener-hole coupon can be printed separately.

| Stage | Candidate | Checks | Disposition |
| --- | --- | --- | --- |
| P0 | Measure actual shaft + original servo retention screw | Tooth geometry and engagement depth, uncertainty | BLOCKED until observed |
| P1 | Generate nominal 25T female spline in native CAD | Tooth count/phase, major/minor radii, no interference with shaft shoulder | CAD_CHECK_ONLY |
| P2 | Bambu Studio 0.4mm nozzle **if actually installed** | Inspect slice preview at every spline level; print sacrificial coupon if geometry preserved | Can fail due to feature loss |
| P3 | Bambu Studio 0.2mm nozzle **if available** | Repeat same design/material/orientation, print coupons, compare visible tooth fidelity | Preferred fine-feature experiment; not guaranteed pass |
| P4 | Fit variants based on actual observed over/under-extrusion | Parameterize female profile offset and entry chamfer; alter one variable at a time | No unmeasured universal clearance |
| P5 | Fit by hand on power-off SDS1601 with no wing rods | Full seating, low backlash, no shaft damage, center screw retention, repeat install/removal | HORN_SPLINE_STATIC_PASS only if documented |
| P6 | Printed horn + ARM root trial after P5 | 3 inline mounting holes, 2 outer bolted to ARM, net ligament, screw head/nut clearance, mass and slip | HORN_ASSEMBLY_PASS only if documented |
| P7 | Owner/AI-HW-authorized instrumented no-wing cycle and later wing test | Slippage, tooth wear, fatigue, motor current/heating, looseness over repeated reversal | POWERED_TEST_NOT_AUTHORIZED by this plan |

For each print: save printer/nozzle/material/profile, model and sliced revision SHA, dimensions, macro images, measured fit, defects and whether original servo shaft was undamaged. Spline tooth geometry must be physically checked, not only confirmed visible in preview.

### Materials

- **PLA:** often produces crisp small features but may deform from heat or crack under cyclic loading; do not presume durable torque transmission.
- **PETG:** may be tougher and more heat-tolerant in some conditions but stringing/bridging and dimensional fidelity of tiny teeth can be worse; print a matched coupon comparison.
- Avoid choosing filament based on generic toughness rankings alone. Verify actual purchased spool, drying needs, printer process and repetitive loading evidence. For any failed filament trial, report FAIL; do not call the alternate material automatically safe.

### Screw-hole and wing-root plan (unchanged)

- Candidate **two M1.6 bolts** per wing through H1 and H3; center-to-center **7mm OWNER-APPROVED**, hole CAD **Ø1.8mm proposed** for through clearance.
- Horn receives **three** collinear holes; middle H2 is **unused**. ARMV5 receives **two** mating holes at H1/H3.
- These lever-hole sizes and locations remain tentative; 1.5mm-thick tapered horn lever can leave too little material at outer hole after three perforations. Check root fillets, local reinforcement, screw-head seats, total gram weight and dynamic load path before production.
- Shaft **center retention screw** remains necessary according to actual SDS1601 design; not counted among the 2 arm bolts.
- No hard-coded screw length until measured stack and nut/washer/retainer clearance.

## 6. Pass/Fail & design change rules

- **HORN-P0_INPUT_CONFIRMED**: owner-confirmed 25T/4.9mm and Bambu A1 documented; nozzle/material/shaft profile still PENDING.
- **HORN-P1_CAD_CHECK**: parametric 25T female geometry and tooth profile documented; no hidden smooth bore assumption.
- **HORN-P2_SLICER_CHECK**: tooth details survive slicing with correct configured installed nozzle and selected material/profile.
- **HORN-P3_PHYSICAL_FIT**: actual power-off servo mating coupon fits securely without damage, center retaining screw usable.
- **HORN-P4_STATIC_ASSEMBLY**: both H1/H3 bolts fit and hold printed ARM root, H2 open, no cracks, no collision.
- **HORN-P5_CYCLIC_TEST**: powered testing ONLY after HW/SA/owner authorization; record tooth wear/backlash, screw loosening, temperature, cycle count, RPM/flap frequency and amplitude, load condition.
- **HORN-P6_WINGED_TEST**: further mechanical/electrical/battery control gates; never inferred from P2/P3.

**Stop conditions:** no recognizable tooth shape; tooth tips fuse; print enters with destructive interference; fit spins/slips without load; center retention screw cannot engage; cracking or loose bolts; servo overheats or rail sags; missing electrical/owner permission for powered test.

If the Bambu A1 cannot make a printed 25T spline that **passes** P3/P5 with the available process, mark **FULL_PRINTED_SPLINE_NOT_PROVEN / BLOCKED**, propose alternative techniques but **do not change to metal coupling** without explicit owner authorization.

## 7. Information to request next (one compact user question)

**Hole pattern decision CLOSED:** owner selected **3.5mm adjacent / 7mm H1-H3**, exactly three horn holes and two ARM holes. **Do not ask again** about P7 vs P12. Future manufacturing inputs: installed A1 nozzle and filament for coupon testing, actual tooth geometry and retained center screw dimensions.

**Current release state:** OWNER_GEOMETRY_CHOICE_CONFIRMED / SPLINE_25T_4.9_OWNER_CONFIRMED / A1_OWNER_CONFIRMED / NOZZLE_UNKNOWN / MATERIAL_UNKNOWN / NO_TASK / NO_PRINT_PASS.
