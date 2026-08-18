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

    //--- VICINITY / TAB-PANEL GATING (bypasses whatever Fence.c would have restricted)
    override bool CanDisplayAttachmentSlot(string slot_name)
    {
        return true;
    }

    override bool CanDisplayAttachmentCategory(string category_name)
    {
        return true;
    }

    override bool CheckSlotVerticalDistance(int slot_id, PlayerBase player)
    {
        return true;
    }

    override bool CanReceiveAttachment(EntityAI attachment, int slotId)
    {
        if (!GetGame().IsMultiplayer() || GetGame().IsClient())
        {
            PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
            if (player)
            {
                ConstructionActionData construction_action_data = player.GetConstructionActionData();
                construction_action_data.SetActionInitiator(NULL);
            }
        }
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

    // Your fence only has one part ("base"), so this checks a single "center" point
    // rather than the reference's three ("center"/"center2"/"center3") — add more
    // memory points and OR them here if you later need a wider check area.
    override bool IsPlayerInside(PlayerBase player, string selection)
    {
        if (!HasProperDistance("center", player))
        {
            return false;
        }
        return true;
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


    override void OnPartBuiltServer(notnull Man player, string part_name, int action_id)
    {
        Print("SPLATFence DEBUG: OnPartBuiltServer CALLED, part_name=" + part_name);
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

    override void EEOnAfterLoad()
    {
        Print("SPLATFence DEBUG: EEOnAfterLoad ENTER, HasBase()=" + HasBase());
        super.EEOnAfterLoad();
        Print("SPLATFence DEBUG: EEOnAfterLoad AFTER super, HasBase()=" + HasBase());
    }

    override void OnCreatePhysics()
    {
        Print("SPLATFence DEBUG: OnCreatePhysics ENTER, HasBase()=" + HasBase());
        super.OnCreatePhysics();
        Print("SPLATFence DEBUG: OnCreatePhysics AFTER super, HasBase()=" + HasBase());
    }

    override void AfterStoreLoad()
    {
        Print("SPLATFence DEBUG: AfterStoreLoad ENTER, HasBase()=" + HasBase());
        super.AfterStoreLoad();
        Print("SPLATFence DEBUG: AfterStoreLoad AFTER super, HasBase()=" + HasBase());
        //UpdatePhysics();
    }

    override void UpdatePhysics()
    {
        Print("SPLATFence DEBUG: UpdatePhysics CALLED, HasBase()=" + HasBase());
        
        ConstructionPart base_part = GetConstruction().GetConstructionPart("base");
        if (base_part)
            Print("SPLATFence DEBUG: base part IsBuilt()=" + base_part.IsBuilt());
        else
            Print("SPLATFence DEBUG: base part NOT FOUND");
        
        super.UpdatePhysics();
        
        Print("SPLATFence DEBUG: UpdatePhysics FINISHED");
    }
}