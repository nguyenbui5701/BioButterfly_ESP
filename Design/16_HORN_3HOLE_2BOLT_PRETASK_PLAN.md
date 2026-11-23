# 16 — PRE-TASK Design Plan: SG90-Style 3-Hole Horn / 2-Bolt Wing Arm

**Status:** OWNER_APPROVED_P7_HOLE_PATTERN / MECHANICAL_DIMENSIONS_AND_PRINT_VALIDATION_PENDING. **Decision recorded 2026-11-24:** owner explicitly chose **Option B/P7**: exactly three collinear horn holes with **3.5mm adjacent center pitch**, **7.0mm outer H1-H3 bolt pitch**; ARMV5 has two matching holes at H1/H3 and uses only two bolts. **P12 (6mm adjacent, 12mm outer) is not selected**; screenshot remains reference evidence only. Fully printed direct SDS1601 horn, nominal 25T/Ø4.9mm and Bambu Lab A1 remain owner-confirmed. See [18_OPENSCAD_CUSTOMIZER_25T_REVIEW.md](18_OPENSCAD_CUSTOMIZER_25T_REVIEW.md) and [17_BAMBU_A1_25T_SPLINE_PRINT_VALIDATION.md](17_BAMBU_A1_25T_SPLINE_PRINT_VALIDATION.md). This decision does NOT approve the absolute radial location, hole diameter, print tolerance or powered operation.  **Date:** 2026-11-23 | **Author:** SA | **Agent consultation required:** AI-3D + AI-HW.  
**This is a design discussion, NOT a Task or permission to print/energize the assembly.**

## 1. Owner-confirmed design intent

- Use the **new uploaded STL** `SG90_one_horn_without_holes.stl` as the geometric reference for a horn to be 3D printed/modified.
- Add **three available mounting holes on the straight horn lever**, but fasten the wing arm with **only two screws**. The wing arm (source `ARMV5(1).STL`) gets **two matching holes**, not three.
- Choose fastener size and attachment spacing appropriate for lightweight, high-cycle bionic butterfly wings. The servo is **SDS1601**, NOT a confirmed SG90. Existing ARMV5 and FC Mount V5 originals must be preserved.
- **Separate the servo central retention screw** from the two arm-to-horn screws. A total of two connection bolts **per wing** plus the shaft-center retention screw (if required by compatible horn design) is intended.
- **No metal horn, 25T insert or adapter is to be silently substituted.** Owner chooses an entirely 3D-printed horn with its **own internal mating spline** and bolt-lever. An alternative coupler would require a fresh owner design change.
- **Owner-approved pattern P7:** three hole centers in one straight line along the horn lever, **3.5mm between adjacent holes** (H1-H2 and H2-H3), **7mm between H1 and H3**. Mount with **two screws only**, through H1 (inner) and H3 (outer); H2 is left empty. ARMV5 wing root gets exactly two matching holes at 7mm pitch.
- No Task until remaining design prerequisites and owner sign-off. Source STL is NOT stored in GitHub; other AI sessions require byte-identical attachment, not just filename.

## 2. Exact inspected source geometry

| Attribute | Extracted from provided STL | Confidence |
| --- | --- | --- |
| Filename | SG90_one_horn_without_holes.stl | FILE_INSPECTED |
| SHA-256 | `6b54afba5a9ba01da71d5fba9db7474e690e79ad3da17c99f72833280b02d161` | FILE_INSPECTED |
| Shell | Single watertight triangular mesh; 3,908 triangles | FILE_INSPECTED |
| Overall envelope | X ≈ -3.498..3.500mm, Y ≈ -16.150..3.499mm, Z = -4..0mm | Derived assuming STL scale is mm |
| Round central hub | Approx. 7.0mm outer diameter, 4.0mm thick | FILE_INSPECTED |
| Tapered lever | Approx. 1.5mm thick, narrows from about 4.8mm near Y=-5mm to 3.9mm at Y=-12mm | FILE_INSPECTED |
| Central passage | Circular stepped bore; sections near Z=-3.5 or -1.5 show ~4.7mm diameter and narrower section near Z=-3 shows ~2.5mm. **No resolved internal 25T spline in this mesh.** | FILE_INSPECTED, not functional fitting |
| SDS1601 output | **25T/Ø4.90mm** | OWNER_CONFIRMED nominal spline specification (matches reference sheet); exact tooth profile, height/fit still require physical measurement |
| ARMV5 | Two distinct watertight components (likely left/right print arrangement), with owner-reported prior carbon and horn fit issues | FILE_INSPECTED + OWNER_REPORTED |

**Warning:** STL geometry and shape do not prove shaft fit, transfer of servo torque or safe 3D print strength. SG90 in a filename is not SDS1601 shaft compatibility.

