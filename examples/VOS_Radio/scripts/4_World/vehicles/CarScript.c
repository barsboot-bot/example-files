modded class CarScript
{
	ref protected EffectSound m_CarRadioLoop;
	protected ItemBase m_Cassette;
	protected bool m_RadioToggled = false;
	protected bool m_RadioPlaying = false;
	protected int m_CurrentTrackIndex = 0;
    protected ref array<string> SoundSets = {};
    protected string SoundSet;
	protected int CassetteSlot = InventorySlots.GetSlotIdFromString("Cassette");

	void CarScript()
	{
		RegisterNetSyncVariableBool("m_RadioToggled");
		RegisterNetSyncVariableBool("m_RadioPlaying");
		RegisterNetSyncVariableInt("m_CurrentTrackIndex");
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ClearCassetteSlotLock, 1000, false);
	};
	
	void ClearCassetteSlotLock()
	{
		GetInventory().SetSlotLock( CassetteSlot, false );
	};
	
 	void ToggleRadio()
	{
		m_RadioToggled = !m_RadioToggled;
		GetInventory().SetSlotLock(CassetteSlot, !GetInventory().GetSlotLock(CassetteSlot));
		SetSynchDirty();
	};
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if (m_RadioToggled && !m_RadioPlaying)
		{
			m_RadioPlaying = !m_RadioPlaying;
			OnPlaySong();
		}
		if (!m_RadioToggled && m_RadioPlaying)
		{
			m_RadioPlaying = !m_RadioPlaying;
			OnStopSong();
		}
	};
	
	override void EEItemAttached(EntityAI item, string slot_name)
    {
        super.EEItemAttached(item, slot_name);
        
        if(slot_name == "Cassette")
        {
            SetupCassette(item);
        }
    }
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);

		if (slot_name == "CarBattery" || slot_name == "TruckBattery")
		{
			m_RadioToggled = !m_RadioToggled;
			SetSynchDirty();
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ClearCassetteSlotLock, 1000, false);
		}
		if (slot_name == "Cassette")
        {
            OnCassetteDetached();
        }
	};

	void SetupCassette(EntityAI item)
    {
        m_Cassette = ItemBase.Cast( item );
        string cfgPath = string.Format("CfgVehicles %1 CfgCassette", m_Cassette.GetType());

        if(GetGame().ConfigIsExisting(cfgPath))
        {
            if(GetGame().ConfigGetInt(cfgPath + " isPlaylist"))
            {
                GetGame().ConfigGetTextArray(cfgPath + " soundSets", SoundSets);
            }
            else
            {
                SoundSet = GetGame().ConfigGetTextOut(cfgPath + " soundSet");
            }
        }
    }
	
	bool IsRadioToggled()
	{
		return m_RadioToggled;
	};
	
	bool IsRadioPlaying()
	{
		return m_RadioPlaying;
	};

	void NextTrack()
    {
        if(m_Cassette && SoundSets.Count())
        {
            if(m_CurrentTrackIndex + 1 >= SoundSets.Count())
            {
                m_CurrentTrackIndex = 0;
            }
            else
            {
                ++m_CurrentTrackIndex;
            }
            SetSynchDirty();
            OnPlaySong();
        }
    }

	void PreviousTrack()
    {
        if(m_Cassette && SoundSets.Count())
        {
            if(m_CurrentTrackIndex - 1 <= 0)
            {
                m_CurrentTrackIndex = 0;
            }
            else
            {
                --m_CurrentTrackIndex;
            }
            SetSynchDirty();
            OnPlaySong();
        }
    }
	
	void OnPlaySong()
	{
		if(!CanPlaySound())
            return;

		OnStopSong();
		if(m_Cassette)
		{
			if(!GetGame().IsServer()  ||  !GetGame().IsMultiplayer())
			{
				if (IsCassettePlaylist())
				{
					m_CarRadioLoop = SEffectManager.CreateSound(SoundSets[m_CurrentTrackIndex], GetPosition());
					m_CarRadioLoop.Event_OnSoundWaveEnded.Insert(NextTrack);
				}
				else
				{
					m_CarRadioLoop = SEffectManager.CreateSound(SoundSet, GetPosition());
				}
				//m_CarRadioLoop.SetSoundWaveKind(WaveKind.WAVEMUSIC);
				m_CarRadioLoop.SetParent(this);
				m_CarRadioLoop.SetSoundAutodestroy(true);
				m_CarRadioLoop.SoundPlay();
			}
		}
	};

	bool CanPlaySound()
    {
        if(FindAttachmentBySlotName("Cassette"))
		{
			return true;
		}
		return false;
    }


	bool IsCassettePlaylist()
    {
		if(m_Cassette)
        string cfgPath = string.Format("CfgVehicles %1 CfgCassette", m_Cassette.GetType());

        if(GetGame().ConfigIsExisting(cfgPath))
        {
            return GetGame().ConfigGetInt(cfgPath + " isPlaylist") == 1;
        }
        return false;
    }

	void OnStopSong()
	{
		if (m_CarRadioLoop)
        {
            if(IsCassettePlaylist())
            {
                m_CarRadioLoop.Event_OnSoundWaveEnded.Clear();
            }
            m_CarRadioLoop.SoundStop();
        }
	};

	void OnCassetteDetached()
    {
        OnStopSong();
        m_Cassette = NULL;
        m_CurrentTrackIndex = 0;
        SoundSets = {};
    }

	override void EEDelete(EntityAI parent)
    {
        super.EEDelete(parent);
        OnStopSong();
    }
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionToggleRadio);
		AddAction(ActionNextTrackInCar);
		AddAction(ActionPreviousTrackInCar);
	};
};
