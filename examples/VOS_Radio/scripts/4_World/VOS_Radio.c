class VOS_Radio : ItemBase
{
    protected ItemBase m_Cassette;
    protected bool m_Playing;
    protected EffectSound m_ActiveSound;
    protected int m_CurrentTrackIndex = 0;
    protected ref array<string> SoundSets = {};
    protected string SoundSet;
    
    protected bool m_SyncStatus;
    
    void VOS_Radio()
    {
        RegisterNetSyncVariableBool( "m_SyncStatus" );
        RegisterNetSyncVariableInt( "m_CurrentTrackIndex" );
    }
    
    override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
        
        if( m_SyncStatus && !IsPlaying() ) TurnOn();
        if( !m_SyncStatus && IsPlaying() ) TurnOff();
	}

    override void EEItemAttached( EntityAI item, string slot_name )
    {
        super.EEItemAttached( item, slot_name );
        
        if( slot_name == "Cassette" )
        {
            SetupCassette(item);
        }
    }
    
	override void EEItemDetached( EntityAI item, string slot_name )
	{
        super.EEItemDetached( item, slot_name );
        
        if( slot_name == "Cassette" )
        {
            OnCassetteDetached();
        }

        TurnOff();
    }

    override void EEDelete(EntityAI parent)
    {
        super.EEDelete(parent);
        Stop();
    }

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
            Play();
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
            Play();
        }
    }

    bool Play()
    {
        if(!CanPlaySound())
            return false;

        Stop();

        if (!GetGame().IsServer()  ||  !GetGame().IsMultiplayer())
        {
            if(IsCassettePlaylist())
            {
                m_ActiveSound = SEffectManager.CreateSound(SoundSets[m_CurrentTrackIndex], GetPosition());
                m_ActiveSound.Event_OnSoundWaveEnded.Insert(NextTrack);
            }
            else
            {
                m_ActiveSound = SEffectManager.CreateSound(SoundSet, GetPosition());
            }
            //m_ActiveSound.SetSoundWaveKind(WaveKind.WAVEMUSIC);
            m_ActiveSound.SetParent(this);
            m_ActiveSound.SetSoundAutodestroy(true);
            m_ActiveSound.SoundPlay();
            SetTapeVolume();
        }

        m_Playing = true;
        m_SyncStatus = true;
        SetSynchDirty();

        return true;
    }

    void Stop()
    {
        m_Playing = false;
        m_SyncStatus = false;
        SetSynchDirty();

        if (m_ActiveSound)
        {
            if(IsCassettePlaylist())
            {
                m_ActiveSound.Event_OnSoundWaveEnded.Clear();
            }
            m_ActiveSound.SoundStop();
        }
    }

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
        string cfgPath = string.Format("CfgVehicles %1 CfgCassette", m_Cassette.GetType());

        if(GetGame().ConfigIsExisting(cfgPath))
        {
            return GetGame().ConfigGetInt(cfgPath + " isPlaylist");
        }
        return false;
    }

    override bool CanReleaseAttachment(EntityAI attachment)
	{
		super.CanReleaseAttachment(attachment);

		if (GetCompEM().IsWorking())
		{
			return false;
		}
		return true;
	}

    void TurnOn()
    {
        if ( GetCompEM().CanWork() )
        {
            GetCompEM().SwitchOn();
        }
    } 
    
    void TurnOff()
    {
        GetCompEM().SwitchOff();
    }

    void OnCassetteDetached()
    {
        Stop();
        m_Cassette = NULL;
        m_CurrentTrackIndex = 0;
        SoundSets = {};
    }

    override void OnSwitchOn()
    {
        if (!Play())
        {
            TurnOff();
        }
    }

    override void OnSwitchOff()
	{
		if(IsPlaying())
		{
			Stop();
            delete m_ActiveSound;
		}
	}
    
    bool IsPlaying()
    {
        return m_Playing;
    }

    void SetTapeVolume()
    {
        m_ActiveSound.SetMaxVolume(0.4);
    }
    
    override void SetActions()
	{
        super.SetActions();
		AddAction(ActionTurnOnWhileInHands);
		AddAction(ActionTurnOffWhileInHands);
		AddAction(ActionTurnOnWhileOnGround);
		AddAction(ActionTurnOffWhileOnGround);
		AddAction(ActionNextTrackInHand);
		AddAction(ActionNextTrack);
		AddAction(ActionPreviousTrack);
		AddAction(ActionPreviousTrackInHand);
	}
};

class VOS_RadioPanasonic : VOS_Radio {};
class VOS_RadioVesna : VOS_Radio {};

class VOS_RadioWalkman : VOS_Radio
{
    override void SetTapeVolume()
    {   
        m_ActiveSound.SetMaxVolume(0.2);
    }
};
