# 18 — Screenshot Review: Mezger Parametric Servo Arm vs SDS1601

**Status:** RESEARCH_VERIFIED / **P7_HOLE_PITCH_OWNER_APPROVED** — NOT A TASK. **Owner update 2026-11-24:** chose Option B/P7, **3.5mm between adjacent collinear holes**, **7.0mm between H1 and H3**, two installed bolts. Screenshot `arm_hole_distance=6mm` is historical source evidence, **not our target**.  
**Date:** 2026-11-24. **SA role:** compare the user's screenshot with the verified source generator and prior horn/ARMV5 draft. Do not edit/build/print production CAD or issue implementation Task yet.

## 1. Source identity and license

The uploaded screenshot shows **"customizable servo arm" by mezger** with a right-side customizer panel. It matches the openly published generator:

- Repository: https://github.com/mezger/customizable-servo-arm
- Source file: https://github.com/mezger/customizable-servo-arm/blob/main/customizable-servo-arm.scad
- Source file Git blob SHA inspected: `4aceab3836b3f193947a6fe633fabff40c70c74f`.
- README: https://github.com/mezger/customizable-servo-arm/blob/main/README.md
- License: **LGPL-2.1**, source/header includes derivative attribution. If our agent adapts this OpenSCAD code and redistributes the derivative, preserve attribution, copyright and license obligations. Do not import any source/asset into GitHub without a separately authorized task/permission review.
- Uploaded screenshot content SHA256: `1d5ef494498f905fc86e343f065c700e5c4eab0188c006f607948f633c228adc`. Screenshot is evidence of displayed UI settings, **not** a measured horn fit or a source CAD upload.

## 2. Settings visible in the screenshot (not all owner-approved values)

| Screenshot value | Observed | Interpretation / gate |
| --- | --- | --- |
| `servo_preset` | **FUTABA 3F Standard Spline 25T** | **DO NOT use as a SDS1601 25T/Ø4.9mm direct-mate preset**; tooth count alone is insufficient |
| `arm_count` | 1 | Matches owner-confirmed single-arm intent |
| `arm_thickness` | 1.5mm | Geometric input only; strength/fatigue not yet tested |
| `arm_hole_diameter` | 1.8mm | Candidate M1.6 normal-clearance through-hole; actual printed diameter must be calibrated |
| `arm_hole_distance` | 6mm | **Adjacent** hole-center pitch, not total distance between two extreme bolts; with 3 holes, outer-outer = **12mm** |
| `servo_head_gap` | 0.1mm | Candidate print compensation; not automatically safe/fit on the user's Bambu A1 |
| Envelope shown | 32.26 × 9.12 × 5mm, labeled "Original detail" | Consistent with reference 3F OpenSCAD parameter geometry, **NOT** an actual SDS1601 horn or assembled ARM fit |
| Nozzle / filament | Not shown | Still UNKNOWN; owner's printer is Bambu Lab A1 |

The screenshot's arm length numeric field is partially off-screen. The displayed **32.26mm** overall length corresponds to the generator's standard **`arm_length=25mm`** with 3F geometry; this is a source-based geometric inference, not a confirmed value manually entered by owner.

## 3. Verified internal 3F spline incompatibility

The source generator defines (literal arrays from its own OpenSCAD source):

```scad
FUTABA_3F_SPLINE = [[5.92, 4, 1.1, 2.5], [25, 0.3, 0.7, 0.1]];
SG90_SPLINE      = [[4.90, 3.2, 1.5, 2.5], [21, 0.25, 0.65, 0.25]];
```

These arrays encode `[[head_outer_diameter, head_height, wall, screw_hole_diameter], [teeth, tooth_height, tooth_length, tooth_width]]`. The **3F preset is 25T but Ø5.92mm**, while **SG90 preset is Ø4.90mm but 21T**.

**OWNER-CONFIRMED SDS1601: 25T Ø4.90mm nominal.** None of the two source presets independently matches **both** fields. Forcing either onto the physical servo risks shaft damage, slippage or structural failure. Even changing `5.92` to `4.90` with 25 teeth is only a **starting parametric geometry**: the generator's assumed tooth height/length/width, shaft major/minor dimensions, engagement depth and manufacturing fit must still be characterized on the actual SDS1601 and the Bambu A1. A diameter match alone cannot establish tooth-profile compatibility.

**Mandatory domain gate:** AI-3D must create a **new SDS1601-specific spline definition or measured CAD** in a future assigned task; then run Bambu Studio slice-preview and sacrificial unpowered fit coupons per Design/17. No direct production horn printing from FUTABA 3F selection.

## 4. Important hole-spacing and arm-length discrepancy

The source script's parametric hole placement is:

```scad
hole_count = (arm_length - (head_diameter/2 + head_thickness + head_heigth)) / arm_hole_distance;
for (j = [0 : hole_count])
    translate([arm_length - j*arm_hole_distance, 0, 0])
        cylinder(d=arm_hole_diameter, h=arm_thickness);
```

