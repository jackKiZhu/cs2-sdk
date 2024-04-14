#pragma once

#include <math/types/vector.hpp>

class CViewSetup {
   public:
    char pad_0000[1172];    // 0x0000
    float m_ortho_left;     // 0x0494
    float m_ortho_top;      // 0x0498
    float m_ortho_right;    // 0x049C
    float m_ortho_bottom;   // 0x04A0
    char pad_04A4[52];      // 0x04A4
    float fov;            // 0x04D8
    float viewmodelFov;  // 0x04DC
    Vector m_origin;        // 0x04E0
    char pad_04EC[12];      // 0x04EC
    Vector m_angles;        // 0x04F8
    char pad_0504[20];      // 0x0504
    float m_aspect_ratio;   // 0x0518
    char pad_051C[28];      // 0x051C
};

class CViewRender
{
   public:
    PAD(0x8);       // 0x0008
    CViewSetup view_setup;  // 0x0010
};
