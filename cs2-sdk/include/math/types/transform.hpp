#pragma once

#include <math/types/quaternion.hpp>
#include <math/types/vector.hpp>

class alignas(16) CTransform {
   public:
    CTransform() {}

    alignas(16) Vector m_Position;
    alignas(16) Quaternion m_Orientation;

    static CTransform FromMatrix(const matrix3x4_t& m);
    matrix3x4_t ToMatrix() const;
};