Given the screenshot's visible 3F geometry and inferred `arm_length=25mm`, the **three hole centers lie approximately 25, 19 and 13mm from the servo axis**; adjacent pitch = 6mm, extreme-hole pair pitch = **12mm**. The screenshot's "arm hole distance" describes **adjacent holes**, not the separation of two installed screws when the middle is unused.

The older SA design (Design/16) proposed **three holes at 5, 8.5, 12mm from servo axis**, with **7mm** between the two installed extreme bolts. **They are different interfaces.** Additionally, the earlier inner 5mm station must be rechecked against the printed hub radius, clearance for screw heads and the center-retaining screw; it is not automatically feasible on the 3F-based geometry.

For **three holes exactly** in the chosen source generator, `arm_length`, `head_diameter`, `head_height`, `head_thickness` and `arm_hole_distance` all affect count. **Changing pitch from 6 to 3.5mm while leaving arm_length 25mm will create more than three holes**, since the code fills all available stations. The safe parametric solution is to implement **an explicit `hole_count=3` and independent station coordinates / a geometry-driven bolt datum**, or adjust arm length and validated center distance together. Never rely on the screenshot appearance after changing only one field.

**Potential mechanical consequences of 12mm extreme-pair pitch / ~25mm servo-to-outer-hole reach:**
- May exceed available mounting pad on existing ARMV5; require actual CAD overlay and two-hole structure review.
- Longer horn increases displacement/lever arm and bending at the root; servo cyclic torque/load/clearance and center-of-mass changes require verification.
- The 1.5mm thick horn and three perforations require edge-ligament + print-layer orientation checks; use two bolts only, no third screw.
- Do not treat the generator's "hole" as a threaded insert; M1.6 Ø1.8mm is through-clearance, requires measured screw/nut stack.
- The center screw retaining the horn to the SDS1601 shaft remains a separate joint.

## 5. P7 selected by owner; P12 retained only as historical comparison

**Path P7 (OWNER-APPROVED Option B):** exactly **three collinear horn holes** with **3.5mm adjacent center pitch** and **7.0mm outer H1-H3 installed bolt pitch**. ARMV5 root receives exactly **two** corresponding holes. Build explicit three-hole geometry rather than using the original generator's auto hole count. Absolute inner-hole distance from hub axis is **not yet approved**; the previous 5mm illustration may be infeasible near hub/bolt heads.

**Path P12 (SCREENSHOT ONLY; NOT SELECTED):** 6mm adjacent / 12mm outer spacing. Retain as the meaning of the screenshot input, but **do not implement** unless the owner explicitly reverses the P7 decision.

**P7 is the only approved hole-pitch choice, but is not yet mechanically validated** against the printed ARMV5, hub/center screw, fastener heads or 25T/4.9mm spline. Verify fit and fatigue before production.

**SA implementation rule:** preserve owner-approved 3.5/3.5/7mm center pitch; determine an absolute hole offset `r` and required local reinforcement only after mechanical overlay. Any proposal to alter pitch must return to the owner as a new design decision.

## 6. Rules added for AI-3D

1. Start with Design/AGENTS.md, 09, 12, 14, 16, 17 and this 18; use screenshot only as configuration evidence.
2. **Never use FUTABA_3F_SPLINE directly** for the owner's 25T Ø4.9mm SDS1601; code's 3F is Ø5.92mm. SG90 array is Ø4.9mm but 21T. Define custom new spline using physical tooth profile and printer coupons.
3. Establish **one shared hole pattern contract** for horn + ARM: hole-center reference axis, three horn centers, which two arm holes are installed, measured pitch, screw diameter, head/nut clearances, tolerance and revision.
4. Freeze `hole_count=3` in derived parameter model instead of assuming the generator auto-count remains three after pitch/length changes.
5. Keep horn and ARM original STL and source SHA intact; do not produce derived STL/SCAD until Design + Task approval and source license compliance.
6. Use sacrificial print samples on the actual Bambu A1 (installed nozzle/filament still unknown) before winged motion. Show tooth micro-profile at every sliced Z level.
7. **Owner explicitly approved P7 on 2026-11-24**: 3.5mm adjacent / 7mm outer. **Do not re-ask**. Confirm absolute hole radial location, printable wall thickness, ARM root and bolt clearance separately.

## 7. Decision closure and remaining validation

**CLOSED:** Owner chose **Option B/P7**: 3 collinear horn holes, adjacent pitch **3.5mm**, outer installed bolt center distance **7mm**; 2 corresponding ARMV5 holes and only 2 bolts. P12 from the screenshot is not our target. **OPEN:** distance of H1 from spline axis, bolt/nut/washer clearance, exact spline fit, print/process tolerances, fatigue and mechanical root reinforcement.

**Current:** OWNER_P7_PITCH_APPROVED / 3F_PRESET_MISMATCH_CONFIRMED / ABSOLUTE_HOLE_POSITION_PENDING / NO_TASK / NO_STL_EDIT.
