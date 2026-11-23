# 16 — PRE-TASK Design Plan: SG90-Style 3-Hole Horn / 2-Bolt Wing Arm

**Status:** DESIGN_PROPOSAL — Owner mechanical approval and measurement pending.  
**Date:** 2026-11-23 | **Author:** SA | **Agent consultation required:** AI-3D + AI-HW.  
**This is a design discussion, NOT a Task or permission to print/energize the assembly.**

## 1. Owner request and locked intent

- Use the **new uploaded STL** `SG90_one_horn_without_holes.stl` as the geometric reference for a horn to be 3D printed/modified.
- Add **three available mounting holes on the straight horn lever**, but fasten the wing arm with **only two screws**. The wing arm (source `ARMV5(1).STL`) gets **two matching holes**, not three.
- Choose fastener size and attachment spacing appropriate for lightweight, high-cycle bionic butterfly wings. The servo is **SDS1601**, NOT a confirmed SG90. Existing ARMV5 and FC Mount V5 originals must be preserved.
- **Separate the servo central retention screw** from the two arm-to-horn screws. A total of two connection bolts **per wing** plus the shaft-center retention screw (if required by compatible horn design) is intended.
- No Task until rules/design scope is agreed. The source STL uploaded in this conversation is NOT stored in GitHub; other AI sessions require the actual byte-identical attachment, not just the basename.

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
| Reference SDS1601 output | Reference sheet says **25T/Ø4.90mm** | REFERENCE_DATASHEET; actual servo to verify |
| ARMV5 | Two distinct watertight components (likely left/right print arrangement), with owner-reported prior carbon and horn fit issues | FILE_INSPECTED + OWNER_REPORTED |

**Warning:** STL geometry and shape do not prove shaft fit, transfer of servo torque or safe 3D print strength. SG90 in a filename is not SDS1601 shaft compatibility.

## 3. Joint architecture: mandatory distinction

1. **Servo spline -> horn hub:** tooth count, diameter, tooth engagement depth and retaining center screw. The SG90-style STL currently shows a stepped round passage, not a verified SDS1601 25T spline. Never press a 25T shaft into a round hole or use radial interference alone as primary cyclic torque coupling.
2. **Horn lever -> printed ARMV5 wing root:** three holes **provisioned** along the lever centerline; **only two holes used** with metal fasteners through matching holes in the wing arm.
3. **Wing root -> four carbon rods:** separate fit issue; do not silently change rod sockets as part of a horn-only dimension revision unless authorized.
4. **Servo -> PCB power:** no new movement or safety clearance assumptions from CAD alone.

### Mechanical alternatives awaiting owner decision

- **Option A — fully 3D-printed servo horn:** Create a **correctly measured SDS1601-specific 25T spline** plus center retainer, three bolt holes on printed lever. Use coupon and physical spline engagement test on actual printer. High risk: fine teeth, backlash, creep and fatigue under flapping; no direct wing drive until independently validated.
- **Option B — recommended for winged prototype:** Keep a **verified 25T metal horn as torque interface**, print a lightweight three-hole outer wing adapter/lever or custom arm, and fasten the wing arm with two screws. It avoids 3D-printing the critical 25T spline. The supplied SG90-style STL can inform lever geometry but the final metal-horn bolt pattern must be physically measured. Not allowed to assume 21T metal horns fit SDS1601.
- The best solution for full printed-coupler experiments may require resin/high-resolution process and strength tests. **Do not claim FDM can accurately reproduce or retain 25 tiny spline teeth from this STL**.

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

## 5. Three-hole / two-bolt pattern proposal

Coordinate convention for the supplied STL: central hub rotation axis **O = (X=0, Y=0)**, lever extends toward **negative Y**, through-holes drilled along **Z**. All proposed positions must be validated on physically measured, printable geometry.

| Hole | Proposed center (X, Y) mm | Role |
| --- | --- | --- |
| H1 (inner) | (0, -5.0) | **Bolt A** |
| H2 (middle) | (0, -8.5) | Optional alternative; normally **unused** |
| H3 (outer) | (0, -12.0) | **Bolt B** |

Center-to-center spacing: H1-H2 = **3.5mm**, H2-H3 = **3.5mm**, H1-H3 = **7.0mm**. Use **H1 + H3** on wing arm, with two matching holes spaced **7.0mm**. H2 remains an adjustment/reconfiguration candidate and does not carry a bolt in the initial assembly.

