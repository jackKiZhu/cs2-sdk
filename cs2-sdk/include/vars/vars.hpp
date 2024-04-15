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
    float m_AimFov = 30.f;
    float m_AimSmooth = 0.15f;
    float m_KP = 0.06f;
    float m_KI = 0.175f;
    float m_Damp = 0.45f;

    float m_RecoilX = 0.35f;
    float m_RecoilY = 0.45f;

    bool m_EnableTriggerbot;

    float m_Fov = 90.f;
    float m_ViewmodelFov = 90.f;
};

extern Vars_t g_Vars;
