#pragma once

#include <math/types/vector.hpp>
#include <math/types/quaternion.hpp>

struct matrix3x4_t {
    auto operator[](int i) const { return m_flMatVal[i]; }
    auto operator[](int i) { return m_flMatVal[i]; }

    Vector GetAxis(int i) const { return Vector(m_flMatVal[0][i], m_flMatVal[1][i], m_flMatVal[2][i]); }
    void SetAxis(int i, const Vector &v) {
        m_flMatVal[0][i] = v.x;
        m_flMatVal[1][i] = v.y;
        m_flMatVal[2][i] = v.z;
    }

    float m_flMatVal[3][4];
};

struct /*__declspec(align(16)) */matrix3x4a_t : public matrix3x4_t {

};
