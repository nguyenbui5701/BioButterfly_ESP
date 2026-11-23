# 14 — ARMV5 Rework Rules: Metal Horn, Two Fasteners and 1.5mm Rods

**Status:** RULES_DRAFT / 2026-11-24. **Owner-confirmed 25T/Ø4.9mm SDS1601 spline and Bambu Lab A1 printer; additional mandatory [17_BAMBU_A1_25T_SPLINE_PRINT_VALIDATION.md](17_BAMBU_A1_25T_SPLINE_PRINT_VALIDATION.md).** **Addendum:** see [16_HORN_3HOLE_2BOLT_PRETASK_PLAN.md](16_HORN_3HOLE_2BOLT_PRETASK_PLAN.md) for newly uploaded SG90-style horn source, proposed H1/H2/H3 layout and M1.6 sizing. **Role:** AI-3D, with AI-HW consultation. **Boundary:** define mechanical acceptance and controlled design iteration; **do not export modified production STL/STEP until owner-approved Design and Task**.

## 1. What has actually been observed

- Owner has printed **ARMV5(1).STL**, reports a one-hole/undersized arm-horn interface and only 1 of 4 sockets accepting nominal 1.5mm carbon rod.
- **FC Mount V5(1).STL** exists but has NOT been printed.
- Mesh inspection: ARMV5 file has **two disconnected watertight components** with unequal projected bounding boxes due to orientation, likely left/right components. Whole-object bbox ~79.68 × 11.50 × 49.32mm; this is **not the footprint of one assembled arm**.
- **Superseded concept:** a 21T metal horn / ~4.5mm was previously mentioned; owner has now explicitly chosen a **fully printed plastic horn directly mating SDS1601**, three collinear lever holes, two arm holes and two installed screws. The earlier metal-horn idea is **not** the current design; retain as historical risk context only.
- Reference SDS1601 datasheet (project Drive/SDS1601 Servo Specs.jpg) lists output **25T / Ø4.90mm**; Owner has now confirmed its **nominal 25T/Ø4.9mm** specification; actual tooth form, tolerance and engagement depth must still be checked.
- Photo is a visual example only; it cannot determine exact bolt spacing, thread, tooth count, horn cross section, attachment depth or torque loads.

## Newly specified three-hole horn / two-hole arm concept

Owner **CONFIRMED** **three collinear mounting holes on the fully plastic 3D-printed horn**, with only **two bolts** (outer holes H1+H3) installed per wing, and **two matching holes in ARMV5 wing root**. The SG90-style new horn's 4mm hub and 1.5mm-thick tapering lever have been measured from STL; its central passage is stepped round, **not verified SDS1601 25T**. Do not confuse the servo center retention screw with these wing-arm connection bolts. Refer to Design/16 for provisional positions, 1.8mm M1.6 clearance, **the selected all-printed internal spline architecture** and critical test gates.

## 2. DO NOT DESIGN THE WRONG JOINT

Separate:
1. **Spline coupling:** the SDS1601 output spline to **newly designed printed internal mating teeth**. Source SG90-style STL has a round stepped bore, not a verified 25T spline. Measure actual shaft tooth count, major/minor diameters and effective engagement before modeling; no force fit, heat-fit, or glue to bypass a failed print.
2. **Center retention screw:** horn retained to servo shaft as designed by servo maker; not eliminated by two outer fasteners.
3. **Arm-to-horn connection:** two fasteners holding printed wing arm to the **fully printed horn lever** for bending/torsion resistance. Use outer collinear positions H1+H3 and corresponding two holes on ARM; third middle horn hole is unused. Locate exact coordinates from final approved print/screw design.
4. **Rod sockets:** four intended carbon rod sockets whose diameter, direction, insertion depth, strain and adhesive method are independent from horn/spline geometry.

Before geometry changes, physically measure servo spline and center retainer (1)(2), assess printed micro-tooth resolution and repeat-cycle fatigue, then verify H1/H3 hole pitch, screw dimensions and clearance on joint (3) and all four socket IDs/axes (4). The selected direct plastic coupling is owner confirmed, but fit remains unproven.

## 3. Required measurement sheet, units = mm

| Field | Method | Acceptance to start CAD |
| --- | --- | --- |
| Exact servo product marking and spline teeth | High-resolution photo + datasheet/bare spline examination; test-fit known horn non-destructively | Exact spline compatibility recorded as MEASURED or VERIFIED |
| Spline outside diameter, engagement height | Caliper/microscope where practical; do not damage teeth | Dimensional drawing with uncertainty |
| Fully printed horn internal spline and retention screw | Physical SDS1601 spline characterization + parametric CAD + printer process verification | Engagement coupon, no shaft damage/slip and center retainer verified |
| Two arm mounting bolt centers and drill diameters | Measure center-to-center, hole diameter, thread/pitch and head clearance | Pattern defined including tolerance |
| Servo-to-arm distance and rotational plane | Actual dry assembly and coordinates | No mechanical collision at neutral and swept extremes |
| Carbon rod actual diameter (multiple readings) | Caliper at several points, note ovality | Dimension and variability per rod batch |
| Per-socket present result (1..4) | Photograph each hole and test the same rod | Failure map including location and insertion direction |
| Printed hole dimensions per axis | Gauges/calipers, reference test coupon; inspect burrs/elephant-foot | Minimum usable fit range based on real printer |
| Print material, nozzle, layer, orientation | Slicer/project and actual printer settings | Reproducible profile |
| Printed mass, stiffness/cracking | Scale + manual non-powered checks | Document if measured; never claim from STL volume |

