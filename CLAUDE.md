# SPLATFence (1.30 experimental copy)

An **indestructible base-building wall** ("Indestructible Wall" / kit), plus
custom high-stack material slots.

**This is a separate copy for 1.30 experimental testing, not the canonical
mod.** It lives at `P:\SPLATMod Files Exp\SPLATFence\` (this folder's own
`.git` repo) — distinguished from the canonical mod purely by **filesystem
location**, one level under `SPLATMod Files Exp\`. Everything inside it —
`CfgPatches`/`CfgMods` class name, `dir=`, every `\SPLATFence\data\...` path,
item/script class names — is intentionally identical to the canonical mod
(reverted back from an earlier `SPLATFenceExp` naming attempt; see the
`$PBOPREFIX$.txt` gotcha below for why that rename briefly happened and why
it was reverted).

**Known consequence of the identical naming:** `@SPLATFence` (canonical) and
whatever this folder packs to **must never be loaded on the same server
process** — both declare `CfgPatches > class SPLATFence`, `CfgMods > class
SPLATFence`, and `CfgVehicles > class SPLATFenceDouble/SPLATFenceKitDouble`,
which will conflict/duplicate if both PBOs are present in one `-mod=` list.
This copy is only ever run on the separate 1.30 experimental server.

Canonical mod: `github.com/SPLATmods/SPLATFence`, branch `main`, at
`../../../SPLATMods Files/SPLATFence/` (1.29 workspace) — has its own
[CLAUDE.md](../../../SPLATMods%20Files/SPLATFence/CLAUDE.md), parent context
[`../CLAUDE.md`](../CLAUDE.md), [`../../CLAUDE.md`](../../CLAUDE.md). There is
also a sibling folder `../SPLATFence IDEK/` (1.29 workspace) — a **dead old
experiment**. Do not reference it, compare against it, or copy from it unless
SPLAT explicitly asks.

## Layout

```
config.cpp                      CfgPatches, CfgMods (world only), CfgSlots, CfgVehicles, CfgNonAIVehicles (proxies)
data/models/model.cfg           skeletons + CfgModels hide-animations for SPLATFenceDouble.p3d (text — OK to read)
data/models/*.p3d, data/textures/*   binary — don't parse
scripts/4_World/itembase/
  splatfencecore.c              class SPLATFenceCore extends BaseBuildingBase — the bulk of the logic
  splatfencedouble.c            class SPLATFenceDouble extends SPLATFenceCore — SetActions + GetConstructionKitType
  splatfencekitdouble.c         class SPLATFenceKitDouble extends FenceKit — OnPlacementComplete spawns SPLATFenceDouble
scripts/4_World/hologram/
  splatfencehologram.c          modded Hologram — loosens placement height/range for SPLATFenceKitDouble only
scripts/4_World/recipies/       (note: "recipies" spelling)
  CraftSPLATFenceDouble.c       class CrafSPLATFenceDouble extends RecipeBase  (note: "Craf" typo in classname)
  PluginRecipesManager.c        modded PluginRecipesManager — RegisterRecipe(new CrafSPLATFenceDouble)
```

`requiredAddons[]`: `DZ_Data`, `DZ_Scripts`, `DZ_Gear_Consumables` (WoodenLog/
WoodenPlank/Nail), `DZ_Gear_Camping` (FenceKit, BaseBuildingBase, fence_kit.p3d),
`DZ_Gear_Crafting` (Rope). `CfgPatches > units[]` = `SPLATFenceDouble`,
`SPLATFenceKitDouble`.

## config.cpp mechanics

- **`CfgSlots`**: `Slot_SPLAT_Material_WoodenLogs/Planks/Nails` extend the vanilla
  `Slot_Material_*` with a big `stackMax` (20/55/99) so one slot holds a full wall's
  worth of material.
- **`CfgVehicles` class patching**: reopens vanilla `WoodenLog`, `WoodenPlank`,
  `Nail` (`: Inventory_Base`) and does `inventorySlot[]+={ "SPLAT_Material_..." }`
  so those items can attach to the fence's custom slots.
- **`SPLATFenceCore : BaseBuildingBase`** (empty config class; `SPLATFenceDouble :
  SPLATFenceCore`). Wall is `hitpoints=1000000` **and** every `GlobalArmor` /
  `DamageZones` `ArmorType` damage coef is `0` (or `0.000001`) — indestructible
  belt-and-braces.
- **`class Construction`**: single stage `wall` with parts `base` (is_base=1,
  2 logs, hammer) → `fence` (20 logs + 50 planks + 99 nails + rope, hammer).
  `build_action_type`/`dismantle_action_type` are **bitmasks** — see parent
  CLAUDE.md §5 for vanilla tool values. `fence` build/dismantle = `2` (Hammer /
  Crowbar+MeatTenderizer). Rope material `quantity=0` + `lockable=1` (attached
  rope reports `GetQuantity()==0`; lockable keeps it in the wall).
- `SPLATFenceKitDouble : FenceKit` uses the vanilla `fence_kit.p3d` model;
  `SPLATFenceKitDoublePlacing : FenceKit` (`scope=1`) is the hologram.
- **Material proxies** — every slot in `attachments[]` gets a
  `CfgNonAIVehicles > Proxy<x>: ProxyAttachment { scope=1; inventorySlot="<slot>";
  model="\SPLATFence\data\models\proxy\<File>.p3d"; }`. The engine's ProxyInventory
  sim shows the proxy whenever the slot is occupied. **Path casing is exact and
  case-sensitive in-engine** (Windows hides mismatches): the file on disk, `model=`
  here, and the proxy reference baked into `SPLATFenceDouble.p3d` must all match
  character-for-character, or the proxy silently fails to resolve. A dangling
  `model=` can drop the whole proxy set.
  - **Non-lockable slots** (`SPLAT_Material_WoodenLogs/WoodenPlanks/Nails`) are
    *consumed* on build → slot empties → engine drops the proxy automatically. No
    model.cfg work.
  - **Lockable slots** (`Material_WoodenLogs` base, `Material_FPole_Rope` fence) are
    *locked in place* on build → item stays attached → engine keeps drawing the
    proxy. To hide it you must add the vanilla hide-anim, keyed on the slot name:
    `AnimationSources` `class <slot>: AnimSourceHidden {}` +
    `SPLATFenceDouble_skeleton > SkeletonBones[]` `"<slot>", ""` +
    `CfgModels > SPLATFenceDouble > sections[]` `"<slot>"` + a `type="hide"` anim +
    a selection named `<slot>` in the `.p3d`. `UpdateAttachmentVisuals` then fires
    `SetAnimationPhase(<slot>, 1)` when the slot locks. Both are wired.
  - **Gotcha:** `isDiscrete=1` skeleton — a `SkeletonBones[]` name with no matching
    (non-empty) `.p3d` selection fails the whole skeleton → **every** proxy vanishes.
    Add bones one at a time; if proxies disappear, the last name added is the bad one.
  - `SPLAT_Material_WoodenPlanks` (slot `stackMax` 55, part needs 50) can leave a
    5-plank remainder keeping that slot occupied → proxy lingers. If that bites, add
    a `SPLATFenceCore.UpdateVisuals()` override forcing phase 1 once
    `GetConstruction().IsPartConstructed("fence")`, or drop the leftover in script.

## Script mechanics (`splatfencecore.c`)

- **Kit round-trip**: `CreateConstructionKit` / `FoldBaseBuildingObject` /
  `GetKitSpawnPosition` (uses model memory point `kit_spawn_position`, falls back
  to `GetPosition()`). `SPLATFenceDouble.SetActions()` adds
  `ActionFoldBaseBuildingObject`.
- **`SPLATFenceKitDouble.OnPlacementComplete`** deliberately does **not** call
  `super` (vanilla `FenceKit` would spawn a plain `Fence`); instead it
  `CreateObjectEx("SPLATFenceDouble", ..., ECE_PLACE_ON_SURFACE)`, sets
  position/orientation, `HideAllSelections()`, `SetIsDeploySound(true)`,
  `ObjectDelete(this)` — server only.
- **Stage-gated attach panel** — `CanDisplayAttachmentSlot(int slot_id)`,
  `CanDisplayAttachmentCategory(string)` and `CanReceiveAttachment(EntityAI,int)`
  all hide/block the fence-stage slots (`SPLAT_Material_WoodenLogs/Nails/WoodenPlanks`,
  `Material_FPole_Rope`) and the `Material` GUI category until `HasBase()` is true
  (base log frame built). Only `Material_WoodenLogs` (base stage) shows before that.
  Mirrors vanilla `Fence.c`. `CanDisplayAttachmentSlot(int)` also gates the
  look-and-attach prompt (`ActionAttachToConstruction` →
  `ConstructionActionData.GetAttachmentSlotFromSelection`). **The obsolete
  `string slot_name` overload is not used by the engine** — must override the
  `int slot_id` one. `CanReceiveAttachment` calls `super` and still clears the
  `ConstructionActionData` initiator (vanilla Fence idiom).
- **Model-limitation workarounds (still permissive)** — `CheckSlotVerticalDistance`
  → true (no real Geometry LOD), `IsFacingPlayer`/`IsFacingCamera` (angle vs
  `MAX_ACTION_DETECTION_ANGLE_RAD` 1.3 rad ≈ 75°),
  `HasProperDistance`/`CheckMemoryPointVerticalDistance` (1.4 m point check).
  `CanPutIntoHands` → false; `GetMeleeTargetType` → `NONALIGNABLE`.
- **`IsPlayerInside`** — player is "inside" (tab panel / attach / dismantle
  materials) if within 1.4 m (`HasProperDistance`) of **`center`** (a point on
  the wall) **or** **`center_low`** (an optional ground-level point below the
  wall so a kit placed high on top of another wall can still be worked on from
  the ground). The `center_low` branch is `MemoryPointExists`-guarded — no-op if
  the model doesn't have that point.
  *Known limit:* even with `IsPlayerInside` passing, a wall placed high enough
  that no part of it is within ~3 m of the player still won't show in the tab
  panel — `VicinityItemManager` (client GUI) never scans far enough to add it,
  and a `modded VicinityItemManager` fix was tried and reverted as more trouble
  than it was worth.
- **Proxy-physics re-arm** (the "persistance bug" workaround): `BaseBuildingBase`
  registers a built part's collision only on a *not-built → built transition*
  (`SetPartFromSyncData → ShowConstructionPartPhysics → AddProxyPhysics`). On a
  dedicated server that transition can be consumed before the entity has a
  physics body — but `AddToConstructedParts()` (which flips
  `ConstructionPart.m_IsBuilt`) runs unconditionally just before it, so the part
  is flagged built either way and that transition can never fire again for the
  rest of the object's life. True in both 1.29 and 1.30 — vanilla `Fence.c`
  itself carries no workaround for it, so this is a long-standing base-game
  quirk SPLATFence compensates for, not something BI fixed.
  **1.30 build 126965 rework (current):** `SPLATRearmConstruction()`
  (`splatfencecore.c`) no longer tries to re-arm the transition at all — it
  forces physics **directly from each part's current `IsBuilt()` state**
  (`RemoveProxyPhysics` then, only if `part.IsBuilt()`, `Construction.
  ShowConstructionPartPhysics`), which sidesteps the transition-only-fires-once
  problem entirely: `IsBuilt()` is already set correctly from persisted sync
  data by `SetPartsAfterStoreLoad()`/`OnStoreLoad()` regardless of whether the
  original `AddProxyPhysics` call silently failed. Still driven from all four
  points where built-state first becomes known — `AfterStoreLoad` (200 ms),
  `OnCreatePhysics` (immediate), `OnVariablesSynchronized` (300 ms),
  `EEOnAfterLoad` (500 ms) — bounded to `SPLAT_REARM_ATTEMPTS = 3`,
  `SPLAT_REARM_INTERVAL = 1000` ms, via `CALL_CATEGORY_GAMEPLAY` `CallLater`.
  `SPLATStateDbg()` logs `server=/hasBase=/baseBuilt=`.
  **History:** an earlier 1.30 fix attempt swapped the old `ConstructionInit()`
  call (now a no-op — that method moved to `EntityAI`
  (`3_game/entities/entityai.c:3441`) and is guarded by
  `if (!GetConstructionBasic())`, only running the first time the construction
  component is ever created) for `GetConstruction().Init()`. That call is
  legitimate and unconditional, but did **not** resolve the bug in testing —
  it still depended on the same fragile transition mechanism, just re-armed a
  different way. The direct `IsBuilt()`-based approach above replaced it.
- `OnPartBuiltServer`/`OnPartDismantledServer` → `UpdateVisuals()`; the
  drop-a-kit-on-complete and delete-on-no-base branches are commented out
  (relevant only if `is_base` is set to 0).
- **Hard-side "Indestructible Wall" name label** — `override IsActionTargetVisible()`.
  `ItemBase.IsActionTargetVisible()` exists specifically for "cases where we
  want to show object widget which cant be taken to hands" (its own doc
  comment) — `ActionTargetsCursor.GetTarget()` reads it every frame and shows
  the crosshair name widget with **zero registered actions**, so there is no
  "Hold F" prompt anywhere. Vanilla precedent: `PowerGeneratorStatic` does the
  same pairing (`IsTakeable()=false` + `IsActionTargetVisible()=true`); the
  `IsTakeable()=false` half is already inherited from `BaseBuildingBase`.
  The override is only *called* once `ActionTargetsCursor.FindActionTarget()`
  has already raycast onto this wall's action geo, so "crosshair is on the wall"
  is a given. It adds one check: camera on the hard side —
  `vector.Dot(camPos − GetPosition(), GetDirection()) > 0` (XZ only;
  `GetDirection()` points outward from the hard face). Use `vector.Dot`, not the
  bare `*` operator (ambiguous vector-vs-scalar → `incompatible parameter`).
  Earlier iterations: keyed off `IsFacingCamera()` (fired when looking at the
  ground *behind* the wall); then raycast a dedicated `hardside` View-LOD
  selection (dropped as redundant once the camera-side check was in — no model
  selection needed).
  **Do not implement this as a real action** (a first pass tried a dummy
  `HasTarget()=false` interact action): the crosshair widget hides its prompt
  when `HasTarget()` is false, but the *separate* bottom-of-screen
  `ItemActionsWidget` shows **its own** prompt exactly when `HasTarget()` is
  false — a dummy action always leaks a prompt into one widget or the other.
  Not gated on `HasBase()` — the wall is indestructible from the moment it
  exists; add `&& HasBase()` if only the built wall should carry the label.

## Placement — `modded class Hologram` (`splatfencehologram.c`)

Lets you stand in front of an existing wall, look up at its top, and place a
kit up there (vertical stacking). Every override is gated on
`SPLATFence_IsKit()` (`m_Parent.IsKindOf("SPLATFenceKitDouble")`) and `super`s
for everything else — no other deployable is touched. `Hologram` is a single
global class instantiated hardcoded in `PlayerBase` (`new Hologram(this,…,item)`),
so `modded class` + a type gate is the only route.

Three vanilla limits, all in `scripts/4_world/classes/hologram.c` (build 124708):

| Override | Vanilla limit removed | Notes |
|---|---|---|
| `HeightPlacementCheck()` → `true` | `\|playerY − projectionY\| > 1.5` (`DEFAULT_MAX_PLACEMENT_HEIGHT_DIFF`) | runs client **and** server; same modded class covers both |
| `IsClippingRoof()` — faithful vanilla copy minus the `b1` clause | `projectionY > cameraY` ("can't place above eye level") | keeps the real roof check (`b2` / `IsUnderRoofEx`). Client-only, but the deploy action's `ActionCondition` gates on the client hologram's `IsColliding()` |
| `GetProjectionEntityPosition()` — faithful vanilla copy, two changes | (1) ghost was clamped to ~2–3 m from player ("nose to the wall"); (2) ghost stayed visible (red, floating) when aiming at open air | (1) `maxProjectionDistance` = `SPLATFENCE_MAX_PROJECTION_DISTANCE` (global const, **6 m**) instead of `min(projectionRadius*2, 6)`; `minProjectionDistance` left as vanilla. (2) if the placement raycast hits **nothing at all** (no terrain, no object), return `"0 0 0"` — projection parks at map origin (not rendered) and `IsCollidingZeroPos()` blocks placement. Same trick vanilla `HideWhenClose()` uses for looking straight up. |

**Not touched:** `IsBaseViable()` / `IsBaseStatic()` / `IsBaseFlat()` (the
4-corner "solid flat static ground underneath" check). Client-only on MP
servers. If stacking on the wall top trips the red ghost, add an
`IsBaseViable()` override here too (return `true` for the kit).

The "hide on air" check keys off the **bool return** of `DayZPhysics.RaycastRV`
(true = hit terrain OR object), **not** `hitObjects.Count()` — `hitObjects` is a
`set<Object>` and never contains terrain, so counting it treats bare ground as
"air". If aiming *at* the wall top ever fails to register a hit at all (the wall
model would need fire geometry the ray can catch), the ghost would wrongly hide.

The two reimplemented methods are **faithful copies of vanilla 124708** with one
value changed each — re-sync if BI edits the originals in a future build.

## Recipe

`CrafSPLATFenceDouble` (sic): ingredient 0 = `Rope` (destroyed), ingredient 1 =
`WoodenStick` **or** `Ammo_SharpStick` (min qty 2, `-2` on craft) → result
`SPLATFenceKitDouble` on the ground, inherits averaged ingredient health.
`CanDo` rejects if the ingredients are attachments. Registered in the modded
`PluginRecipesManager`.

## Gotchas

- Class-name typos are load-bearing (already shipped): `CrafSPLATFenceDouble`
  (missing `t`), folder `recipies`. Don't "fix" without updating every reference.
- `model.cfg` selection name `base` is also the wall proxy selection in the
  `.p3d`; renaming the construction part needs a model edit.
- Expect harmless "bone could not be found" RPT warnings for selections the
  current `.p3d` doesn't have.
- **This folder HAS a `$PBOPREFIX$.txt`** containing `SPLATFence` — added
  2026-09-17, still required. Unlike canonical `SPLATFence` (which sits one
  level deep, directly at `P:\SPLATFence\`, so the pack tool's default
  folder-name-derived prefix already matches), this experimental copy sits
  **two levels deep** at `P:\SPLATMod Files Exp\SPLATFence\`. Without an
  explicit prefix, most pack tools derive the in-PBO root from the full path
  under `P:\`, which would make it `SPLATMod Files Exp\SPLATFence` instead of
  `SPLATFence` — and since every path in `config.cpp`/the `.rvmat`s/the
  imageset is `\SPLATFence\data\...`, that mismatch would break every asset
  reference in the mod at once. **Do not delete this file or move the
  folder without updating it.**
- **Naming history (2026-09-17):** this folder was briefly renamed
  `SPLATFenceExp` (class names, `dir=`, every path) to make it self-evidently
  distinct from the canonical mod while iterating on 1.30 base-building
  changes. That surfaced a *second*, more serious problem on top of the
  `$PBOPREFIX$.txt` one above: `SPLATFenceDouble.p3d` has **proxy/LOD
  references baked into the binary model** (from when it was originally
  exported in Object Builder under `P:\SPLATFence\`) pointing at
  `splatfence\data\models\SPLATFenceDouble_Base.p3d`,
  `..._Deployed.p3d`, and `SPLATFenceDoubleKit_deployed.p3d` — confirmed via
  RPT `Cannot open object splatfence\data\models\...` errors. Renaming the
  mod's own paths to `SPLATFenceExp` didn't and couldn't fix that (it's
  compiled into the `.p3d`, not text config), so the built wall/kit rendered
  and collided as nothing (script logic still worked — dismantle still fired
  on an invisible, walk-through-able wall — because that's driven by
  persisted entity state, not the model). Fixing it properly means reopening
  `SPLATFenceDouble.p3d` in Object Builder and repointing those three proxy
  paths. Reverting the whole mod's naming back to `SPLATFence` (this state)
  was the faster fix — it makes the mod's own paths match what's already
  baked into the model, at the cost of the naming collision documented above.
  **If `SPLATFenceDouble.p3d` (or `SPLATFenceKitDouble`'s models) is ever
  re-exported from Object Builder with corrected/relative proxy paths, this
  whole folder (and its `$PBOPREFIX$.txt`) can safely be renamed to
  `SPLATFenceExp` again** to remove the naming collision with canonical
  `SPLATFence` — until then, keep it named `SPLATFence`.
