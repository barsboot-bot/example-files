// ============================================================
//  UE_ModuleAccessories — наушники и портативные колонки
//
//  Наушники (UE_Headphones):
//    Подключаются к играющему плееру/приёмнику рядом.
//    Владелец наушников слышит трек на полной громкости,
//    остальные игроки — нет (приватное прослушивание).
//
//  Колонка (UE_PortableSpeaker):
//    Ставится на землю и подключается к плееру. Радиус
//    слышимости источника увеличивается в speakerRadiusMult раз,
//    базовая громкость — до speakerVolumeMult (клампится до 1).
//
//  Связь "аксессуар -> источник" хранится в GetPEM().SetFlagByte /
//  UEModalObject (net-синхронный), поэтому сервер знает привязку
//  при расчёте затухания для каждого клиента.
// ============================================================

class UE_AccessoryAction: UserActionComponent
{
    ref array<Object> m_ActionObjects;

    override bool CanAppy()
    {
        if (!GetGame().IsDedicated()) return false;
        PlayerBase pl = PlayerBase.Cast(GetActionsObject());
        if (!pl) return false;
        for (int i = 0; i < m_ActionObjects.Count(); i++)
        {
            Object o = m_ActionObjects.Get(i);
            if (o && o != pl && IsAccessory(o)) return true;   // аксессуар в инвентаре/руках
        }
        return false;
    }

    static bool IsAccessory(Object o)
    {
        string c = o.GetType();
        return c == "UE_Headphones" || c == "UE_PortableSpeaker";
    }
};

// ---------- «Подключить наушники к плееру» ----------
class UE_ConnectHeadphonesAction: UE_AccessoryAction
{
    override void AppendToContextMenu()
    {
        string name = "UE_ConnectHeadphones";
        SetDefaultName("#унесённые#подключить_наушники"); // локализация опциональна
        SetIcon("EquipmentAndCrafting:64x64/category_gear_helmet");
        super.AppendToContextMenu();
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        Object t = target.GetObject();
        if (!t) return false;
        if (t.GetType() != "UE_Headphones") return false;
        // цель должна быть рядом с играющим плеером
        return FindNearestPlayingSource(player) != null;
    }

    override void OnExecuteServer(ActionData action_data)
    {
        Object hp = action_data.m_target.GetObject();
        Object src = FindNearestPlayingSource(action_data.m_mainPlayer);
        if (!src || !hp) return;
        int id = UE_AudioManager.Instance().FindSourceIdByObject(src);
        if (id < 0) return;
        UE_AudioManager.Instance().SetPrivateListener(id, action_data.m_main, true);
        action_data.m_mainPlayer.GetInventory().DropCargo(InvItem.New(hp)); // наушники "уходят в плеер"
        Print("[унесённые] " + action_data.m_main.GetName() + ": наушники подключены к #" + id);
    }

    //~ ближайший активный источник в радиусе headphonesRange
    static Object FindNearestPlayingSource(PlayerBase pl)
    {
        float range = UE_AudioManager.Instance().GetHeadphonesRange();
        float best = range;
        Object bestObj = null;
        map<int, ref UE_PlaybackState> sources = UE_AudioManager.Instance().m_Sources;
        for (int i = 0; i < sources.Count(); i++)
        {
            UE_PlaybackState st = sources.GetByIndex(i).Get2();
            if (!st || !st.isPlaying || !st.object) continue;
            if (UE_AudioManager.Instance().IsPrivate(st.id) && st.privateOwner && st.privateOwner != pl) continue; // чужие приватные не трогаем
            float d = vector.Distance(st.position, pl.GetPosition());
            if (d < best) { best = d; bestObj = st.object; }
        }
        return bestObj;
    }
};

// ---------- «Отключить наушники» ----------
class UE_DisconnectHeadphonesAction: UE_AccessoryAction
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        Object t = target.GetObject();
        return t && t.GetType() == "UE_Headphones";
    }

    override void OnExecuteServer(ActionData action_data)
    {
        // снимает приватность со всех источников, где слушатель = этот игрок
        UE_AudioManager.Instance().ClearPrivateForPlayer(action_data.m_main);
        Object hp = action_data.m_target.GetObject();
        if (hp) action_data.m_main.GetInventory().CreateAttachmentEx(hp.GetType(), 0);
    }
};

// ---------- «Поставить и подключить колонку» ----------
class UE_ConnectSpeakerAction: UE_AccessoryAction
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        Object t = target.GetObject();
        if (!t) return false;
        if (t.GetType() != "UE_PortableSpeaker") return false;
        return UE_ConnectHeadphonesAction.FindNearestPlayingSource(player) != null;
    }

    override void OnExecuteServer(ActionData action_data)
    {
        Object spk = action_data.m_target.GetObject();
        Object src = UE_ConnectHeadphonesAction.FindNearestPlayingSource(action_data.m_mainPlayer);
        if (!src || !spk) return;
        int id = UE_AudioManager.Instance().FindSourceIdByObject(src);
        if (id < 0) return;
        UE_AudioManager.Instance().AttachSpeaker(id, spk);
        Print("[унесённые] колонка усилит источник #" + id);
    }
};

// ---------- «Забрать колонку» ----------
class UE_DetachSpeakerAction: UE_AccessoryAction
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        Object t = target.GetObject();
        return t && t.GetType() == "UE_PortableSpeaker" && UE_AudioManager.Instance().HasSpeakerAttached(t);
    }

    override void OnExecuteServer(ActionData action_data)
    {
        Object spk = action_data.m_target.GetObject();
        UE_AudioManager.Instance().DetachSpeaker(spk);
        if (spk) action_data.m_main.GetInventory().CreateAttachmentEx(spk.GetType(), 0);
    }
};

// ============================================================
//  Регистрация экшенов (вызывается из init.c / ECF)
// ============================================================
class UE_RegisterAccessoryActions
{
    static void Register()
    {
        // вешаем действия на сами предметы
        Parametrichandler h;
        GetActionList().AddActionToList(h, "UE_Headphones",  new UE_ConnectHeadphonesAction());
        GetActionList().AddActionToList(h, "UE_Headphones",  new UE_DisconnectHeadphonesAction());
        GetActionList().AddActionToList(h, "UE_PortableSpeaker", new UE_ConnectSpeakerAction());
        GetActionList().AddActionToList(h, "UE_PortableSpeaker", new UE_DetachSpeakerAction());
    }
};
