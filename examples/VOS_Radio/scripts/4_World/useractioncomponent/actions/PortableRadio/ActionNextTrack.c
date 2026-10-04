class ActionNextTrack: ActionInteractBase
{
	override string GetText()
	{
		return "#next_track";
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		VOS_Radio mRadio = VOS_Radio.Cast(target.GetObject());
        if(mRadio.IsPlaying() && mRadio.IsCassettePlaylist())
		    return true;
        return false;
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		Object targetObject = VOS_Radio.Cast(action_data.m_Target.GetObject());
		VOS_Radio mRadio;

		if (targetObject)
		{
			mRadio = VOS_Radio.Cast( targetObject );
			if(mRadio && mRadio.IsPlaying() && mRadio.IsCassettePlaylist())
			{
				mRadio.NextTrack();
			}
		}
	}
};