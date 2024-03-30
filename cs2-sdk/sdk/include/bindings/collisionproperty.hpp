#pragma once

class CCollisionProperty {
   public:
    SCHEMA(Vector, m_vecMins, "CCollisionProperty", "m_vecMins");
    SCHEMA(Vector, m_vecMaxs, "CCollisionProperty", "m_vecMaxs");

    PAD(0x38);
    uint16_t unkMask;
};
