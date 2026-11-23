# 08 — Multi-Agent Collaboration Rules (SA + 3 Senior AIs)

**Status:** DESIGN_DRAFT. Applies to AI-3D, AI-HW and AI-SW once owner signs off.  
**Purpose:** divide disciplines without overlapping source ownership, preserve technical evidence and coordinate cross-domain decisions.

## 1. Team and authority

| Role | Primary accountability | May approve | Cannot approve |
| --- | --- | --- | --- |
| Product Owner | Product intent, budget, real-world build and merge decision | Scope, design baseline, powered test/flight go | Fabricate evidence |
| SA (lead) | End-to-end solution, architecture, interface contracts, requirements, tasking, PR review | Design proposals within owner-approved scope; SA PR review verdict | Owner's hardware go / merge without delegation |
| AI-3D (senior) | Wings, mechanism, servo mounts, enclosure, tolerances, STL/STEP/print/process risk | Mechanical recommendation to SA | Electrical ratings, firmware safety implementation |
| AI-HW (senior) | Power supply, battery/charging safety, schematic, pinout, sensors, PCB, electrical test strategy | Electrical recommendation to SA | Mechanical mass/fit without measured CAD, software release |
| AI-SW (senior) | ESP32 firmware, control algorithms, UI, telemetry, unit/integration tests | Technical software recommendation to SA | Battery power/servo travel limits without HW/3D sign-off |

No senior AI gets independent authority to override SA and owner decisions. "Senior" is an expected analysis/review standard, **not** an independently running agent.

## 2. Cross-domain source of truth and ownership

