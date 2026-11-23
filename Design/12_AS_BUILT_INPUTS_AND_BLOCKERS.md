# 12 — Current As-Built Inputs, Evidence Register and Blocking Unknowns

**Revision:** DRAFT-1 / 2026-11-23. **Owner:** SA. **Type:** design evidence, NOT a manufacturing authorization or implementation Task. Read after Design/AGENTS.md.  
**Scope of latest owner update:** Owner has XIAO ESP32-C3, an ESP32 gamepad, SDS1601 servo(s), two 3.7V 220mAh cells; intends ESP-NOW gamepad-to-butterfly; has a PCB from a supplied Gerber package; has printed ARMV5 but has not printed FC Mount V5; initially discussed a metal servo horn; **latest owner choice is fully 3D-printed horn with direct spline on SDS1601**, 3 collinear holes but 2 installed bolts, and more usable 1.5mm carbon sockets.

## 1. Evidence classification — DO NOT collapse these categories

- **OWNER_REPORTED:** owner says equipment is available or a fit is failing; do not present as measured design dimension.
- **FILE_INSPECTED:** properties extracted directly from supplied file(s); tool/representation constraints apply.
- **REFERENCE_DATASHEET:** advertised/reference specifications; never assume a purchased unit conforms without verifying its marking and spline.
- **MEASURED_ON_BUILD:** actual readings, dimensions, photos or tests from identifiable physical unit; currently NONE provided for horn/spline, rod diameters, PCB assembly or power.
- **PROPOSED:** new design target to review, not approved dimension or working firmware.
- **BLOCKED:** cannot responsibly manufacture, energize, choose a mate or certify fit without a missing measurement or evidence.

## 2. Supplied file provenance (attachment bytes reviewed 2026-11-23)

| Asset | SHA-256 | Inspection scope |
| --- | --- | --- |
| ARMV5(1).STL | 474048cdd7a3d5ebb9d0acd3557a93e97e11fc5a867416691d9b72613e9875b6 | Triangular surface geometry |
| FC Mount V5(1).STL | 69731d0fd0ef55a2886767e6e1e19d8d1070319c90cfe897c15835fef9ed11a7 | Triangular surface geometry |
| ESP-FLUTTER_PCB_GERBER_V1.1-Max_Imagination.rar | 55469956322e8fb065a52d4da7a66c9622db8eb9ebac841eed0d4491120ac345 | Gerber/Excellon file inventory, outline and drill metadata |
| Servo/horn reference photo (image(20261123-134833).png) | 59cb17ffb4dc3a5223b30491d2f85b307bb416cd872b76a8eeb77813796fec74 | Photograph only; NOT a dimensioned drawing |
| **SG90_one_horn_without_holes.stl** | **6b54afba5a9ba01da71d5fba9db7474e690e79ad3da17c99f72833280b02d161** | Single watertight 3,908-face mesh, ~7mm hub OD and ~19.65mm total length; no confirmed SDS1601 25T coupling; see Design/16 |

The attachments exist in this conversation working environment, **not in GitHub**. Do not assume later AI sessions can open a local /mnt/data path or retrieve these bytes from this document. The owner must make source files available to assigned agent (reattach or supply explicitly approved reference access); verify hashes before editing. Do **not** upload copyrighted reference binaries to a public repo without permission.

## 3. Mesh findings (Trimesh geometric inspection)

| File | Observed | Engineering interpretation |
| --- | --- | --- |
| ARMV5(1).STL | Overall mesh bounds approx 79.68 × 11.50 × 49.32 mm; **two disconnected watertight components**; each volume approx 1626.5 mm³ | Likely two matching/mirrored arm parts positioned on a shared print bed, not one 79.68mm-wide functional arm. Do NOT treat the overall bounding box as a single arm |
| FC Mount V5(1).STL | One watertight component; bounds approx 32.0 × 39.2 × 38.0 mm; volume approx 4052.6 mm³ | Geometric mesh integrity is NOT proof of physical fit or clearance |

Coordinates/units are interpreted as millimeters consistent with intended prints; **STL itself does not encode units**. Verify slicer import scaling, orientation and mesh-to-physical measurements. Neither volume predicts final mass without material, density, slicing and infill decisions.

