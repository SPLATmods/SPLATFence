class SPLATFenceCore extends BaseBuildingBase
{
    const float MAX_ACTION_DETECTION_ANGLE_RAD = 1.3; // ~75 degrees

    //--- CONSTRUCTION KIT
    override ItemBase CreateConstructionKit()
    {
        ItemBase construction_kit = ItemBase.Cast(GetGame().CreateObject(GetConstructionKitType(), GetKitSpawnPosition()));
        if (m_ConstructionKitHealth > 0)
        {
            construction_kit.SetHealth(m_ConstructionKitHealth);
        }
        return construction_kit;
    }

    override ItemBase FoldBaseBuildingObject()
    {
        ItemBase item = CreateConstructionKit();
        DestroyConstruction();
        return item;
    }

    override vector GetKitSpawnPosition()
    {
        bool exists = MemoryPointExists("kit_spawn_position");
        //Print("SPLATFence DEBUG: kit_spawn_position exists=" + exists);

        if (exists)
        {
            vector position = GetMemoryPointPos("kit_spawn_position");
            //Print("SPLATFence DEBUG: raw local point=" + position + " world=" + ModelToWorld(position));
            return ModelToWorld(position);
        }

        //Print("SPLATFence DEBUG: falling back to GetPosition()=" + GetPosition());
        return GetPosition();
    }

    override int GetMeleeTargetType()
    {
        return EMeleeTargetType.NONALIGNABLE;
    }

    //--- VICINITY / TAB-PANEL + LOOK-AND-ATTACH GATING
    //
    // Fence-stage materials (stage-2 logs / planks / nails / rope) stay hidden and
    // unattachable until HasBase() - i.e. until the log base frame is built. This
    // mirrors vanilla Fence.c, which hides its "Material" category behind HasBase().
    // HasBase() flips to true when the is_base=1 "base" part is built and is
    // net-synced to clients by BaseBuildingBase (RegisterNetSyncVariableBool).
    //
    // Slot names, per config.cpp:
    //   Material_WoodenLogs         -> base stage (2 logs, builds "base", is_base=1)
    //   SPLAT_Material_WoodenLogs   -> fence stage
    //   SPLAT_Material_WoodenPlanks -> fence stage
    //   SPLAT_Material_Nails        -> fence stage
    //   Material_FPole_Rope         -> fence stage

    // NOTE: the int overload is the live one. The old string overload of this
    // method is obsolete - the engine logs a warning and no longer dispatches to
    // it (see EntityAI.CanDisplayAttachmentSlot). ActionAttachToConstruction ->
    // ConstructionActionData.GetAttachmentSlotFromSelection also checks THIS
    // overload, so one method covers both the tab panel and the look-and-attach
    // prompt.
    override bool CanDisplayAttachmentSlot(int slot_id)
    {
        if (!super.CanDisplayAttachmentSlot(slot_id))
            return false;

        if (!HasBase() && InventorySlots.GetSlotName(slot_id) != "Material_WoodenLogs")
            return false;

        return true;
    }

    override bool CanDisplayAttachmentCategory(string category_name)
    {
        if (!super.CanDisplayAttachmentCategory(category_name))
            return false;

        // "Material" is the GUIInventoryAttachmentsProps CLASS name for the
        // planks/nails/rope/stage-2-logs group - hide the whole header until the
        // base is up.
        if (category_name == "Material" && !HasBase())
            return false;

        return true;
    }

    // Model has no real Geometry LOD, so keep the vertical-distance check
    // permissive. This is a separate model-limitation workaround, not part of the
    // stage gating above.
    override bool CheckSlotVerticalDistance(int slot_id, PlayerBase player)
    {
        return true;
    }

    // Authoritative attach gate (inventory drag-drop + server-side validation).
    override bool CanReceiveAttachment(EntityAI attachment, int slotId)
    {
        if (!super.CanReceiveAttachment(attachment, slotId))
            return false;

        //manage construction action initiator (vanilla Fence idiom)
        if (!GetGame().IsMultiplayer() || GetGame().IsClient())
        {
            PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
            if (player)
            {
                ConstructionActionData construction_action_data = player.GetConstructionActionData();
                construction_action_data.SetActionInitiator(NULL);
            }
        }

        // Before the base is built, nothing but base-stage logs can be attached -
        // blocks look-and-attach AND inventory drag-drop of nails/planks/rope.
        if (!HasBase() && InventorySlots.GetSlotName(slotId) != "Material_WoodenLogs")
            return false;

        return true;
    }

    override bool CanPutIntoHands(EntityAI parent)
    {
        return false;
    }

    override bool CanBeRepairedToPristine()
    {
        return true;
    }

    // --- HARD-SIDE NAME LABEL ("Indestructible Wall", no action prompt)
    //
    // ItemBase.IsActionTargetVisible() -> ActionTargetsCursor.GetTarget() shows
    // this object's name in the crosshair widget with zero registered actions
    // (its doc comment: "cases where we want to show object widget which cant be
    // taken to hands"; vanilla precedent PowerGeneratorStatic). IsTakeable()=false
    // is inherited from BaseBuildingBase.
    //
    // This is only ever called once ActionTargetsCursor.FindActionTarget() has
    // already raycast onto this wall's action geo, so "crosshair is on the wall"
    // is a given. All we add: only show the label from the HARD side - camera on
    // the +GetDirection() side of the wall (GetDirection() points outward from
    // the hard face, same convention IsFacingCamera uses). XZ only.
    //
    // An earlier version keyed off IsFacingCamera(), which also fired when
    // looking at the ground *behind* the wall. A version after that raycast the
    // camera ray against a dedicated "hardside" View-LOD selection - dropped as
    // redundant once this camera-side check was in.
    //
    // Do NOT implement as a dummy HasTarget()=false action: that hides the prompt
    // on the crosshair widget but is exactly what makes the bottom-screen
    // ItemActionsWidget show its own "press F" prompt.
    override bool IsActionTargetVisible()
    {
        vector cam_offset = GetGame().GetCurrentCameraPosition() - GetPosition();
        cam_offset[1] = 0;
        vector fwd = GetDirection();
        fwd[1] = 0;
        return vector.Dot(cam_offset, fwd) > 0;
    }

    //--- DIRECT LOOK-AND-ATTACH / FACING CHECKS
    override bool IsFacingPlayer(PlayerBase player, string selection)
    {
        vector ref_dir = GetDirection();
        vector fence_player_dir = player.GetDirection();
        fence_player_dir.Normalize();
        fence_player_dir[1] = 0;
        ref_dir.Normalize();
        ref_dir[1] = 0;

        if (ref_dir.Length() != 0)
        {
            float angle = Math.Acos(fence_player_dir * ref_dir);
            if (angle >= MAX_ACTION_DETECTION_ANGLE_RAD)
                return true;
        }
        return false;
    }

    override bool IsFacingCamera(string selection)
    {
        vector ref_dir = GetDirection();
        vector cam_dir = GetGame().GetCurrentCameraDirection();
        ref_dir.Normalize();
        ref_dir[1] = 0;
        cam_dir.Normalize();
        cam_dir[1] = 0;

        if (ref_dir.Length() != 0)
        {
            float angle = Math.Acos(cam_dir * ref_dir);
            if (angle >= MAX_ACTION_DETECTION_ANGLE_RAD)
                return true;
        }
        return false;
    }

    // Player counts as "inside" (can open the tab panel / attach / dismantle
    // materials) if they're close to EITHER:
    //   "center"     - a point on the wall itself, and
    //   "center_low" - a ground-level point added below the wall, so a kit placed
    //                  high on top of another wall can still be worked on from the
    //                  ground. Only consulted if the model actually has the point.
    // Both use HasProperDistance()'s 1.4 m check. Add more points and OR them in
    // here if a wider reach is needed.
    override bool IsPlayerInside(PlayerBase player, string selection)
    {
        if (HasProperDistance("center", player))
            return true;

        if (MemoryPointExists("center_low") && HasProperDistance("center_low", player))
            return true;

        return false;
    }

    override bool HasProperDistance(string selection, PlayerBase player)
    {
        if (MemoryPointExists(selection))
        {
            vector selection_pos = ModelToWorld(GetMemoryPointPos(selection));
            float distance = vector.Distance(selection_pos, player.GetPosition());
            if (distance >= 1.4)
            {
                return false;
            }
        }
        return true;
    }

    override bool CheckMemoryPointVerticalDistance(float max_dist, string selection, PlayerBase player)
    {
        if (player)
        {
            vector player_pos = player.GetPosition();
            vector pos;
            if (MemoryPointExists(selection))
            {
                pos = ModelToWorld(GetMemoryPointPos(selection));
            }
            if (Math.AbsFloat(player_pos[1] - pos[1]) <= max_dist)
            {
                return true;
            }
            return false;
        }
        return true;
    }


    //--- PHYSICS REGISTRATION (vanilla pattern)
    // BaseBuildingBase registers a built part's collision only on a not-built -> built
    // TRANSITION: SetPartFromSyncData -> ShowConstructionPartPhysics -> AddProxyPhysics.
    // If that transition is consumed before the entity has a physics body the call is
    // discarded, and the part is already flagged built, so it can never fire again.
    //
    // Vanilla does not re-assert physics to recover from that - it RE-ARMS the transition.
    // ConstructionInit() -> Construction.Init() -> UpdateConstructionParts() does
    // m_ConstructionParts.Clear() and rebuilds every part from config flagged not-built,
    // so the following SetPartsAfterStoreLoad() replays it. That is the whole of vanilla's
    // recovery (BaseBuildingBase.OnCreatePhysics); Fence.c adds nothing on top of it, and
    // neither do the Building Fortifications barricades.
    //
    // The same re-arm is driven here from all three points at which the built state can
    // first become known - store load, physics creation, client sync - and retried a
    // bounded number of times, so the outcome does not depend on the order those arrive
    // in. That order is what differs between a LAN server and a hosted one.
    const int SPLAT_REARM_ATTEMPTS = 3;
    const int SPLAT_REARM_INTERVAL = 1000;

    protected int  m_SPLATRearmCount;
    protected bool m_SPLATRearmScheduled;

    protected string SPLATStateDbg()
    {
        string baseBuilt = "n/a";
        Construction cons = GetConstruction();
        if (cons)
        {
            baseBuilt = "" + cons.IsPartConstructed("base");
        }

        return " server=" + g_Game.IsServer() + " hasBase=" + HasBase() + " baseBuilt=" + baseBuilt;
    }

    protected void SPLATScheduleRearm(int delay)
    {
        if (m_SPLATRearmScheduled || m_SPLATRearmCount >= SPLAT_REARM_ATTEMPTS)
        {
            return;
        }

        m_SPLATRearmScheduled = true;
        g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SPLATRearmConstruction, delay, false);
    }

    protected void SPLATRearmConstruction()
    {
        m_SPLATRearmScheduled = false;

        if (!HasBase() || m_SPLATRearmCount >= SPLAT_REARM_ATTEMPTS)
        {
            return;
        }

        m_SPLATRearmCount++;

        // Drop anything an earlier pass registered before replaying. The transition path
        // and Construction.UpdatePhysics both only ever Add, never Remove, so without this
        // a repeat would stack proxies on the same selection.
        map<string, ref ConstructionPart> parts = GetConstruction().GetConstructionParts();
        for (int i = 0; i < parts.Count(); ++i)
        {
            RemoveProxyPhysics(parts.GetKey(i));
        }

        ConstructionInit();
        SetPartsAfterStoreLoad();
        UpdateVisuals();

        Print("[SPLATFence] rearm " + m_SPLATRearmCount + "/" + SPLAT_REARM_ATTEMPTS + SPLATStateDbg());

        if (m_SPLATRearmCount < SPLAT_REARM_ATTEMPTS)
        {
            SPLATScheduleRearm(SPLAT_REARM_INTERVAL);
        }
    }

    //--- front 1: store load - server side, after a restart
    override void AfterStoreLoad()
    {
        super.AfterStoreLoad();
        Print("[SPLATFence] AfterStoreLoad" + SPLATStateDbg());
        SPLATScheduleRearm(200);
    }

    //--- front 2: physics creation - both sides, and where vanilla itself recovers
    override void OnCreatePhysics()
    {
        super.OnCreatePhysics();
        Print("[SPLATFence] OnCreatePhysics" + SPLATStateDbg());
        SPLATRearmConstruction();
    }

    //--- front 3: sync arriving on the client - covers both build and stream-in
    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();
        SPLATScheduleRearm(300);
    }

    override void EEOnAfterLoad()
    {
        super.EEOnAfterLoad();
        Print("[SPLATFence] EEOnAfterLoad" + SPLATStateDbg());
        SPLATScheduleRearm(500);
    }


    override void OnPartBuiltServer(notnull Man player, string part_name, int action_id)
    {
        super.OnPartBuiltServer(player, part_name, action_id);
        UpdateVisuals();
        
        //if is_base is set to 0 in config then uncomment this
        // Drop a folded kit as a byproduct the moment the wall completes
        // if (GetGame().IsServer())
        // {
        //     ItemBase kit = ItemBase.Cast(GetGame().CreateObjectEx(GetConstructionKitType(), GetKitSpawnPosition(), ECE_PLACE_ON_SURFACE));
        // }
    }


    override void OnPartDismantledServer(notnull Man player, string part_name, int action_id)
    {
        super.OnPartDismantledServer(player, part_name, action_id);
        UpdateVisuals();

        //If is_base is set to 0 in config then uncomment this
        // if (GetGame().IsServer() && !HasBase())
        // {
        //     GetGame().ObjectDelete(this);
        // }
    }

    override void OnPartDismantledClient(string part_name, int action_id)
    {
        super.OnPartDismantledClient(part_name, action_id);
        SoundDismantleStart(part_name);
    }
}