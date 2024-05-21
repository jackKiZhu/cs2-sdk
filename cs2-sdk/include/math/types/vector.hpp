#pragma once

#include <math/types/matrix3x4.hpp>

class Vector {
   public:
    Vector() : x(0.f), y(0.f), z(0.f) {}
    Vector(float rhs) : x(rhs), y(rhs), z(rhs) {}
    Vector(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector operator+(const Vector& rhs) const { return Vector{x + rhs.x, y + rhs.y, z + rhs.z}; }
    Vector operator-(const Vector& rhs) const { return Vector{x - rhs.x, y - rhs.y, z - rhs.z}; }
    Vector operator*(float scalar) const { return Vector{x * scalar, y * scalar, z * scalar}; }
    Vector operator/(float scalar) const { return Vector{x / scalar, y / scalar, z / scalar}; }
    bool operator==(const Vector& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
    bool IsZero() const { return x == 0.f && y == 0.f && z == 0.f; }
    bool IsNan() const { return std::isnan(x) || std::isnan(y) || std::isnan(z); }
    bool IsFinite() const { return std::isfinite(x) && std::isfinite(y) && std::isfinite(z); }
    bool IsValid() const { return IsFinite() && !IsNan(); }
    void operator+=(const Vector& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
    }
    void operator-=(const Vector& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
    }
    void operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
    }
    void operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
    }

    void NormalizeAngle() {
        x = std::remainder(x, 360.f);
        y = std::remainder(y, 360.f);
        z = std::remainder(z, 360.f);
    }

    Vector NormalizedAngle() const {
        Vector vec = *this;
        vec.NormalizeAngle();
        return vec;
    }

    float Length() const { return std::sqrt(x * x + y * y + z * z); }

    float LengthSqr() const { return x * x + y * y + z * z; }

    float Length2D() const { return std::sqrt(x * x + y * y); }

    void Normalize() {
        float length = Length();
        if (length == 0.f) {
            x = y = z = 0.f;
            return;
        }
        x /= length;
        y /= length;
        z /= length;
    }

    Vector Normalized() const {
        Vector vec = *this;
        vec.Normalize();
        return vec;
    }

    Vector Transform(const matrix3x4_t& matrix) const {
        return Vector{x * matrix[0][0] + y * matrix[0][1] + z * matrix[0][2] + matrix[0][3],
                      x * matrix[1][0] + y * matrix[1][1] + z * matrix[1][2] + matrix[1][3],
                      x * matrix[2][0] + y * matrix[2][1] + z * matrix[2][2] + matrix[2][3]};
    }

    float DotProduct(const Vector& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
    float DotProductAbsolute(const Vector& rhs) const { return std::abs(x * rhs.x) + std::abs(y * rhs.y) + std::abs(z * rhs.z); }

    float DistTo(const Vector& rhs) const { return (*this - rhs).Length(); }

    Vector Min(const Vector& rhs) const { return Vector{std::min(x, rhs.x), std::min(y, rhs.y), std::min(z, rhs.z)}; }
    Vector Max(const Vector& rhs) const { return Vector{std::max(x, rhs.x), std::max(y, rhs.y), std::max(z, rhs.z)}; }

    Vector ToAngle() const {
        Vector angle;
        if (x == 0.f && y == 0.f) {
            angle.x = z > 0.f ? 270.f : 90.f;
            angle.y = 0.f;
        } else {
            angle.x = std::atan2(-z, std::sqrt(x * x + y * y)) * (180.f / (float)M_PI);
            if (angle.x < 0.f) angle.x += 360.f;
            angle.y = std::atan2(y, x) * (180.f / (float)M_PI);
            if (angle.y < 0.f) angle.y += 360.f;
        }
        angle.z = 0.f;
        angle.NormalizeAngle();
        return angle;
    }

    Vector ToVector(Vector* right = nullptr, Vector* up = nullptr) const {
        const float sp = std::sin(x * ((float)M_PI / 180.f)), cp = std::cos(x * ((float)M_PI / 180.f)),
                    sy = std::sin(y * ((float)M_PI / 180.f)), cy = std::cos(y * ((float)M_PI / 180.f)),
                    sr = std::sin(z * ((float)M_PI / 180.f)), cr = std::cos(z * ((float)M_PI / 180.f));

        Vector forward{cp * cy, cp * sy, -sp};
        if (right) {
            right->x = -1.f * sr * sp * cy + -1.f * cr * -sy;
            right->y = -1.f * sr * sp * sy + -1.f * cr * cy;
            right->z = -1.f * sr * cp;
        }

        if (up) {
            up->x = cr * sp * cy + -sr * -sy;
            up->y = cr * sp * sy + -sr * cy;
            up->z = cr * cp;
        }

        return forward;
    }

    float x, y, z;
};