**Reported failure:** only one of the intended four nominal 1.5mm carbon-rod sockets accepts the actual rod; arm-horn attachment feels too small/has only one hole. **No exact local bore diameters, axis angles, print shrinkage, fastener thread, horn hole spacing or measured rod diameter are established yet.** The watertight-mesh check cannot establish these fit tolerances.

## 4. Servo/horn compatibility is a HARD BLOCKER

**Owner-confirmed latest design:** a completely **3D-printed plastic horn including its own SDS1601-mating internal spline** made from the supplied SG90-style STL; **three holes straight in a line**, ARMV5 root **two matching outer holes**, **two installed bolts**. No metal horn/insert by default. Initial SA candidate is M1.6 with Ø1.8mm clearance; **not a manufacturing approval**. See [16_HORN_3HOLE_2BOLT_PRETASK_PLAN.md](16_HORN_3HOLE_2BOLT_PRETASK_PLAN.md).

- **Historical superseded proposal:** metal horn 21T/~4.5mm. Owner rejected metal-horn architecture in favor of fully printed direct coupling. Do not use 21T geometry to model or print the chosen horn. Confirm actual SDS1601 spline tooth count/diameter and use accurately printed mating geometry.
- Reference SDS1601 specification image says **25T, approx Ø4.90mm spline**. That differs in tooth count and nominal diameter from 21T/4.5mm. A photo of a blue horn does not prove its spline count or mounting geometry.
- **Never force an imprecise printed bore onto SDS1601, drill its spline, or treat a simple Ø4.9mm round hole as a 25T tooth profile.** Confirm purchased servo tooth count, measured major/minor diameters, engagement depth and center retention screw before CAD freeze. Print separate spline-fit coupons and test without wing load.
- Three different joints: (1) servo spline to **printed hub spline**, (2) original servo shaft-center retention fastener, (3) **two separate arm-to-horn bolts** at H1/H3 (H2 unused) and their 7mm provisional center spacing. The owner's two-bolt request concerns joint (3) unless explicitly clarified. Two arm-to-horn bolts do not replace the servo center retention screw.
- Check metal horn lever length, thickness, sweep, arm load path, screw-head/washer clearance and receiver-side cable interference. No new weight/strength claim before assembling.

## 5. PCB Gerber V1.1 findings (NOT electrical signoff)

- Archive was readable and contains **12 entries**: F_Cu, In1_Cu, In2_Cu, B_Cu; F/B Mask, F/B Paste, F/B Silkscreen; Edge_Cuts and one Excellon *.drl. **Four copper layers** are represented.
- Edge_Cuts coordinates (Gerber metric format 4.6) span approx **18.5 × 45.0mm** as an axis-aligned bounding rectangle. The actual board outline includes curves; box is not the full mechanical drawing.
- Provided drill file declares tool diameters **0.30mm** (two named tools) and **0.60mm**, with no clearly listed large mounting holes in that drill file. This is **NOT** enough to conclude that the assembly requires no mounting holes: check edge outline, castellations/slots and complete original manufacturing package.
- Gerber layer headers advertise **KiCad/Pcbnew 10.0.4 (2026-08-24)**, while the drill-file header advertises **Flux PCB Export 1 (2026-08-24)**. They may still be compatible; **registration, board origin, drill alignment, plating and manufacturing coherence are NOT proven**. Compare drill centers against pad positions in a CAM viewer before fabrication/assembly.
- Files contain manufacturing shapes, NOT necessarily complete schematic, netlist, BOM, C/P pick-and-place, component rotations, polarity or assembly directions. The owner is preparing to solder parts: require a BOM/assembly cross-check **before** connecting battery.
- The earlier reference wiring/PCB/config documentation disagrees on ADC divider values; the actual assembled resistor values and net connectivity must be measured, not guessed from artwork.
- FC Mount mesh bbox and PCB bbox cannot establish compatibility because they may use different coordinate axes and origins. Require **physical PCB outline with holes/connector keep-outs vs mounted CAD datum**.

## 6. Actual supplied hardware and confidence

