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
    float m_aimFov = 30.f;
    float m_aimSmooth = 0.15f;
    float m_kp = 0.06f;
    float m_ki = 0.175f;
    float m_damp = 0.45f;

    bool m_EnableTriggerbot;
};

extern Vars_t g_Vars;
