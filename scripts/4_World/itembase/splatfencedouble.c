class SPLATFenceDouble extends SPLATFenceCore
{
    
    override void SetActions()
{
    super.SetActions();
    AddAction(ActionFoldBaseBuildingObject);
}
    
    
    
    override string GetConstructionKitType()
    {
        return "SPLATFenceKitDouble";
    }
}