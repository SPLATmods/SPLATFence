class CfgPatches
{
	class SPLATFence{
		units[] =
		{
			"SPLATFenceDouble",
			"SPLATFenceKitDouble"
		};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] =
		{
			"DZ_Data",
			"DZ_Scripts",
			"DZ_Gear_Consumables",
			"DZ_Gear_Camping",
			"DZ_Gear_Crafting"
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
			class imageSets
			{
				files[] = 
				{
					"SPLATFence\data\gui\imagesets\splatfence_imageset.imageset"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"SPLATFence/scripts/4_World"
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

	class Slot_Material_Nails;
	class Slot_SPLAT_Material_Nails : Slot_Material_Nails
	{
		name = "SPLAT_Material_Nails";
		stackMax = 99;
	};
};

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
	
	class Nail: Inventory_Base
	{
		// vanilla Nail has no inventorySlot[] to append to - assign, don't +=
		inventorySlot[]+=
		{
			"SPLAT_Material_Nails"
		};

	};
	//Fence
	class BaseBuildingBase;
	class SPLATFenceCore : BaseBuildingBase{};
	
	class SPLATFenceDouble : SPLATFenceCore
	{
		scope=2;
		displayName="Indestructible Wall";
		descriptionShort="Indestructible wall with hardened exterior that cannot be damaged. Made from pressure treated utility-grade wooden logs reinforced by counter-pressure slats and rope ties. Can be dismantled from the interior side. This unique item has specific rules associated with it, any base using this wall should always have at least 1 accessible entrace from the ground.  Completely blocking a base off will result in removal of walls and forfeiture of all loot to whoever reported it.";
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
					hitpoints = 1000000;
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
				class Projectile
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
				class Melee
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
						hitpoints=1000000;
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
				class fence: base
				{
					componentNames[]=
					{
						"fence"
					};
				};
			};
		};
		
		attachments[]=
		{
			"Material_WoodenLogs",
			"SPLAT_Material_WoodenLogs",
			"SPLAT_Material_Nails",
			"SPLAT_Material_WoodenPlanks",
			"Material_FPole_Rope"
		};

		class GUIInventoryAttachmentsProps
		{	
			class Base
			{
				name="Base";
				description="";
				attachmentSlots[]=
				{	
					"Material_WoodenLogs"
				};
				icon="set:splatfence_buildingStage_icons image:SPLATFence_Base";
				selection="wall";
			};
			class Material
			{
				name="Indestructible Wall";
				description="";
				attachmentSlots[]=
				{	
					"SPLAT_Material_WoodenLogs",
					"SPLAT_Material_Nails",
					"SPLAT_Material_WoodenPlanks",
					"Material_FPole_Rope"
				};
				icon="set:splatfence_buildingStage_icons image:SPLATFence_Fence";
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
			class Fence: AnimSourceHidden {};

			// Lockable material slots: their proxy stays attached (locked) after
			// build, so UpdateAttachmentVisuals' SetAnimationPhase(<slot>, 1) is what
			// hides it. Each needs a matching SkeletonBones[] + sections[] + type="hide"
			// anim in model.cfg and a selection of the same name in the .p3d.
			// The 3 non-lockable slots are consumed on build - engine drops their
			// proxy automatically, no anim needed.
			class Material_WoodenLogs: AnimSourceHidden {};
			class Material_FPole_Rope: AnimSourceHidden {};
		};
		
		class Construction
		{
			class wall
			{
				class base
				{
					name="Base";
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
							slot_name="Material_WoodenLogs";
							quantity=2;
							lockable=1;
						};
					};
				};
				class fence
				{
					name="Indestructible Wall";
					id=2;
					required_parts[]={"base"};
					conflicted_parts[]={};
					collision_data[]={};
					build_action_type=2;		// Hammer (2 & 10 > 0)
					dismantle_action_type=2;	// Crowbar / MeatTenderizer, as vanilla fence walls
					material_type=2;
					class Materials
					{
						class Material1
						{
							type="WoodenLog";
							slot_name="SPLAT_Material_WoodenLogs";
							quantity=20;
						};
						class Material2
						{
							type="WoodenPlank";
							slot_name="SPLAT_Material_WoodenPlanks";
							quantity=50;
						};
						class Material3
						{
							type="Nail";
							slot_name="SPLAT_Material_Nails";
							quantity=99;				
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
		displayName="Indestructible Wall Kit";
		descriptionShort="Indestructible wall with hardened exterior that cannot be damaged. Made from pressure treated utility-grade wooden logs reinforced by counter-pressure slats and rope ties. Can be dismantled from the interior side. This unique item has specific rules associated with it, any base using this wall should always have at least 1 accessible entrace from the ground.  Completely blocking a base off will result in removal of walls and forfeiture of all loot to whoever reported it.";
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

class CfgNonAIVehicles
{
	class ProxyAttachment;
	
	class ProxyWoodenLog : ProxyAttachment
	{
		scope = 1;
		inventorySlot = "Material_WoodenLogs";
		model = "\SPLATFence\data\models\proxy\WoodenLog.p3d";
	};
	class ProxyWoodenLog2 : ProxyAttachment
	{
		scope = 1;
		inventorySlot = "SPLAT_Material_WoodenLogs";
		model = "\SPLATFence\data\models\proxy\WoodenLog2.p3d";
	};
	class ProxyWoodenPlank : ProxyAttachment
	{
		scope = 1;
		inventorySlot = "SPLAT_Material_WoodenPlanks";
		model = "\SPLATFence\data\models\proxy\WoodenPlank.p3d";
	};
	class ProxyRope : ProxyAttachment
	{
		scope = 1;
		inventorySlot = "Material_FPole_Rope";
		model = "\SPLATFence\data\models\proxy\Rope.p3d";
	};
	class ProxyNails : ProxyAttachment
	{
		scope = 1;
		inventorySlot = "SPLAT_Material_Nails";
		model = "\SPLATFence\data\models\proxy\Nails.p3d";
	};
};