| Item | Owner-provided fact | Unknown or gate |
| --- | --- | --- |
| XIAO ESP32-C3 | Physically available | Exact board revision, antenna configuration, boot GPIO, actual soldered footprint |
| ESP32 DIY gamepad (Shopee link supplied) | Physically available, intended control transmitter | Exact chip/module, pin map, joystick/buttons/supply, preloaded firmware and ESP-NOW compatibility NOT inspected |
| SDS1601 servo(s) | Physically available | Actual spline/shaft, two-servo count, electrical batch data, mounting hole pattern/current and PWM operating limits |
| Two 3.7V 220mAh cells | Available | Individual cell health, polarity, model/C rating, whether already wired 2S, charge/balance connector and protected power distribution |
| Gerber V1.1 | Supplied, owner preparing PCB assembly | BOM/schematic/net verification, board revision/assembly side, safe power rail and ADC resistor validation |
| ARMV5 printed | Printed, reported mechanical fit defects | Printed orientation/material/nozzle, true rod sizes, horn pattern measurements, caliper dimensions |
| FC Mount V5 | Only STL available, not printed | Fit of assembled PCB, connectors, antenna/USB/pack and servo wires |

**ESP-NOW** is an explicit owner direction for the pilot transport. It replaces the earlier *phone as primary pilot* assumption in design proposals; implementation remains unapproved until owner signs the updated design.

## 7. Blockers and next evidence — not yet coding tasks

| ID | Severity | Missing evidence | Owner/role that must validate |
| --- | --- | --- | --- |
| B-3D-01 | BLOCKER | Actual SDS1601 spline/retention dimensions and printer ability to reproduce **matching printed internal micro-spline**, engagement and fatigue | Owner physical check; AI-3D + AI-HW |
| B-3D-02 | BLOCKER | Three collinear horn holes (H1/H2/H3), two corresponding ARM root holes, M1.6 candidate screw stack, horn/root wall strength | Owner measurement; AI-3D |
| B-3D-03 | MAJOR | Actual carbon rods diameter and fit by hole, machine/material/print orientation; calibration coupon | Owner measurement; AI-3D |
| B-3D-04 | MAJOR | Mounted PCB physical orientation vs FC Mount, connector keep-outs and all-up mass | Owner/AI-3D + AI-HW |
| B-HW-01 | BLOCKER | PCB 4-layer Gerber/drill registration and assembly BOM/schematic/net verification | AI-HW |
| B-HW-02 | BLOCKER | Actual pack wiring, charger/2S balancing, power paths, protection, USB backfeed | Owner/AI-HW |
| B-HW-03 | BLOCKER | Battery divider circuit vs software calibration; correct servo voltage/PWM pins and power-on levels | AI-HW + AI-SW |
| B-SW-01 | MAJOR | Exact gamepad MCU and controls, button roles, ESP-NOW pairing and RF channel behavior | Owner/AI-SW |
| B-SW-02 | BLOCKER for powered motion | Verified STOP/ARM/failsafe protocol, timeout and rearm tests | AI-SW + SA |
| B-LIC-01 | MAJOR | Rights to redistribute/modify ESP-FLUTTER code/CAD/PCB assets | Owner + SA |

## 8. Decision and scope policy

This file is a factual register and risk list. It **does not authorize** modifying STL, fabrication outputs, PCB or firmware; do not create Task files until owner design approval. Design changes must reference provenance hash and distinguish measured, user-reported, inferred and proposed values.

## 9. Reference verification pointers (not an as-built measurement)

- SDS1601 reference spec image in original project Drive: 25T / Ø4.90mm, 1520us neutral, 333Hz refresh and 800–2200us pulses.
- Independently published SDS1601 specifications: https://eriksen.co.th/index/product/detail/id/5314.html?lang=zh-cn
- Despite documentation consistency, the owner must verify the **physical purchased unit** and **printed spline coupon** with a harmless unpowered hand-fit before adopting printed horn; do not force or damage shaft.
- Exact Shopee gamepad product listing was not directly accessible for verification; do not assume joystick/buttons, module revision or shipping firmware from its URL alone.

**SA gate:** BASELINE_RECORDED / HORN_COMPATIBILITY_BLOCKED / PCB_ASSEMBLY_UNVERIFIED / ESP_NOW_INTERFACE_DESIGN_REQUIRED.
