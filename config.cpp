class CfgPatches
{
	class SPLATFence{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = 
		{
			"DZ_Data",
			"DZ_Scripts",
			"DZ_Gear_Consumables"
		};
	};
};

class CfgMods
{	
	class SPLATFence
	{
		dir="SPLATFence";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="SPLATfence";
		credits="SPLAT";
		author="SPLAT";
		authorID="0";
		version="1.0";
		extra=0;
		type="mod";
		dependencies[]=
		{
			"World"
		};
		class defs
		{
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"SPLATFence/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"SPLATFence/scripts/5_Mission"
				};
			};
		};
	};
};

//Custom slots to set max slot quanitiy
class CfgSlots
{
	class Slot_Material_WoodenLogs;
	class Slot_SPLAT_Material_WoodenLogs : Slot_Material_WoodenLogs
	{
		name = "SPLAT_Material_WoodenLogs";
		stackMax = 20;
	};

	class Slot_Material_WoodenPlanks;
	class Slot_SPLAT_Material_WoodenPlanks : Slot_Material_WoodenPlanks
	{
		name = "SPLAT_Material_WoodenPlanks";
		stackMax = 55;
	};



}

class CfgVehicles
{
	//Add custom slot to the item so that it allows the item in the slot
	class Inventory_Base;
	class WoodenLog: Inventory_Base
	{
		
		inventorySlot[]+=
		{
			"SPLAT_Material_WoodenLogs"
		};
		
	};
	
		class WoodenPlank: Inventory_Base
	{
		
		inventorySlot[]+=
		{
			"SPLAT_Material_WoodenPlanks"
		};
		
	};
	
	//Fence
	class BaseBuildingBase;
	class SPLATFenceCore : BaseBuildingBase{};
	
