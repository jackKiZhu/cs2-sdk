#include <pch.hpp>

#include <math/types/vector.hpp>
#include <math/types/matrix3x4.hpp>

Vector Vector::Transform(const matrix3x4_t* matrix) const {
    const matrix3x4_t& mat = *matrix;
    return Vector{x * mat[0][0] + y * mat[1][0] + z * mat[2][0] + mat[0][3], x * mat[0][1] + y * mat[1][1] + z * mat[2][1] + mat[1][3],
                  x * mat[0][2] + y * mat[1][2] + z * mat[2][2] + mat[2][3]};
}
