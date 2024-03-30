#pragma once

struct Vars_t {
    bool m_EnableESP;

    bool m_OtherBoxes;
    bool m_HostageBoxes;
    bool m_WeaponBoxes;
    bool m_ChickenBoxes;

    bool m_Team;
    bool m_PlayerBoxes;
    bool m_PlayerNames;
    bool m_PlayerHealthBar;

    bool m_Use3DBoxes;

    bool m_EnableAimbot;
};

extern Vars_t g_Vars;
