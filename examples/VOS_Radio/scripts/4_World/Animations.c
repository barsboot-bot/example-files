modded class ModItemRegisterCallbacks
{
 	override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
		super.RegisterOneHanded( pType, pBehavior );
        
        pType.AddItemInHandsProfileIK("VOS_Radio", "dz/anims/workspaces/player/player_main/props/player_main_1h_radio.asi", pBehavior, "dz/anims/anm/player/ik/gear/Radio.anm");
        pType.AddItemInHandsProfileIK("VOS_RadioVesna", "dz/anims/workspaces/player/player_main/props/player_main_1h_radio.asi", pBehavior, "dz/anims/anm/player/ik/gear/Radio.anm");
        pType.AddItemInHandsProfileIK("VOS_RadioPanasonic", "dz/anims/workspaces/player/player_main/props/player_main_1h_radio.asi", pBehavior, "dz/anims/anm/player/ik/gear/Radio.anm");
        pType.AddItemInHandsProfileIK("VOS_RadioWalkman", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/PersonalRadio.anm");
        pType.AddItemInHandsProfileIK("YK_Cassette_Base", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/cassette.anm");
        pType.AddItemInHandsProfileIK("YK_Cassette_Base2", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/cassette.anm");
        pType.AddItemInHandsProfileIK("YK_CassetteCase", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/cleaning_kit_wood.anm");
	}	  	
}