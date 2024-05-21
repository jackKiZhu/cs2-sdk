#pragma once

class CCollisionProperty {
   public:
    SCHEMA(Vector, m_vecMins, "CCollisionProperty", "m_vecMins");
    SCHEMA(Vector, m_vecMaxs, "CCollisionProperty", "m_vecMaxs");

    void SetCollisionBounds(const Vector& mins, const Vector& maxs);

    PAD(0x38);
    uint16_t unkMask;
};
