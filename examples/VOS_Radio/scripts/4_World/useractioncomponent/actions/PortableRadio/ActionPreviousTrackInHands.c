class ActionPreviousTrackInHand : ActionSingleUseBase
{
	void ActionPreviousTrackInHand()
	{
		m_StanceMask		= DayZPlayerConstants.COMMANDID_NONE;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
		m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONFB_ITEM_ON;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool HasTarget()
	{
		return false;
	}

	override string GetText()
	{
		return "#previous_track";
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
        VOS_Radio mRadio = VOS_Radio.Cast(item);
        if(mRadio.IsPlaying() && mRadio.IsCassettePlaylist())
		    return true;
        return false;
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		VOS_Radio mRadio = VOS_Radio.Cast(action_data.m_MainItem);

		if(mRadio && mRadio.IsPlaying() && mRadio.IsCassettePlaylist())
		{
			mRadio.PreviousTrack();
		}
	}
};