**All coordinates are a DRAFT CONCEPT, not manufacturing dimensions.** They are selected from the supplied mesh envelope to make the layout tangible. The inner hole must clear the hub and central retainer, while outer hole still has sufficient edge and tip margins after screw-head and required arm shape. If head/washer clearance or root strength fails, shift/reinforce geometry and report changes for SA approval.

Screw holes: CAD nominal **1.8mm for M1.6 through clearance** in both horn and wing root, **subject to printed calibration**. Do not assume the as-printed hole is 1.8mm. Consider a thin test coupon and controlled reaming, and measure center-to-center pitch tolerance on both parts.

## 6. Why two bolts and not three?

- Two spaced bolts help the printed wing-root resist **twist around the arm-to-horn joint** and slip under repeated reversals.
- The third pilot hole can be used to move a fastener if a validated mounting geometry requires it, but leaving it unused does not make it harmless — every unused through-hole creates a possible stress concentration.
- Three installed bolts add mass and potentially weak-zone perforations; user specified two. The design must prove two sufficiently secure, with safe fastener engagement and no relative slip under load.
- The center shaft retention fastener is a **separate mechanical function**; two lever bolts do not replace it.

## 7. Assembly and printing proposal (before producing files)

1. **Identify physical servo spline:** exact SDS1601 batch, shaft tooth count/diameter/height, retention screw and photo; do not rely on SG90 STL name.
2. **Choose A or B coupling** with owner. For B obtain the real metal horn model/hole geometry. For A model exact compatible spline and run tiny shaft-fit coupons first.
3. Confirm original ARMV5 arm-side root material, alignment plane, 2-bolt pad width/thickness and full left/right actuation sweep.
4. Parameterized CAD revision: hub/spline, three hole centers H1/H2/H3, clearance diameter, head/washer/nut pockets, fillets and local reinforcement, exact screw stack and material; matching **two** holes in wing arm, mirrored only after verifying handedness.
5. **Print test coupons first**: hole size variants and, for Option A, spline engagement coupons. Record printer/nozzle/material, axes, dimensional errors, number of failed prints and actual measured fit.
6. Print a **single unloaded horn+arm sample**, install two bolts without wing rods, manually inspect slip, stress whitening, cracking and fastener clearance. Preserve the center retainer.
7. Only after actual static-fit PASS and prior HW gates, authorize carefully controlled no-wing power test and then winged bench tests. Do not test in flight to establish basic spline compatibility.
8. Preserve original STL bytes and revision names. Any derived file must retain correct source provenance/permission and editable CAD, not only exported STL.

## 8. Mechanical acceptance criteria (to be validated with AI-3D)

- **HORN-G0**: compatible actual servo spline and central retainer fully identified (BLOCKED now).
- **HORN-G1**: horn interface option A/B selected, compatible with exact physical servo and full test plan.
- **HORN-G2**: three through mounting holes in horn, centerline true to datum; corresponding TWO arm holes match selected pair without forced screw insertion.
- **HORN-G3**: printed fit results and no cracks/delamination; all fasteners have adequate thread engagement and secure retention method.
- **HORN-G4**: visual/manual static sweep and tool/cable/switch clearance with no collisions; mass increase documented.
- **HORN-G5**: repeated cycling only after AI-HW/SA-approved power/servo gate, with displacement/slip measurements and no detectable looseness at specified test load/duration.
- **HORN-G6**: later tether/flight readiness must satisfy existing HW/MECH/SA gates independently.

HORN-G0..G6 are **NOT_RUN/BLOCKED** at time of design draft. Replacing plastic with metal or changing screw size requires a revised review/approval.

## 9. Specific unresolved questions for owner

1. **Most important:** Do you intend to put the newly 3D-printed horn **directly on the SDS1601 output shaft** (Option A), or use an **actual 25T metal horn on the servo and attach the printed wing arm/adapter by two bolts** (Option B, recommended)? The existing uploaded STL hub has no verified 25T interface.
2. Is the three-hole pattern meant to be a **line along the narrow lever**, using the inner and outer holes, with the middle as spare? If instead three holes form a triangle around the hub, the geometry and bolt recommendation changes.
3. Which physical printer/process/material is available (FDM nozzle size PLA/PETG, or resin)? Can you provide a photo of actual SDS1601 spline and any metal horn/retention screw?
4. Do you already have M1.6/M2 machine bolts/nuts, and can you measure wing-arm root thickness and intended stack? No final bolt length without this.

Do not create Task files until owner has reviewed the design choices and SA records accepted scope. **Current: HORN_PRETASK_PLAN_READY / HORN_G0_BLOCKED / NO_NEW_CAD_CREATED.**
