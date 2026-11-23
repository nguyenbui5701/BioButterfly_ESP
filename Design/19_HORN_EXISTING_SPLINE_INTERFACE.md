# 19 — Horn-only continuation: physical spline interface clarification

**Status:** HORN_ONLY_SCOPE_CONFIRMED / ACTUAL_MATING_INTERFACE_PENDING.  
**Date:** 2026-11-24 | **Owner statement:** “phần spline tôi đã có sẵn cơ khí. Chỉ cần tiếp tục triển khai horn nhựa”.

## 1. Interpret owner input precisely; do NOT assume an insert or shaft shape

- The owner has a **physical 25T / nominal Ø4.9mm spline feature** in the available SDS1601 mechanical hardware.
- **Do not design or print a separate male servo output spline**: that output already exists on the servo/hardware.
- Continue with **the horn plastic part** and matching ARMV5 wing-root interface. Owner-confirmed **P7** remains the design source of truth: three collinear horn mounting holes H1/H2/H3; H1–H2 = H2–H3 = **3.5mm**, outer H1–H3 = **7mm**; two bolts through H1+H3 only and two ARMV5 matching holes.
- **Important distinction not yet answered:** Does “physical spline already available” refer **only to the SDS1601's exposed 25T output shaft**, or to a **separate premade mating coupler/adapter/horn hub** mounted to the servo? The owner has not specified which.
  - **Case A, bare servo output shaft:** the printed horn must still have a **female 25T mating interface** within its central hub. We are not printing the male spline as a separate piece, but the negative spline shape in the plastic horn still needs fit validation; 25T/Ø4.9mm alone does not specify tooth shape.
  - **Case B, separate physical mating coupler/adapter:** the printed horn interfaces with that part's **actual bolt/shaft geometry**; a female 25T print may NOT be necessary. The adapter's material, dimensions, fastening method, retention and safe load path must be inspected. DO NOT assume such a part exists.
- **One blocking owner clarification:** “Bạn đang có riêng một đầu nối spline 25T/hub gắn sẵn lên servo, hay chỉ đang nói đến trục 25T vốn có trên servo SDS1601?” Prefer a photo of the actual part/assembly if available. This selects Case A or Case B and is needed to produce the final central horn hub.
- The answer should not trigger re-questioning the already-closed decisions on horn being plastic, P7 pattern, Bambu A1, servo 25T/Ø4.9mm or ESP-NOW.

## 2. Two joints remain mechanically distinct

### J1: Servo hardware -> printed horn

- Preserve original servo retention design and validate positive torque transfer. Avoid friction-only attachment on a smooth round drilled hole.
- **Case A:** horn female 25T cavity and separate center retention screw; small sacrificial printed fit coupon before load.
- **Case B:** horn adapter geometry (flat mounting face, keyed shaft, screws, boss or other as physically evidenced), potentially without printed teeth. No unnecessary spline coupon when all spline engagement is supplied by existing coupler, but verify adapter-to-servo fit and adapter-to-horn fasteners/clearance.

### J2: Printed horn lever -> printed ARMV5 wing root

- Exactly three collinear horn holes; two holes on ARMV5.
- Installed bolts: H1 and H3; H2 is unused. H1–H2 = H2–H3 = 3.5mm and H1–H3 = 7.0mm **owner-approved**.
- **Provisional candidate:** 2× M1.6 machine bolts per wing, Ø1.8mm nominal through clearance. Actual printer hole shrink, screw heads, nut retention, arm thickness, fastener length and root strength remain test gates.
- Parametric coordinates with servo shaft axis O: H1 at `(0, -r)`, H2 `(0, -(r+3.5))`, H3 `(0, -(r+7.0))`. The absolute offset `r` is NOT approved and must clear the actual J1 coupler/hub/screw head as well as outer tip/edge margin.
- The STL `SG90_one_horn_without_holes.stl` is the **shape reference** only; its stepped round central bore is not proof of fit to either J1 variant. `ARMV5(1).STL` contains two components and must preserve distinct mirrored assemblies.

## 3. Scope and actions

**Allowed at DESIGN phase:** inspect STL geometry, define parametric construction, safety/fit gates, keep the P7 hole pitch as an immutable shared horn–ARM contract, propose print coupons and ask for the J1 physical interface.

**Not yet approved:** final printer-ready horn, production female spline or adapter pocket, powered motion, drilling/forcing servo shaft, creating Task/ before approved design. CAD/STEP/STL can be produced once J1 physical mating is understood and owner/SA authorizes the implementation route.

**Do not silently switch to a metal horn:** if a separate hardware coupler exists, using it is a source-input clarification, not permission to redesign the user's all-plastic lever choice. Clarify whether that coupler provides the spline interface.

## 4. Minimal next intake

**One question only:** Is it **A: the exposed 25T toothed shaft on the SDS1601 itself**, or **B: a distinct already-owned spline coupler/adaptor/metal hub**? Photo of the actual physical mating part resolves it best.

**Status:** HORN_P7_GEOMETRY_CONFIRMED / J1_CENTRAL_HUB_TYPE_PENDING / NO_NEW_STL / NO_TASK.
