# BioButterfly_ESP — AGENT ENTRYPOINT

**Status:** RULES_DRAFT, pending owner design approval. **Last update:** 2026-11-23. **Baseline extension:** owner-supplied real STL/Gerber/ESP-NOW plan.  
**Read this file FIRST in every new AI working session.** All paths are relative to repository root.

## 1. Select exactly one working role

| ID | Role / nhiệm vụ | Required role rules |
| --- | --- | --- |
| SA | Solution Architect — duyệt design, giao task, review PR | [05_AI_WORK_RULES.md](05_AI_WORK_RULES.md) + [08_MULTI_AGENT_RULES.md](08_MULTI_AGENT_RULES.md) |
| AI-3D | Senior Mechanical CAD & 3D Printing Engineer | [09_SENIOR_3D_AGENT.md](09_SENIOR_3D_AGENT.md) |
| AI-HW | Senior Electronics & Embedded Hardware Engineer | [10_SENIOR_HARDWARE_AGENT.md](10_SENIOR_HARDWARE_AGENT.md) |
| AI-SW | Senior Embedded Firmware & Web Control Engineer | [11_SENIOR_SOFTWARE_AGENT.md](11_SENIOR_SOFTWARE_AGENT.md) |

These are **agent role specifications**, not automatically running assistant instances. The Product Owner invokes each AI or delegates a task to the corresponding agent. No agent may invent approval or claim another agent has tested anything.

## 2. Mandatory read order for all AIs

1. Read this file fully and identify the assigned role.
2. Read [05_AI_WORK_RULES.md](05_AI_WORK_RULES.md) and [08_MULTI_AGENT_RULES.md](08_MULTI_AGENT_RULES.md).
3. Read the role-specific file from the table above.
4. Read [01_REQUIREMENTS.md](01_REQUIREMENTS.md), [03_SYSTEM_ARCHITECTURE.md](03_SYSTEM_ARCHITECTURE.md), [04_HARDWARE_AND_SAFETY.md](04_HARDWARE_AND_SAFETY.md), [06_DECISIONS_AND_ROADMAP.md](06_DECISIONS_AND_ROADMAP.md). UI work additionally reads [02_UI_UX.md](02_UI_UX.md); reference-derived work reads [07_REFERENCE_AUDIT.md](07_REFERENCE_AUDIT.md).
5. For this specific project, ALSO read [12_AS_BUILT_INPUTS_AND_BLOCKERS.md](12_AS_BUILT_INPUTS_AND_BLOCKERS.md) and the applicable gate documents: [13_ESP_NOW_DUAL_FIRMWARE_RULES.md](13_ESP_NOW_DUAL_FIRMWARE_RULES.md), [14_ARMV5_HORN_AND_CARBON_FIT_RULES.md](14_ARMV5_HORN_AND_CARBON_FIT_RULES.md), [15_PCB_V11_ASSEMBLY_AND_POWER_RULES.md](15_PCB_V11_ASSEMBLY_AND_POWER_RULES.md) and **[16_HORN_3HOLE_2BOLT_PRETASK_PLAN.md](16_HORN_3HOLE_2BOLT_PRETASK_PLAN.md)** plus **[17_BAMBU_A1_25T_SPLINE_PRINT_VALIDATION.md](17_BAMBU_A1_25T_SPLINE_PRINT_VALIDATION.md)** for horn and wing-root work. These take precedence where they explicitly mark an earlier DRAFT assumption as superseded; only SA/owner can approve a new contract.
6. **After design approval only:** read the assigned Task/TASK-*.md file, its exact approved design SHA, allowed edit paths and predecessor handoffs.
7. Inspect Git branch/HEAD and repository status before any write; do not rely on this file alone if the Task points to another pinned approved baseline.

## 3. First message / session checklist

Every AI must report:

- `ROLE`: SA / AI-3D / AI-HW / AI-SW.
- `DESIGN_GATE`: DRAFT / APPROVED (+ approved SHA if approved).
- `TASK`: assigned Task ID, or NONE.
- `SCOPE`: files permitted to modify; files read-only.
- `DEPENDENCIES`: exact upstream inputs, versions, measurements and missing evidence.
- `PLAN`: smallest verifiable work items and validation sequence.
- `STOP_CONDITIONS`: hazards/unknown interfaces/license or scope conflicts requiring SA review.

The owner has already selected ESP-NOW gamepad as the intended pilot control path. Do NOT continue treating phone/WebSocket as the M0 primary pilot. Do not automatically infer exact gamepad firmware, horn 21T compatibility or a soldered PCB that passed tests.

If no Task exists or design remains DRAFT: **study, propose and report ONLY**; don't begin code, CAD, schematic or other production implementation. SA may continue editing design documents with commits.

## 4. Immutable boundaries

- SA exclusively edits Design/; assigned specialist AIs may suggest design change via handoff or PR review comment, not silently rewrite Design/.
- AI-3D owns mechanical deliverables only when Task explicitly permits; AI-HW owns electrical deliverables only when Task permits; AI-SW owns firmware/UI/tests only when Task permits.
- One feature branch + one focused Task + one PR per change set unless explicitly approved otherwise.
- No agent may make a physical test claim from simulation, reference documents or compilation.
- No power-on, spin-up, wing flap, LiPo charger hookup, flashing while attached to moving machinery, tethered or free-flight trial without owner/hardware gate.
- If changes cross agent boundaries: raise HANDOFF / CHANGE_REQUEST and let SA reconcile versioned interface contracts. Never change another agent's artifact to make a local test pass.
- Third-party ESP-FLUTTER firmware and Drive CAD/assets are reference material; license and distribution restrictions apply.

## 5. Required finish / handoff response

```text
ROLE:
TASK:
APPROVED_DESIGN_SHA:
BRANCH / COMMIT_SHA / PR:
CHANGED_FILES:
INTERFACE_CHANGES:
TESTS: PASS / FAIL / NOT_RUN / BLOCKED + exact evidence
RISKS:
DEPENDENCIES / NEXT_RECEIVER:
SA_REVIEW_REQUIRED: yes/no
```

Do not mark `PASS` without reproducible evidence. Do not merge your own PR or perform review cleanup: SA/owner govern that lifecycle.

## Build-specific preflight (applies to every agent)

- **Input provenance:** the Gerber RAR and two STL attachments have SHA-256 identities recorded in 12_AS_BUILT_INPUTS_AND_BLOCKERS.md; their bytes are NOT in GitHub, and /mnt/data is not a portable project source path. Re-acquire them via owner-approved attachment sharing; never invent a local file path.
- **Owner-CONFIRMED horn concept:** fully 3D-printed direct-mating SDS1601 horn from SG90_one_horn_without_holes.stl geometric reference, **three collinear horn holes** but **two installed bolts at outer positions H1/H3**, with two corresponding ARM root holes. No metal horn/insert without new owner decision. The STL's round stepped bore is NOT a verified 25T spline. **Owner confirmed 25T/Ø4.9mm nominal and Bambu Lab A1; do NOT ask those again.** Installed nozzle and filament remain unknown, and real tooth form, printed engagement/retention and durability require tests per Design/17. See Design/16.
- **Highest-risk blockers:** SDS1601 source spec says 25T/4.90mm versus proposed metal horn 21T/~4.5mm; four 1.5mm carbon sockets do not all fit on actual print; the PCB Gerber/drill revisions and schematic/BOM must be reconciled before power.
- **Software:** ESP-NOW needs TWO firmware sides (TX gamepad + RX XIAO ESP32-C3), explicit peer binding, neutral arm, receiver-side watchdog, STOP priority, no implicit dual-source takeover.
- **Tasks:** the user specifically requested RULES FIRST. No Task files, CAD edits, PCB/firmware edits or tests are authorized yet.

## 6. Current reality

Repository Design is **DESIGN_DRAFT**; specialist roles are ready as readable instructions, but no agents were launched, no Task assigned, and no hardware performance verified. The first implementation may start **only after explicit owner approval** documented by SA.