## 3. Joint architecture: mandatory distinction

1. **Servo spline -> horn hub:** tooth count, diameter, tooth engagement depth and retaining center screw. The SG90-style STL currently shows a stepped round passage, not a verified SDS1601 25T spline. Never press a 25T shaft into a round hole or use radial interference alone as primary cyclic torque coupling.
2. **Horn lever -> printed ARMV5 wing root:** three holes **provisioned** along the lever centerline; **only two holes used** with metal fasteners through matching holes in the wing arm.
3. **Wing root -> four carbon rods:** separate fit issue; do not silently change rod sockets as part of a horn-only dimension revision unless authorized.
4. **Servo -> PCB power:** no new movement or safety clearance assumptions from CAD alone.

### SELECTED — fully 3D-printed horn, direct SDS1601 coupling

**OWNER-CONFIRMED DESIGN CHOICE:** horn including hub, mating spline and lever must be made as a fully printed part. No metal horn or insert in the load path unless owner later changes direction. The supplied SG90-style STL is the starting geometry/reference, not a guaranteed fitted SDS1601 horn: its bore is circular/stepped and lacks the exact SDS1601 tooth profile. **A new parametric internal spline must be derived from measured purchased SDS1601 shaft and appropriate datasheet geometry.** Simply boring the SG90 part to nominal Ø4.9mm is not acceptable.

**Structural risk:** a printed micro-spline can shear, round off or creep under repeated high-cycle flapping even if it statically engages. At nominal 25T and Ø4.90mm, tooth pitch around the outer circumference is approximately 0.62mm (order-of-magnitude only); whether the chosen printer, nozzle, layer orientation and material can reproduce the engagement depends on real process capability. Any printed spline that is too coarse, fragile or slips **FAILS the mechanical gate**; do not hide the failure with excessive interference, glue, heating or an unapproved metal alternative.

**New owner manufacturing input:** Bambu Lab A1 FDM. Official guide lists stock 0.4mm and optional 0.2mm nozzle; current installed nozzle and filament are UNKNOWN. For 25T at Ø4.9mm, outer-circumference tooth pitch ≈0.616mm (NOT tooth thickness). Prefer evaluating 0.2mm nozzle for spline micro-detail if available, but do not assert that printer can produce a load-bearing fit until the coupon passes. Full matrix in Design/17.

**Minimum development sequence (before winged use):**
1. Record the owner-confirmed **25T/Ø4.9mm nominal spline**, then physically inspect actual purchased servo tooth *form*, minor diameter, engagement depth/height, shaft-center retention screw and horn-installation direction. Never substitute nominal OD for tooth profile. Separate manufacturer-advertised dimensions from actual measurements.
2. Select printer/process/material and print **small spline-fit test coupons**, not full horns first. Compare geometry with caliper/microscope where possible, inspect tooth resolution, orientation, hole shrink and internal tooth damage.
3. Non-powered engagement test on the real servo with no wing load: fully seats with no forcing, no permanent damage to shaft teeth, acceptable backlash, secure separate center retention screw, removable for service; record tests/failed samples.
4. Only after spline static engagement passes, combine hub and 3-hole lever in CAD, analyze root fillets, fatigue, layer orientation, stress concentrations and mass; print a single horn+ARM connection specimen.
5. Subject prototype to restrained, instrumented no-wing and then low-load cycling **only after existing electrical, mechanical and owner powered-test gates**. Any slip, rounding, crack, heat/creep or excessive looseness is STOP/FAIL; redesign or revisit coupling with explicit owner authorization.

**Manufacturing reality check:** standard FDM with a large nozzle may not resolve a 25T micro-spline sufficiently. If printed samples cannot meet the fit/fatigue gates, report `FULL_PRINTED_SPLINE_NOT_VIABLE_WITH_AVAILABLE_PROCESS` rather than treating the part as approved. A resin/higher-resolution process may improve dimensional fidelity but **does not automatically prove fatigue strength**.

**Confirmed hole topology:** 3 through-holes **collinear** on lever centerline; ARMV5 root has 2 matching holes; install 2 bolts at selected outer positions H1 and H3, leave H2 unused. The third hole weakens the lever locally: inspect the net ligament and consider localized width increase rather than moving to oversized bolts. All dimensions below remain **proposed** until fabrication testing.

## 4. Fastener size assessment (proposed, no load qualification)

| Option | Typical clearance bore | Lever compatibility | SA disposition |
| --- | --- | --- | --- |
| **M1.4** | ~1.6mm (confirm actual screw standard) | More plastic remains, smaller head; delicate handling/sourcing | Backup if measured load/assembly allows |
| **M1.6** | **Ø1.8mm normal clearance** per ISO 273 reference | Good preliminary balance of net ligament, screw availability and manageable assembly | **Preferred for initial test design** |
| **M2** | **Ø2.2mm normal clearance** per ISO 273 reference | Margins thin in original narrow lever, especially toward tip; may require locally widening/reinforcing lever and arm root | Not preferred on unchanged lever |

