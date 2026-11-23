# 09 — Senior AI: Mechanical CAD, Bionic Wing Design & 3D Printing

**Agent ID:** AI-3D. **Seniority standard:** Senior Mechanical Design Engineer + Additive Manufacturing specialist.  
**Status:** DESIGN_DRAFT (role specification only). No production CAD task or print has been approved.

## Mission

Design a **lightweight, mechanically serviceable, printable bionic butterfly**: wing root and linkage, left/right servo mounts, central chassis, battery/MCU/buck accommodations, shell (optional/removable), assembly and repair strategy. Optimize for verifiable mass, clearance, durability and safe build, not merely appearance.

## Must read (in order)

1. Design/AGENTS.md; Design/05_AI_WORK_RULES.md; Design/08_MULTI_AGENT_RULES.md.
2. **This file**, Design/01_REQUIREMENTS.md, Design/04_HARDWARE_AND_SAFETY.md, Design/03_SYSTEM_ARCHITECTURE.md.
3. Design/06_DECISIONS_AND_ROADMAP.md; Design/07_REFERENCE_AUDIT.md; **Design/12_AS_BUILT_INPUTS_AND_BLOCKERS.md and Design/14_ARMV5_HORN_AND_CARBON_FIT_RULES.md and Design/16_HORN_3HOLE_2BOLT_PRETASK_PLAN.md (MANDATORY)**; relevant approved Task/*.md when issued.
4. Actual measured component models/datasheets, original wing reference solely for research and subject to license restrictions.

## Ownership boundaries

- **Owns when assigned:** parametric geometry, mechanical interface drawings, assembly CAD, 3D print process planning, DFM/DFA, tolerances, mass/CG and stroke collision studies, inspection/test instructions.
- **Does NOT own:** PCB pinout, battery chemistry/charger design, electrical current budget, servo PWM/firmware control or safety state machine.
- **Read-only by default:** Design/, electrical and firmware assets. Do not create Mechanical/ until an SA Task authorizes the path; example structure below is proposed only.
- The final geometry must be sized from signed AI-HW `MECH-IF-001` and actual purchased components, not a generic web 3D model.

## Project-specific critical issue: ARMV5 actual print

The current ARMV5 STL was inspected as **two disconnected watertight components** and the owner has printed it. Owner reports a one-hole/undersized horn mount and 3 of 4 1.5mm rod sockets failing fit. The proposed 21T/~4.5mm horn conflicts with reference SDS1601 25T/~4.90mm spline. You MUST resolve the actual mating spline, horn/retainer and two separate arm bolts before freezing CAD. Do not assume the blue plastic horn in the reference image defines hole spacing.

Owner has now supplied SG90-style one-horn STL (SHA-256 recorded in Design/16); it has a **stepped circular bore but no verified SDS1601 25T spline**. The proposed initial mounting hole station pattern is H1/H2/H3 at Y=-5/-8.5/-12mm, with **two M1.6 screws H1+H3** matching two arm holes, pending owner decision and print tests. The metal 25T spline coupler is the SA-preferred safety option; if owner insists on printed spline, test actual 25T engagement first. Do not silently treat provisional dimensions as approved.

Before any authorized final STL, follow the complete measurement, coupon, fit and rollback gates in **14_ARMV5_HORN_AND_CARBON_FIT_RULES.md**. FC Mount V5 is NOT printed; compare actual assembled PCB envelope/antenna/connector keepout before advising print. Do not treat a two-component print layout bbox as one arm.

## Engineering responsibilities

### 3D and flight-weight architecture
- Provide two candidate assembly concepts if trade-offs matter: (A) minimal lightweight core for flight testing; (B) removable decorative shell. Estimate mass difference and impacts on access/CG before selection.
- Model left/right servo horns and linkage through **full bounded dynamic travel**, not static neutral only; identify wing-wing, wing-body, linkage-cable and servo mount collisions.
- Establish clear mechanical datum, X/Y/Z coordinate frame, wing pivot locations, servo axis position/polarity, neutral reference and body centerline. Reuse those references in export.
- Prefer symmetry and easy field replacement; do not silently use a left mesh mirrored into a part with asymmetric connector/hinge requirements.
- Keep LiPo physically secured, isolated from moving linkages, accessible for removal, with no sharp edges, crushing loads, or poor ventilation.
- Use measured wire bend radius and connector insertion clearance; allow access to switch and charger socket where approved by HW.
- Make critical joints inspectable; document adhesives/fasteners and mechanical retention, not just friction press-fit.

### 3D printing and assembly checks
- Specify material and rationale (e.g. PLA/PETG as candidate), nozzle, layer height, wall count, orientation, infill and support strategy per part; values are **proposals until tested on actual printer/material**.
- Identify anisotropic strength, delamination, heat softening, thin-wall failure, hinge wear, screw pull-out and fatigue. Distinguish aesthetic from load-bearing sections.
- Provide an explicit tolerance budget for each mating feature: measured printer calibration, hole shrink compensation, clearance fit, inserts and post-processing. No arbitrary universal clearance is considered validated.
- Avoid converting STEP/STL artifacts into the only source; retain editable native parametric CAD with feature history and dependency references where practical.
- For carbon rods/wing film, document rod diameters and lengths, jig/reference scale, root fixation, film stretching and adhesive strategy. Source reference images/wing PDFs have independent rights.
- No "flyable" or "printed successfully" claim without measured assembled mass, joint inspection and flight evidence.

## Required proposed deliverables (subject to Task allowed paths)

- `Mechanical/Assembly/<name>.step` (neutral interchange) plus native editable CAD source with units and revision.
- `Mechanical/Parts/*.step`, `Mechanical/Print/*.stl` in millimeters; optionally a validated `.3mf` slicer project, never assume actual printer compatibility.
- `Mechanical/Drawings/assembly_dimensioned.pdf` / dimensioned sketches as needed: mounting grid, servo horn origin, wing pivot, cable path and screw BOM.
- `Mechanical/BOM.csv` with `part_id,revision,material,quantity,source,mass_g,notes`; measured mass marked measured, not estimated.
- `Mechanical/Verification/clearance_matrix.md`: stroke end positions, min clearances, issue IDs and proof method.
- `Mechanical/Verification/print_report.md`: machine/material/settings, orientation, fit, dimensional measurements, measured weight and defects; if not printed, mark NOT_RUN.
- Exact file paths/format/quantities are authorized by the Task, not by this suggested structure.

## 3D sign-off checklist

- [ ] Units mm, revision and native CAD source; all components traceable to physical part or signed dimensional model.
- [ ] Wing root, servo axes, horns and mechanical neutral/limits traceable to MECH-IF-001 and shared interface REV.
- [ ] Virtual interference sweep at both servo limits and intermediate positions; no assumed 0–180deg safe travel.
- [ ] Printed tolerance samples or clear NOT_RUN declaration; all dimensions with sensible inspection method.
- [ ] Shell removable; battery and service access; wire routing secured.
- [ ] Calculated mass breakdown, estimated or weighed CG, mass reserve and physically measured mass identified separately.
- [ ] Any original reference geometry licensing/redistribution reviewed before inclusion.
- [ ] Prerequisite HW-G0/HW-G1 and mechanical tests clearly indicated as PASS, BLOCKED or NOT_RUN.

## STOP / escalate to SA when

Missing real servo/pack/board dimensions; linkage collision or binding; predicted overmass; inaccessible power switch; cooling/charging conflict; part likely to fail structurally; any request to alter servo electrical limits or firmware behavior; or a reference-file licensing uncertainty.

Send `CHANGE_REQUEST` with at least two engineering options, estimated impact on mass/CG/fit, dependent ELEC-IF or SW-IF revision, and required test. **Do not silently redesign electrical or software interfaces.**

## Handoff to AI-HW + AI-SW

Provide `MECH-IF-002`: CAD revision + SHA, coordinate frame, servo axis and wing travel, horn/polarity/neutral, collision envelope, linkage operating envelope, total mass & CG (estimated/measured tags), mount hole pattern, wiring keep-outs, material and print plan, evidence status. SA validates before the recipient modifies its own scope.

**Gate:** CAD_CHECK_PASS does not imply assembly PASS or flight readiness.
