class CfgPatches
{
	class VOS_Cassette_Base
	{
		units[]=
		{
			"VOS_CassetteCase"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
	};
};
class CfgVehicles
{
	class Container_Base;
	class VOS_CassetteCase: Container_Base
	{
		scope=2;
		displayName="$STR_Case";
		descriptionShort="$STR_Case_desc";
		model="\dz\gear\tools\cleaning_kit_wood.p3d";
		animClass="Knife";
		rotationFlags=17;
		quantityBar=0;
		weight=580;
		itemSize[]={2,3};
		itemsCargoSize[]={10,5};
		fragility=0.0099999998;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"VOS_Radio\CassetteCase\data\cassette_case.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"VOS_Radio\CassetteCase\data\cassette_case.rvmat"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"VOS_Radio\CassetteCase\data\cassette_case.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"VOS_Radio\CassetteCase\data\cassette_case.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"VOS_Radio\CassetteCase\data\cassette_case_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"VOS_Radio\CassetteCase\data\cassette_case_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"VOS_Radio\CassetteCase\data\cassette_case_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class MeleeModes
		{
			class Default
			{
				ammo="MeleeLightBlunt";
				range=1;
			};
			class Heavy
			{
				ammo="MeleeLightBlunt_Heavy";
				range=1;
			};
			class Sprint
			{
				ammo="MeleeLightBlunt_Heavy";
				range=2.8;
			};
		};
	};
};