**Critical:** M1.6 is a **recommended starting candidate**, not a proven fatigue-strength rating. Use **two M1.6 machine bolts per wing** (four total for two wings), plus compatible retention screw at each servo shaft as applicable. Preferred connection uses **through-bolts with captured nuts/verified metal threads**, not arbitrary plastic self-tapping in a 1.5mm-thick horn leaf. Final screw length depends on **measured combined thickness** of horn, wing arm, washer/nut and allowable protrusion; `M1.6 x 5mm` or `M1.6 x 6mm` is a **shopping/test candidate only**, not an approved cut list. Check locking method, head height and whether a screw/washer sweeps into servo case.

**Preliminary design calculation from STL lever widths** (hole centered X=0; value is approximate minimum side material *at hole center*, excluding printed variance/edge fillet and cyclic notch factors):

| Hole station Y, from hub center | Lever width at station | Remaining each side for Ø1.8 | Remaining each side for Ø2.2 |
| --- | --- | --- | --- |
| -5.0mm | ~4.84mm | ~1.52mm | ~1.32mm |
| -8.5mm | ~4.35mm | ~1.27mm | ~1.07mm |
| -12.0mm | ~3.86mm | ~1.03mm | ~0.83mm |

M2 would leave less than 1mm nominal at the outermost hole before print tolerance and fatigue. Even M1.6 leaves only ~1mm at the outermost station; **local widening, radiused reinforcement or a shorter usable pair may be required** after structural review. Three holes have a strength penalty regardless of only using two bolts.

## 5. Owner-approved P7 hole pattern; absolute location still to validate

**Decision:** The owner explicitly selected **Option B/P7** over screenshot P12. Treat this as the **one canonical horn-to-ARM mounting hole pattern**:

| Parameter | Requirement | Decision state |
| --- | --- | --- |
| Number of horn lever holes | **Exactly 3**, H1/H2/H3, collinear on lever centerline | OWNER_APPROVED |
| Adjacent center pitch | **H1-H2 = 3.5mm**, **H2-H3 = 3.5mm** | OWNER_APPROVED |
| Outer fastener center pitch | **H1-H3 = 7.0mm** | OWNER_APPROVED |
| Which holes fastened | **H1 and H3**; H2 stays empty | OWNER_APPROVED |
| ARMV5 matching holes | **Exactly 2**, aligned with horn H1/H3 at **7.0mm** pitch | OWNER_APPROVED |
| Bolt size | 2x M1.6 machine screws per wing (candidate) | PROPOSED — not yet strength/print-validated |
| Through clearance bore | Nominal CAD Ø1.8mm for M1.6 (candidate) | PROPOSED — requires actual printed-fit coupon |
| Shaft-retaining center screw | Separate from two arm bolts, preserve real SDS1601 retainer | REQUIRED FUNCTION; actual size/mechanical details pending |

**Shared parametric coordinate frame:** set servo output axis at `O=(0,0)`, horn lever along negative Y. Let `r` be the **undetermined** distance from servo output axis to the inner H1 center; then the three horn-hole centers are:

```text
H1 = (0, -r)
H2 = (0, -(r + 3.5 mm))
H3 = (0, -(r + 7.0 mm))
ARMV5 holes = H1 and H3 only
```

**Do not lock `r` yet.** The previous illustration used `r=5.0mm` (hole radii 5.0/8.5/12.0mm), but this is **NOT an approved mechanical coordinate**. It may interfere with the printed horn hub, its shaft-center retention screw, bolt heads/nuts, ARM root or structural wall. AI-3D must select and justify `r`, the lever envelope and head/washer clearance after overlaying the real SDS1601 spline/hub and ARMV5 root. Keep the 3.5/3.5/7.0mm spacing fixed unless owner explicitly changes the product decision.

**Source customizer warning:** the screenshot's `arm_hole_distance=6mm` would generate **12mm** outer-screw spacing; it is **superseded as a target**. The upstream OpenSCAD auto-calculates the number of holes from length and pitch. **Do not simply change 6 to 3.5 while retaining the old 25mm arm length**, because it may generate more than three holes. Author an explicit exactly-three-hole station array/loop, independent of overall arm length, with one shared parameter or datum for the two ARM matching holes.

A P7 pattern alone does not prove strength. Three 1.8mm-class perforations on a 1.5mm-thick tapered arm can leave too little plastic; check thin-wall ligaments, bolt-head/nut clearance, localized root reinforcement, stiffness, tool access, print anisotropy, mass and no-contact servo rotation. No production STL until coupon/fit and design gates pass.

