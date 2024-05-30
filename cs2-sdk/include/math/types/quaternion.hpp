#pragma once

#include <math/types/matrix3x4.hpp>

class Quaternion {
   public:
    Quaternion() = default;
    explicit Quaternion(const matrix3x4_t* matrix);
    matrix3x4_t ToMatrix(const Vector& v) const;

    float operator[](int i) const { return (&x)[i]; }
    float &operator[](int i) { return (&x)[i]; }

    float x, y, z, w;
};
