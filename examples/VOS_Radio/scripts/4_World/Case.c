class VOS_CassetteCase : Container_Base
{
    override bool CanReceiveItemIntoCargo(EntityAI item)
    {
        if (!super.CanReceiveItemIntoCargo(item))
            return false;

        if (!item.IsKindOf("VOS_Cassette_Base"))
            return false;

        return true;
    }

    override bool CanSwapItemInCargo(EntityAI child_entity, EntityAI new_entity)
    {
        if (!super.CanSwapItemInCargo(child_entity, new_entity))
            return false;

        if (!new_entity.IsKindOf("VOS_Cassette_Base"))
            return false;

        return true;
    }

    override void EECargoIn(EntityAI item)
    {
        super.EECargoIn(item);

        Man player;
        if (!item.IsKindOf("VOS_Cassette_Base") && Man.CastTo(player, GetHierarchyRootPlayer())) {
            player.PredictiveDropEntity(item);
        }
    }
};


/*
class VOS_CassetteCase : Container_Base
{
	override bool IsContainer()
	{
		return true;
	}

	override bool CanReceiveItemIntoCargo( EntityAI item )
	{
		if (item.IsKindOf("VOS_Cassette_Base"))
		{
			return super.CanReceiveItemIntoCargo(item);
		}
		return false;
	}

	override bool CanSwapItemInCargo (EntityAI child_entity, EntityAI new_entity)
	{
		if (new_entity.IsKindOf("VOS_Cassette_Base"))
		{
			return super.CanSwapItemInCargo( child_entity, new_entity );
		}
		return false;
	}
}