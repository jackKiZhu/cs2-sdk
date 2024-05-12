#pragma once

struct Vars_t {
    bool m_EnableESP;
    bool m_EnableIfSpectating;

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
    bool m_EnableInDuel;
    float m_AimFov = 9.5f;
    float m_DelayFov = 2.f;
    float m_KP = 0.03f;
    float m_KI = 0.100f;
    float m_Damp = 0.45f;
    float m_ReactionTreshold = 0.85f;
    float m_Bonus = 0.5f;
    bool m_DrawFov;

    float m_RecoilX = 0.7f;
    float m_RecoilY = 0.4f;

    bool m_EnableTriggerbot;

    float m_Fov = 90.f;
    float m_ViewmodelFov = 90.f;

    float m_MinExposure = 0.f;
    float m_MaxExposure = 0.f;
};

extern Vars_t g_Vars;
