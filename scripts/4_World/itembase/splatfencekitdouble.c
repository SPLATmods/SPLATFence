class SPLATFenceKitDouble extends FenceKit
{
    // Deliberately NOT calling super.OnPlacementComplete() here —
    // FenceKit's own version creates a vanilla "Fence", which isn't what we want
    override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
    {
        if ( g_Game.IsServer() )
        {
            SPLATFenceDouble splatfence = SPLATFenceDouble.Cast( g_Game.CreateObjectEx( "SPLATFenceDouble", GetPosition(), ECE_PLACE_ON_SURFACE ) );
            splatfence.SetPosition( position );
            splatfence.SetOrientation( orientation );
            
            HideAllSelections();
            SetIsDeploySound(true);

			GetGame().ObjectDelete(this);
        }	
    }
}