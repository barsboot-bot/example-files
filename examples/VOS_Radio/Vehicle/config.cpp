class CfgPatches
{
	class VOSRadio
	{
		units[]={};
		ammo[]={};
		weapons[]={};
		magazines[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Vehicles_Wheeled"
		};
	};
};
class CfgVehicles
{
	class CarScript;
	class OffroadHatchback: CarScript
	{
		attachments[]+=
		{
			"Cassette"
		};
		class GUIInventoryAttachmentsProps
		{
			class RadioMoreCassetes
			{
				name="$STR_Magnetica";
				description="$STR_Magnetica";
				icon="default";
				attachmentSlots[]=
				{
					"Cassette"
				};
			};
		};
	};
	class CivilianSedan: CarScript
	{
		attachments[]+=
		{
			"Cassette"
		};
		class GUIInventoryAttachmentsProps
		{
			class RadioMoreCassetes
			{
				name="$STR_Magnetica";
				description="$STR_Magnetica";
				icon="default";
				attachmentSlots[]=
				{
					"Cassette"
				};
			};
		};
	};
	class Hatchback_02: CarScript
	{
		attachments[]+=
		{
			"Cassette"
		};
		class GUIInventoryAttachmentsProps
		{
			class RadioMoreCassetes
			{
				name="$STR_Magnetica";
				description="$STR_Magnetica";
				icon="default";
				attachmentSlots[]=
				{
					"Cassette"
				};
			};
		};
	};
	class Sedan_02: CarScript
	{
		attachments[]+=
		{
			"Cassette"
		};
		class GUIInventoryAttachmentsProps
		{
			class RadioMoreCassetes
			{
				name="$STR_Magnetica";
				description="$STR_Magnetica";
				icon="default";
				attachmentSlots[]=
				{
					"Cassette"
				};
			};
		};
	};
	class Truck_01_Base;
	class Truck_01_Covered: Truck_01_Base
	{
		attachments[]+=
		{
			"Cassette"
		};
		class GUIInventoryAttachmentsProps
		{
			class RadioMoreCassetes
			{
				name="$STR_Magnetica";
				description="$STR_Magnetica";
				icon="default";
				attachmentSlots[]=
				{
					"Cassette"
				};
			};
		};
	};
	class Offroad_02: CarScript
	{
		attachments[]+=
		{
			"Cassette"
		};
		class GUIInventoryAttachmentsProps
		{
			class RadioMoreCassetes
			{
				name="$STR_Magnetica";
				description="$STR_Magnetica";
				icon="default";
				attachmentSlots[]=
				{
					"Cassette"
				};
			};
		};
	};
};
