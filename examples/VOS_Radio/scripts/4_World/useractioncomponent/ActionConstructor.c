modded class ActionConstructor 
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);
		actions.Insert(ActionToggleRadio);
		actions.Insert(ActionNextTrack);
		actions.Insert(ActionNextTrackInHand);
		actions.Insert(ActionNextTrackInCar);
		actions.Insert(ActionPreviousTrackInCar);
		actions.Insert(ActionPreviousTrackInHand);
		actions.Insert(ActionPreviousTrack);
	};
};