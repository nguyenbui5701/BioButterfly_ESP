# BioButterfly_ESP — Solution Architecture Design

**Status:** DESIGN_DRAFT (awaiting owner approval).  
**Owner of this folder:** Solution Architect (SA).  
**Baseline date:** 2026-11-23.

This repository starts with design-only documentation. No application firmware, UI implementation, or mechanical CAD is approved by these documents.

## Document map

1. [01_REQUIREMENTS.md](01_REQUIREMENTS.md) — MVP scope, requirements, acceptance criteria, NFRs.
2. [02_UI_UX.md](02_UI_UX.md) — mobile-first web cockpit, state-dependent controls, screen contracts.
3. [03_SYSTEM_ARCHITECTURE.md](03_SYSTEM_ARCHITECTURE.md) — components, control pipeline, safety state machine, interfaces.
4. [04_HARDWARE_AND_SAFETY.md](04_HARDWARE_AND_SAFETY.md) — physical design, power, wiring assumptions and bench gates.
5. [05_AI_WORK_RULES.md](05_AI_WORK_RULES.md) — SA/AI Code workflow, source boundaries, PR review, cleanup rules.
6. [06_DECISIONS_AND_ROADMAP.md](06_DECISIONS_AND_ROADMAP.md) — decisions, design gates, release phases, open points.
7. [07_REFERENCE_AUDIT.md](07_REFERENCE_AUDIT.md) — verified reference-code map, reuse boundaries and evidence.

## Governance

- **SA edits `Design/` only** during design; implementation tasks go into `Task/` **only after design approval**.
- AI Code owns implementation branches. SA never changes that working tree.
- Review checkouts belong in a separate `ReviewPR/` worktree, never the AI Code workspace. The checkout directory is local/transient, not committed.
- On nonblocking review, remove the local review checkout and close/delete the assigned task file **only when the PR lifecycle policy in 05_AI_WORK_RULES.md permits**; historical decisions and Git commits must remain auditable.
- No code reuse from third-party sources until their license and usage conditions are verified.

**Important:** The reference project's Google Drive was provided but its individual contents were not accessible in this review environment. Mechanical drawings, exact wiring, and BOM are **not yet verified**. Any derived dimensions, current ratings, or flight performance figures remain provisional until bench evidence or source documents are inspected.