**No arbitrary 21T/25T substitution** and no blind enlargement of every hole to the same size.

## 4. Carbon-rod socket fit methodology

- Distinguish **nominal rod stock (1.5mm)** from actual measured rod diameters and **CAD bore diameter** from as-printed bore and as-assembled retention.
- Record socket centerline, depth, entry chamfer, wall thickness, termination and crossing with adjacent rod holes; verify all intended directions.
- Prepare a **small parameterized test coupon** matching actual nozzle, layer orientation and representative wall geometry. Candidate CAD nominal bores: **1.50 / 1.60 / 1.70 / 1.80mm**; these are test variants only, NOT pre-approved final sizes.
- Test with the owner’s real rods in each axis/orientation. Document: fits by hand, requires controlled press, excessive slop, cracking, insufficient insertion, glue allowance; photograph results. Choose fit appropriate to actual retention method and wall strength.
- Do NOT drill all sockets without checking thin walls, neighboring socket intersection or load path; if post-drilling is chosen, specify safe jig/depth and inspect for delamination.
- Use rounded lead-ins, continuous wall sections and root reinforcement only where mass and fatigue trade-offs permit. Do not add heavy bosses blindly; compare gram cost and CG.

## 5. Dual-bolt horn joint methodology

1. Build parametric **printed internal spline** from confirmed SDS1601 shaft geometry; check print resolution, engagement depth and retention screw; coupon-test compatible spline first.
2. Create parametric geometry around a defined joint datum and rotation axis; provide three **collinear** horn mounting coordinates H1/H2/H3 with only H1+H3 bolted to the two-hole ARM root, and bolt-axis normals.
3. Evaluate **two bolts plus center retaining screw** for tool accessibility, bolt head clearance, total thickness and torquing.
4. Ensure both fasteners engage meaningful material and metal; avoid slots too close to root edge/rod sockets. Check wall thickness, layer anisotropy, crush/creep from bolts, washer/backing requirement and screw retention.
5. Consider at least **two joint design variants** only if trade-offs are unresolved: screw through printed arm into tapped horn vs captured nuts/insert system. Do not invent thread sizes.
6. Record projected mass difference, torsional alignment, backlash, service/replacement procedure and no-contact clearance through permitted servo travel.
7. Freeze geometry only against verified **servo spline measurements, printer/material process and coupon outcomes**, not a guessed 25T bore or old 21T horn SKU.

## 6. FC Mount V5 review — do not print speculatively

- Compare **actual PCB shape and measured dimensions** against FC Mount import at a common datum, including board thickness, connector/pin height, SMD component highest point, soldered part access, USB insertion, antenna keep-out, battery cable and mounting retention.
- PCB Gerber outline bbox ~18.5 × 45mm vs FC Mount file bbox 32 × 39.2 × 38mm **cannot prove fit**: axis assignment/orientation and local clearance are unspecified.
- Do not order/suggest print of FC Mount until dimensions are checked with AI-HW and local 3D geometry alignment; use a small fit gauge/prototype when uncertain.
- Preserve power switch, battery removal and balance plug access; no strain on solder joints or RF antenna enclosure.

## 7. Required output gates and acceptance

- **MECH-G0 INPUT_MATCH:** exact source SHA + provenance, servo/horn/rod/printer real measurements available; compatibility signed.
- **MECH-G1 CAD_REVIEW:** native editable model, parametric mating dimensions, left/right orientation, min wall thickness and swept collision checks; no unapproved changes outside arm/mount.
- **MECH-G2 PRINT_COUPON:** fit coupon printed with actual material/nozzle/profile; measured hole result table for each socket direction.
- **MECH-G3 ARM_TEST:** fully printed 25T-compatible horn (if actual shaft verifies 25T) mates without forcing or damaging the servo; both bolts fasten horn to matching ARM holes; separate center retention remains; all four rods fit; no slip, cracks or collision.
- **MECH-G4 FC_MOUNT_FIT:** board/module mock-up fit, connector/service/antenna clearance and mass constraints verified before mounting powered assembly.
- **MECH-G5 INTEGRATED_STATIC:** assembly inspected after wiring (NO POWER); servo rotation requires separately approved bench safety gate.

**Reports:** per gate PASS/FAIL/BLOCKED/NOT_RUN; signed source SHA, test conditions, photos/measurements. An STL that loads is only MESH_LOAD_PASS, never MECH-G3.

## 8. Revision and rollback

- Keep original ARMV5 and FC Mount V5 attachments immutable; never overwrite baseline STL.
- After authorized Task, produce incremented name such as ARMV6 with change log, native CAD source and export settings. Preserve separately identified left and right components; no accidental layout scaling or mirrored spline incompatibility.
- If retest fails, record actual fit and cause; **do not keep increasing holes or weakening the root without analysis**. Change only one fit variable per coupon/test to isolate effect.
- Before claiming done, handoff **MECH-IF-002** with horn SKU, mating pattern, rod hole spec, safe angle envelope, mass and fit status. AI-HW and AI-SW review corresponding changes.

**STOP:** unknown spline teeth/diameter, printed spline tooth rounding/shear/slip, attempted forced engagement, cracks, missing center retaining screw, insufficient bolt engagement, rod fit requiring destructive force, PCB/frame collision or untested load-bearing print.

**Current gate:** ARM_REWORK_RULES_READY / MECH_G0_BLOCKED / NO_CAD_CHANGE_AUTHORIZED.
