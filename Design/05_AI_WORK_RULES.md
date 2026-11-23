# 05 — AI Work Rules, SA Governance and PR Review

**Version:** 1.0 DRAFT, 2026-11-23. **Repository:** nguyenbui5701/BioButterfly_ESP. **Primary authoritative working documents:** Design/. **Effective for all future agents after owner sign-off.**

## 1. Roles and ownership

| Role | Authority and scope | Must not |
| --- | --- | --- |
| Product Owner (user) | Define vision, approve design and hardware risks, authorize implementation/merge | Assume test PASS from verbal claims |
| Solution Architect (SA) | Own Design/ requirements, architecture, rules, design change decisions, task decomposition after approval; review PR and post findings | Modify AI Code worktree, silently merge unapproved changes, silently expand scope |
| AI Code | Implement one approved Task at a time on its own feature branch; write tests and PR evidence | Change Design/ or start unassigned features; bypass safety gates |
| Reviewer (SA) | Read rules, Task, base/head diff and relevant tests; use isolated review checkout; leave PR review comment | Review by reading PR title only; contaminate AI Code working tree |
| Builder/tester | Verify actual hardware, execute approved bench procedures and record proof | Attempt flight based on software build PASS |

The SA normally commits **only files under Design/**; exception: creating/updating/removing individually approved Task/*.md files **after design freeze**. SA does not modify firmware or UI production code by default. Any exception needs explicit owner authorization and separate scope.

## 2. Canonical folders

- Design/ — versioned requirements, architecture, UI, hardware, rules, ADRs, review criteria; SA-owned and persistent.
- Task/ — small, owner-approved implementation assignments; SA writes and commits task files only after design gate. Active Task files are temporary and are removed by SA after nonblocking review, retaining Git history.
- src/, include/, web/, test/, platformio.ini (exact code layout selected later) — implementation owned by AI Code.
- ReviewPR/ — dedicated **LOCAL, TRANSIENT** checkout/worktree for SA review, never the AI Code workspace, never part of an implementation PR and never committed as source. Directory name is **ReviewPR** (no space). The earlier verbal alias "Review PR" means this same folder.
- Reference/ or third-party materials — not created by default; separate owner/license approval required before importing assets.

GitHub review may use API-based read-only file inspection **when no checkout is required**. However, whenever the PR needs local build/tests/source execution, SA must checkout into an isolated ReviewPR/ workspace; never run review tests in AI Code's working directory. The user-requested default is to use ReviewPR/ for review.

## 3. Branch and commit strategy

1. AI Code branches from fresh main, naming convention: feature/task-<ID>-<short-topic>.
2. One Task -> one focused PR unless task explicitly authorizes a dependent series; each PR references its Task and target Design document revision.
3. SA design commits directly to main during initial setup because repository is currently empty and user specifically requested immediate commit. After baseline/design freeze, prefer a separate docs branch + PR for cross-cutting changes where other agents are working.
4. Never force-push, rewrite shared branch history, delete unrelated files, or commit secrets, batteries' proprietary downloads, CAD binaries or build artifacts without review.
5. Each meaningful design or task change is committed immediately. Commit subject: docs(design): ..., task(...): ..., fix(...): ..., test(...): ...
6. Always report actual **commit SHA** and GitHub link; do not claim committed if the write response failed.
7. AI Code makes implementation commits; SA may post review comments or documentation/task commits but does not edit AI Code PR head by default.

## 4. Design-before-task gate

**DESIGN_DRAFT -> DESIGN_REVIEW -> DESIGN_APPROVED -> TASK_READY.**

- No implementation Task may be generated while the owner has not approved the design baseline.
- Approval must identify exact Design commit SHA, hardware assumptions, unresolved risks and approved MVP scope in 06_DECISIONS_AND_ROADMAP.md.
- Design changes affecting safety, API, UI interaction or power need SA impact analysis and owner approval **before** AI Code applies them.
- Tasks must refer to a pinned design SHA or an immutable approved release tag, to prevent moving targets.
- Unknown physical parameters remain blocking for wiring/flight tasks but may be mocked for pure host-side logic tests.

## 5. Task file format (required)

Filename pattern: Task/TASK-<number>-<slug>.md. Every task includes:

- Header: task ID, version, owner, status, priority, base branch, design baseline SHA, PR link (once created).
- Goal / business value / phase.
- **Read first:** exact Design/ documents plus specific source files to inspect; no wild undocumented assumptions.
- Scope and explicit exclusions; expected files to CREATE/EDIT/READ ONLY.
- Interface contracts, test plan, local build and deployment commands.
- Safety precautions for powered tests and required physical gates.
- Quantitative acceptance criteria, evidence artifacts with timestamps/commit SHA.
- Stop conditions / dependencies / handoff point and items requiring owner/SA decision.
- Prohibition on unapproved changes and instructions to leave unrelated source untouched.

AI Code must confirm design/task versions at start. If ambiguity changes behavior or safety, stop that part, document options and ask SA; do not invent electrical ratings or external API contracts.

## 6. AI Code implementation rules

- **Minimize change footprint.** Never rewrite unrelated files or perform architecture-scale refactor without an approved task.
- Use narrow modules: protocol decoder, pilot session, safety supervisor, flight state machine, motion planner, servo HAL, battery monitor, settings and telemetry.
- Do not import the entire ESP-FLUTTER firmware and rename it. License audit and controlled reuse are mandatory.
- Avoid blocking delays in control loop and network handlers, dynamic allocation in hot path unless justified, unaudited pointer arithmetic and unchecked string-to-float conversion.
- Reject invalid finite/range/type values before motion code. Apply final servo mechanical limit after trim/mix.
- Safety STOP and control link timeout enforced on device, independent of browser. Default boot DISARMED. No stale command replay, no silent automatic re-arm.
- No default public AP password or credentials committed. Access control and pilot ownership enforced on device.
- Check errors on serial, sensor, ADC, NVS and network interfaces; log state transitions, do not flood loop.
- No wiring, charger or regulator advice may bypass 04_HARDWARE_AND_SAFETY.md gates.
- Scope split: code-only unit tests vs real-hardware tests must be clearly labeled; never fabricate measured performance.

## 7. PR submission contract (AI Code -> SA)

Every PR description has:

1. Task ID and approved Design baseline SHA.
2. Summary of implementation and design compliance mapping.
3. Files changed and impact.
4. Build/test commands, raw result PASS/FAIL/NOT RUN, SHA, board/host environment.
5. Control safety evidence: boot, arm, stop, timeout, malformed inputs, sequence replay and recovery.
6. Screenshots or demo for UI changes; logic traces for state-machine changes.
7. Known limitations, open questions and hardware testing restrictions.
8. Rollback instructions and dependency/license/SBOM changes.

Do not state a hardware pass without real hardware evidence. Automated tests do not replace electrical validation.

## 8. SA review workflow and local isolation

When PR is supplied:

1. Confirm repository, PR base/head SHA, files changed and Task/design baseline. Read the latest **approved** 05_AI_WORK_RULES.md plus all relevant docs; do not assume current main always matches task baseline.
2. In an isolated local review workspace, create **ReviewPR/PR-<number>/** from a fresh repository checkout or detached worktree of the exact PR head, using a separate object directory/worktree from AI Code. Example:
   
       git fetch origin pull/<PR>/head:refs/review/pr-<PR>
       git worktree add --detach ReviewPR/PR-<PR> refs/review/pr-<PR>
   
   If this tool session cannot create a local checkout, state that fact and do a read-only GitHub PR diff review **without claiming local test PASS**; preserve requested isolation next time local checkout is possible.
3. Review full diff, all changed source, tests, attack surface, performance risks, licensing, telemetry and failure modes; inspect logs/test artifacts.
4. Run safe tests from the isolated review checkout only when tools and environment support it; do not energize servos or flash a board without hardware authorization.
5. Assign severity per finding: BLOCKER (unsafe/broken core invariants), MAJOR (material requirement missing), MINOR (nonblocking), NIT (style).
6. **Post substantive review on PR** with exact paths/lines and reproduction steps, plus explicit decision label. Never say reviewed without actually posting.
7. Keep PR merge decisions with owner unless delegated.

### Severity and disposition

| Outcome | PR comment/review | ReviewPR local checkout | Task file |
| --- | --- | --- | --- |
| SA_REJECTED / CHANGES_REQUIRED (BLOCKER or MAJOR) | Post actionable findings and required regression proof; request changes | KEEP same PR checkout | KEEP and append revision items with finding IDs and affected files; commit task update |
| SA_APPROVED (no blocking findings) | Post approve/nonblocking notes and any remaining risks | DELETE review checkout after review completed | DELETE the **temporary assigned Task file** in a separate docs/task commit; retain PR link and outcome in design review history |
| SA_BLOCKED (missing evidence/environment) | Post status/what is missing; do NOT approve | KEEP if checkout exists | KEEP until evidence resolves |
| PR abandoned/closed without approval | Post closure context where possible | DELETE after safe cleanup | Archive status/closeout then remove per owner direction |

**Important:** Deleting the assigned task after a nonblocking review implements the owner's requested cleanup. The historical task is preserved in prior Git commits and the PR link/decision log in 06_DECISIONS_AND_ROADMAP.md. Do not delete other agents' tasks.

### Safe cleanup procedure
- Check PR verdict and path identity: only the exact ReviewPR/PR-<number> checkout created by SA is eligible.
- If worktree: use Git-supported worktree remove after confirming no uncommitted evidence; preserve any artifacts needed for audit outside the transient directory.
- Do not recursively delete arbitrary paths, AI Code's workspace, shared build directories or user's files.
- If review is BLOCKER/MAJOR, do **not** delete checkout/task; update the same task with numbered findings, expected changes, evidence and re-review criteria.
- When AI Code pushes a fix, fetch the new PR HEAD into the **same isolated ReviewPR/PR-<number> checkout** safely, compare original findings and retest focused regressions. Re-review, post new comment and then cleanup on SA_APPROVED.
- PR comment posted and task cleanup commit **must both succeed** before declaring SA review complete.

## 9. Rules for decisions, changes and evidence

- Source of truth order: explicit owner approvals > approved Design baseline > current Task contract > implementation comments > reference project. Resolve conflicts through an ADR.
- Record risk changes; don't allow a code patch to silently modify control limits, timeout behavior, battery cutoff or servo PWM.
- Each hypothesis in tuning must be tested against baseline one variable at a time, record negative results and analysis before further tuning.
- Label every result PASS, FAIL, BLOCKED or NOT_RUN with evidence. Never infer PASS from absence of errors.
- Promote a test gate only with full evidence; stop powered testing on any unsafe outcome.
- Store no passwords, private tokens or personal account details in a public repository.

## 10. SA review checklist

- [ ] Task and approved design SHA linked.
- [ ] Changed paths stay within task scope, no unauthorized code or license issues.
- [ ] Builds/tests replicated or limitations clearly reported.
- [ ] Boot/arm/control ownership/STOP/failsafe and exception paths verified.
- [ ] Settings migrations and numeric validation verified.
- [ ] UI shows fresh device-truth and safe state transitions.
- [ ] Power/servo/physical tests not claimed without evidence.
- [ ] GitHub PR comment/review posted with precise verdict.
- [ ] On reject: checkout/task retained and corrections logged.
- [ ] On approve: checkout safely removed, assigned task file removed with commit, audit ledger updated.

**No Task issued before owner approves Design baseline.**