	class SPLATFenceDouble : SPLATFenceCore
	{
		scope=2;
		displayName="SPLAT Fence Double";
		descriptionShort="Unraidable Fence Double Sized";
		model="\SPLATFence\data\models\SPLATFenceDouble.p3d";
		bounding="BSphere";
		overrideDrawArea="3.0";
		forceFarBubble="true";
		handheld="false";
		lootCategory="Crafted";
		carveNavmesh=1;
		physLayer="item_large";
		createProxyPhysicsOnInit="false";
		createdProxiesOnInit[]=
		{
			"Deployed"
		};
		rotationFlags=2;
		
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 5000;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"\SPLATFence\data\textures\splatfence.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"\SPLATFence\data\textures\splatfence.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"\SPLATFence\data\textures\splatfence.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"\SPLATFence\data\textures\splatfence.rvmat"
							}
						},
						
						{
							0,
							
							{
								"\SPLATFence\data\textures\splatfence.rvmat"
							}
						}
					};
				};
			};
			class GlobalArmor
			{
				class FragGrenade
				{
					class Health
					{
						damage=0;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0;
					};
				};
			};
			class DamageZones
			{

				class base
				{
					class Health
					{
						hitpoints=100;
						transferToGlobalCoef=0;
						healthLevels[]=
						{
							
							{
								1.0,
								
								{
									//"DZ\gear\camping\data\fence_pile_of_planks.rvmat"
								}
							},
							
							{
								0.69999999,
								
								{
									//"DZ\gear\camping\data\fence_pile_of_planks.rvmat"
								}
							},
							
							{
								0.5,
								
								{
									////"DZ\gear\camping\data\fence_pile_of_planks_damage.rvmat"
								}
							},
							
							{
								0.30000001,
								
								{
									////"DZ\gear\camping\data\fence_pile_of_planks_damage.rvmat"
								}
							},
							
							{
								0.0,
								
								{
									////"DZ\gear\camping\data\fence_pile_of_planks_destruct.rvmat"
								}
							}
						};
					};
					class ArmorType
					{
						class Projectile
						{
							class Health
							{
								damage=0.000001;
							};
							class Blood
							{
								damage=0;
							};
							class Shock
							{
								damage=0;
							};
						};
						class Melee
						{
							class Health
							{
								damage=0.000001;
							};
							class Blood
							{
								damage=0;
							};
							class Shock
							{
								damage=0;
							};
						};
						class FragGrenade
						{
							class Health
							{
								damage=0.000001;
							};
							class Blood
							{
								damage=0;
							};
							class Shock
							{
								damage=0;
							};
						};
					};
					componentNames[]=
					{
						"base"
					};
					fatalInjuryCoef=-1;
				};
				
			};
		};
		
		attachments[]=
		{
			"SPLAT_Material_WoodenLogs",
			"Material_Nails",
			"SPLAT_Material_WoodenPlanks",
			"Material_FPole_Rope"
		};

		class GUIInventoryAttachmentsProps
		{	
			class Base
			{
				name="SPLAT Fence Double";
				description="";
				attachmentSlots[]=
				{	
					"SPLAT_Material_WoodenLogs",
					"Material_Nails",
					"SPLAT_Material_WoodenPlanks",
					"Material_FPole_Rope"
				};
				icon="set:dayz_inventory image:cat_bb_base";
				selection="wall";
			};
		};
		
		class AnimationSources
		{
			class AnimSourceShown
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
			class AnimSourceHidden
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=1;
			};
			class Deployed: AnimSourceHidden {};   
			class Base: AnimSourceHidden {};
			
		};
		
		class Construction
		{
			class wall
			{
				class base
				{
					name="SPLAT Fence Double";
					//setting this to 0 will limit dismantle to one side but create other problems like fence kit spawning and odd visual behavior when dismantling
					//these were fixable in splatfencecore.c but I have reverted because it may be easier to handle the soft side only dismantling from scripts rather than everyting else
					is_base=1;
					id=1;
					required_parts[]={};
					conflicted_parts[]={};
					collision_data[]={};
					build_action_type=4;
					dismantle_action_type=4;
					material_type=1;
					class Materials
					{
						class Material1
						{
							type="WoodenLog";
							slot_name="SPLAT_Material_WoodenLogs";
							quantity=2;
							//16
							//lockable=1;
						};
						class Material2
						{
							type="WoodenPlank";
							slot_name="SPLAT_Material_WoodenPlanks";
							quantity=10;
							//25
							//lockable=1;
						};
						class Material3
						{
							type="Nail";
							slot_name="Material_Nails";
							quantity=20;
							//55
							//lockable=1;
						};
						class Material4
						{
							type="Rope";
							slot_name="Material_FPole_Rope";
							quantity=0;
							lockable=1; //keeps rope in fence after built
						};
					};
				};
			};
		};
	};

	class FenceKit;
	class SPLATFenceKitDouble: FenceKit
	{
		scope=2;
		displayName="SPLAT Fence Double Kit";
		descriptionShort="Fence kit for unraidable wall";
		model="\DZ\gear\camping\fence_kit.p3d";
		rotationFlags=17;
		itemSize[]={1,5};
		weight=280;
		itemBehaviour=1;
		attachments[]=
		{
			"Rope"
		};
		debug_ItemCategory=10;
		soundImpactType="wood";
	};
	
	//Hologram
	class SPLATFenceKitDoublePlacing: FenceKit
	{
		scope=1;
		displayName="This is a hologram";
		descriptionShort="Nothing to see here, move along";
		model="\SPLATFence\data\models\SPLATFenceDoubleKit_Placing.p3d";
		storageCategory=10;
		hiddenSelections[]=
		{
			"placing"
		};
		hiddenSelectionsTextures[]=
		{
			"dz\gear\consumables\data\pile_of_planks_co.tga"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\gear\camping\data\fence_pile_of_planks.rvmat"
		};
		hologramMaterial="tent_medium";
		hologramMaterialPath="dz\gear\camping\data";
		alignHologramToTerain=0;
		slopeTolerance=0.30000001;
	};
	
};
