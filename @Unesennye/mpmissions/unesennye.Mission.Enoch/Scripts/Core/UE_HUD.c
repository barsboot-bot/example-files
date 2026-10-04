// ============================================================
//  UE_HUD — игровой оверлей мода «унесённые»
//  Показывает название текущего трека/станции и громкость
//  ближайшего слышимого источника (правый нижний угол).
//  Рисует через ScriptDrawAPI (движковый 2D-рендер, доступен
//  из скриптов без сторонних библиотек).
// ============================================================

class UE_HUD: ScriptModule
{
    private ref ShapeGroup m_Shape;
    private ref TextShape m_TitleText;
    private ref TextShape m_SubText;
    private ref RectShape m_VolBg;
    private ref RectShape m_VolFill;

    private string m_LastTitle = "";
    private float m_FadeTime = 0;       // когда скрывать (после тишины)
    private bool m_Visible = false;

    void UE_HUD()
    {
        // панель ~280x54, правый нижний край экрана
        m_Shape = new ShapeGroup;
        m_Shape.AddShape(m_VolBg  = new RectShape(0, 0, 260, 6,  Color(0,0,0,0.55)));
        m_Shape.AddShape(m_VolFill= new RectShape(0, 0, 0,   6,  Color(0.85,0.65,0.2,0.9)));
        m_Shape.AddShape(m_TitleText = new TextShape(0, -22, "", "font:combination;fontsize:18;color:1,1,1,1"));
        m_Shape.AddShape(m_SubText   = new TextShape(0,  10, "", "font:combination;fontsize:13;color:0.75,0.75,0.75,0.85"));
        GetGame().GetUIManager().GetInterface("HudOverlay").AddChild(m_Shape);
        m_Shape.SetVisible(false);
    }

    //~ позиционирование относительно размера окна (правый нижний угол)
    void Layout()
    {
        float w = GetMouse().GetScreenSizeX();
        float h = GetMouse().GetScreenSizeY();
        m_Shape.SetPos(w - 290, h - 90);
    }

    //~ вызывается из OnUpdate менеджера раз в тик (клиент)
    void Update(float dt)
    {
        if (GetGame().IsDedicated()) return;

        // находим самый громкий активный источник рядом с игроком
        float bestV = 0;
        UE_PlaybackState best;
        map<int, ref UE_PlaybackState> mirror = UE_AudioManager.s_ClientMirror;
        if (mirror)
        {
            for (int i = 0; i < mirror.Count(); i++)
            {
                UE_PlaybackState st = mirror.GetByIndex(i).Get2();
                if (!st || !st.isPlaying) continue;
                float v = UE_AudioManager.Instance().CalcAttenuation(st.position, st.volume);
                if (v > bestV) { bestV = v; best = st; }
            }
        }

        if (best && bestV > 0.01)
        {
            string title = UE_AudioManager.DescribeSource(best.stationKey, best.playlist);
            string sub = DescribeType(best.type);
            Show(title, sub, bestV);
        }
        else
        {
            Hide(dt);
        }
    }

    static string DescribeType(int t)
    {
        switch (t)
        {
            case UE_SourceType.CASSETTE: return "кассета";
            case UE_SourceType.DISK:     return "диск";
            case UE_SourceType.RADIO:    return "радио";
            case UE_SourceType.CAR:      return "автомагнитола";
        }
        return "";
    }

    void Show(string title, string sub, float vol)
    {
        if (!m_Visible)
        {
            m_Shape.SetVisible(true);
            m_Visible = true;
            Layout();
        }
        if (title != m_LastTitle)
        {
            m_TitleText.SetText(title);
            m_LastTitle = title;
        }
        m_SubText.SetText(sub);
        m_VolFill.SetSize(260.0 * Math.Clamp(vol, 0, 1), 6);
        m_FadeTime = 3.0;   // переживёт короткие паузы между треками
    }

    void Hide(float dt)
    {
        if (!m_Visible) return;
        m_FadeTime -= dt;
        if (m_FadeTime <= 0)
        {
            m_Shape.SetVisible(false);
            m_Visible = false;
            m_LastTitle = "";
        }
    }
};

// ============================================================
//  Примечание по API: если в вашей сборке движка классы
//  ShapeGroup/TextShape недоступны из миссии, замените тело
//  Update() на вывод через GetDebug().AddWaypoint()/ChatMessage
//  или используйте UI-layout (DiagramCanvas) из expansion mod.
// ============================================================