## 6. Why two bolts and not three?

- Two spaced bolts help the printed wing-root resist **twist around the arm-to-horn joint** and slip under repeated reversals.
- The third pilot hole can be used to move a fastener if a validated mounting geometry requires it, but leaving it unused does not make it harmless — every unused through-hole creates a possible stress concentration.
- Three installed bolts add mass and potentially weak-zone perforations; user specified two. The design must prove two sufficiently secure, with safe fastener engagement and no relative slip under load.
- The center shaft retention fastener is a **separate mechanical function**; two lever bolts do not replace it.

## 7. Assembly and printing proposal (before producing files)

1. **Identify physical servo spline:** exact SDS1601 batch, shaft tooth count/diameter/height, retention screw and photo; do not rely on SG90 STL name.
2. **Use the confirmed all-plastic direct-hub approach**. Construct a measured SDS1601-specific mating spline in new parametric CAD, and print isolated engagement coupons first. A metal coupler is not part of this approved concept.
3. Confirm original ARMV5 arm-side root material, alignment plane, 2-bolt pad width/thickness and full left/right actuation sweep.
4. Parameterized CAD revision: hub/spline, three hole centers H1/H2/H3, clearance diameter, head/washer/nut pockets, fillets and local reinforcement, exact screw stack and material; matching **two** holes in wing arm, mirrored only after verifying handedness.
5. **Print test coupons first**: hole size variants and, for Option A, spline engagement coupons. Record printer/nozzle/material, axes, dimensional errors, number of failed prints and actual measured fit.
6. Print a **single unloaded horn+arm sample**, install two bolts without wing rods, manually inspect slip, stress whitening, cracking and fastener clearance. Preserve the center retainer.
7. Only after actual static-fit PASS and prior HW gates, authorize carefully controlled no-wing power test and then winged bench tests. Do not test in flight to establish basic spline compatibility.
8. Preserve original STL bytes and revision names. Any derived file must retain correct source provenance/permission and editable CAD, not only exported STL.

## 8. Mechanical acceptance criteria (to be validated with AI-3D)

- **HORN-G0**: compatible actual servo spline and central retainer fully identified (BLOCKED now).
- **HORN-G1**: concept choice VERIFIED BY OWNER = printed direct spline, straight-line H1/H2/H3; **dimensional fit and process feasibility still BLOCKED** until test coupons and full plan are approved.
- **HORN-G2**: three through mounting holes in horn, centerline true to datum; corresponding TWO arm holes match selected pair without forced screw insertion.
- **HORN-G3**: printed fit results and no cracks/delamination; all fasteners have adequate thread engagement and secure retention method.
- **HORN-G4**: visual/manual static sweep and tool/cable/switch clearance with no collisions; mass increase documented.
- **HORN-G5**: repeated cycling only after AI-HW/SA-approved power/servo gate, with displacement/slip measurements and no detectable looseness at specified test load/duration.
- **HORN-G6**: later tether/flight readiness must satisfy existing HW/MECH/SA gates independently.

HORN-G0..G6 are **NOT_RUN/BLOCKED** at time of design draft. Replacing plastic with metal or changing screw size requires a revised review/approval.

## 9. Remaining INPUT questions (not repeat owner-confirmed choices)

1. **Printer capability:** Bambu Lab **A1 FDM is confirmed**; only installed nozzle diameter (**0.4mm vs 0.2mm**) and actual filament (**PLA/PETG/other**) remain unknown. These determine coupon strategy; see Design/17.
2. **Physical servo and fasteners:** close-up of SDS1601 shaft markings/spline and original retention screw, optionally caliper readings of shaft outer diameter and axial engagement depth. No fabricated spline dimensions.
3. **Stackup:** original ARMV5 root thickness, printer profile, availability of M1.6 machine bolts/nuts/washers and usable lever head clearance. Final bolt length unknown until measured.
4. **Verification method:** static no-wing test and later controlled fatigue cycling with builder present; no powered test without PCB/Battery gates.

Owner's core design choices (fully printed direct horn; **P7 3.5mm adjacent / 7mm outer**, three collinear holes) are **CLOSED**. Do not ask them again unless the owner changes scope.

Do not create Task files until other prerequisites are reviewed and SA records approved scope. **Current: OWNER_APPROVED_P7_3.5MM_7MM / ABSOLUTE_HOLE_OFFSET_AND_FASTENER_NOT_APPROVED / NOZZLE_AND_FILAMENT_UNKNOWN / HORN_G0_BLOCKED / NO_TASK / NO_NEW_CAD_CREATED.**