- **Design/**: SA-owned requirements, approved interfaces, ADR, rules and audit ledger. Other agents READ ONLY.
- **Task/**: SA-owned task briefs after DESIGN_APPROVED, including agent owner, exact permitted file paths, baseline SHA, dependencies and tests.
- **Mechanical artifacts**: future folder name is proposed `Mechanical/`, not a permission to create or edit anything until SA assigns a task.
- **Electronics artifacts**: future folder name is proposed `Hardware/`, not a permission to create or edit anything until SA assigns a task.
- **Firmware/UI/tests**: future `src/`, `include/`, `web/`, `test/`, `platformio.ini`, per Task allowlist only.
- **ReviewPR/**: SA-only isolated temporary PR worktree; specialists never touch it.
- No agent may modify another domain's deliverables, rename a shared pin/net/connector, alter common file format or shift design constraints without a signed change request.

If a task spans multiple domains, split it into ordered sub-tasks/PRs with a stable handoff artifact. Prefer **one file owner** for shared config; producer updates contract through SA, consumer implements against pinned revision.

## 3. Mandatory interface contracts

Every inter-agent handoff must provide a **versioned, unambiguous contract** (documented under an SA-controlled Design/ document/ADR or attached to assigned Task), not loose chat claims.

| Interface | Producer -> Consumer | Required fields / evidence |
| --- | --- | --- |
| MECH-IF-001 | AI-HW -> AI-3D | Exact physical part numbers, measured dimensions, connector footprint, masses, mounting holes, cable keep-outs, screw sizes, service access, regulator/pack heatsink/ventilation limits |
| MECH-IF-002 | AI-3D -> AI-HW + AI-SW | Wing orientation, mechanical neutral/polarity, permitted angular travel, collision envelope, total mass/CG and mounting revision, print material/process, acceptance |
| ELEC-IF-001 | AI-HW -> AI-SW | Board/revision, net-to-GPIO mapping, ADC divider values, sensor scale/calibration, servo supply range, PWM signal electrical compatibility, max current, cutoff/fault input, boot-safe pin state |
| SW-IF-001 | AI-SW -> AI-HW + AI-3D | Proposed PWM output protocol/rate, arm-disarm states, startup signal behavior, configured servo limits, control timeout and safe recovery behavior, observability needed for bench testing |
| SYS-IF-001 | SA -> all | Requirement IDs, owner-approved architecture, ADR state, accepted trade-offs, safety and integration gate, immutable design SHA |

For each interface include: `contract_id`, `revision`, `source_ref`, `producer`, `consumers`, `units`, `tolerances`, `test_method`, `evidence_status`, `approval`. Numbers with unknown tolerance remain `TBD` and block dependent powered tasks. Use SI units and distinguish electrical voltage, servo PWM refresh Hz and wing flap Hz.

## 4. Dependency order

1. SA scopes and obtains approval of baseline requirements/design.
2. AI-HW and AI-3D perform **read-only research/proposals** in parallel where data is not yet known.
3. Once approved tasks exist, AI-HW confirms actual component dimensions/electrical envelope (ELEC-IF-001 + MECH-IF-001); AI-3D establishes fit, service layout, wing linkage envelope (MECH-IF-002). Iterate via SA.
4. AI-SW may build **host-only state machine/parser tests** against approved mock interfaces while real hardware is pending; any PWM/pin/battery integration waits for HW evidence.
5. Integration review is tri-disciplinary: AI-3D signs mechanical clearance claims, AI-HW signs power/servo electrical claims, AI-SW signs software safety claims. **SA evaluates the complete system; owner authorizes powered/flight work.**
6. A failed gate stops escalation; don't retry with larger motion/current or new features until cause is understood and documented.

## 5. Change-request protocol

When finding conflicts or new requirements, **do not patch across boundaries**. Submit a concise `CHANGE_REQUEST` to SA with:

```text
CR-ID:
Origin role and Task:
Affected requirement / ADR / interfaces:
Observed evidence and source:
Proposed change (+ alternatives):
Mechanical / electrical / software / battery impact:
Risk and rollback:
Tests needed:
Blocking: YES/NO
Decision requested from SA/owner:
```

SA performs impact review, coordinates affected agents and, if needed, commits Design revision with explicit owner approval. Task may then be updated with the new pinned design SHA. No agent unilaterally updates a safety threshold.

## 6. Branches and PRs

- Start only from assigned approved Task. Suggested branches: `feature/task-<ID>-3d-...`, `feature/task-<ID>-hw-...`, `feature/task-<ID>-sw-...`.
- Distinct agent branches/worktrees; do not share worktree or push onto another AI's branch.
- One agent owner per PR; cross-domain dependencies are links to source commit SHA/contract revision.
- Every PR includes: owner role, Task ID, design SHA, input contract revisions, exact output files, engineering assumptions, tests with evidence, failures, rollback, licensing and reviewers needed.
- SA reviews every PR per [05_AI_WORK_RULES.md](05_AI_WORK_RULES.md), using isolated ReviewPR/ for local review and posting comments.
- No BLOCKER/MAJOR -> SA comments approval, cleans its ReviewPR worktree and assigned Task under the established rule. BLOCKER/MAJOR -> SA keeps both, appends numbered remediation items and requires re-review. Product Owner handles merge.
- Do not delete tests, CAD source or schematic source as a cleanup operation; only the SA-owned temporary Task and review checkout may be removed under the review policy.

## 7. Acceptance terminology

- `REFERENCE_VERIFIED`: visible in third-party docs only (not ours).
- `DESIGN_PROPOSED`: engineering option, no owner acceptance.
- `SIMULATION_PASS`: exact model/version and inputs passed; NOT hardware.
- `SOFTWARE_PASS`: host/build tests with logs and SHA, NOT bench/flight.
- `CAD_CHECK_PASS`: static geometry/fit check on revision, NOT printed/assembled.
- `HW_BENCH_PASS`: owner-permitted physical measurement, named equipment/conditions.
- `INTEGRATION_PASS`: end-to-end integrated bench test with all relevant approvals.
- `FLIGHT_TEST_APPROVED`: owner explicitly authorized the bounded trial, NOT evidence of performance.
- `BLOCKED` or `NOT_RUN`: no assumption of pass.

When one domain's result contradicts another's, use the **more restrictive safe condition** until SA resolves it. Never assume reference flight videos establish our battery life, lift or stability.

## 8. Gate-specific review matrix

| Gate | AI-3D review | AI-HW review | AI-SW review | SA / owner |
| --- | --- | --- | --- | --- |
| Prototype concept | Shape, mass/CG, mount plan | Parts and power budget | Software architecture | SA design; owner approval |
| Servo no-wing bench | Horn accessibility/clearance | Rail/ADC/PWM/wiring | Neutral/STOP/disarm logic | SA verifies evidence, owner authorizes |
| Full-wing bench | Collision/tolerance/deflection | Load, thermal, pack sag | Waveform/clamps/failsafe | SA review + owner authorization |
| Tethered | Structural inspection, CG | Pack and wiring safety | Control/telemetry logs | Owner GO after SA review |
| Free-flight | Physical failure assessment | Electrical failure assessment | Software recovery paths | Owner explicitly GO / NO_GO |

## 9. Communication discipline

- Raise BLOCKER immediately when a safety invariant or required evidence is missing. Report limitations rather than inventing values.
- Use immutable SHA and versioned evidence; avoid floating "latest" as a technical input.
- Do not run potentially destructive actions (force-push, unscoped deletes, energized actuator tests) without explicit authorization.
- AI-role peer review is advisory; SA records the formal PR review and owner remains the merge authority.
- At the end of every Task, provide the role handoff fields defined in [AGENTS.md](AGENTS.md).

**Current state:** AGENT_RULES_DRAFT; Design not owner-approved; no Task/ assigned.
