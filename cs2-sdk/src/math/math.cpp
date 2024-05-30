#include "pch.hpp"

#include <math/math.hpp>

#include <imgui/imgui_internal.h>

void CMath::UpdateViewMatrix(VMatrix* viewMatrix) {
    if (!viewMatrix) return;
    m_ViewMatrix = *viewMatrix;
}

bool CMath::WorldToScreen(const Vector& in, ImVec2& out) {
    if (!ImGui::GetCurrentContext()) return false;

    const float z = m_ViewMatrix[3][0] * in.x + m_ViewMatrix[3][1] * in.y + m_ViewMatrix[3][2] * in.z + m_ViewMatrix[3][3];
    if (z < 0.001f) return false;

    out = ImGui::GetIO().DisplaySize * 0.5f;
    out.x *= 1.0f + (m_ViewMatrix[0][0] * in.x + m_ViewMatrix[0][1] * in.y + m_ViewMatrix[0][2] * in.z + m_ViewMatrix[0][3]) / z;
    out.y *= 1.0f - (m_ViewMatrix[1][0] * in.x + m_ViewMatrix[1][1] * in.y + m_ViewMatrix[1][2] * in.z + m_ViewMatrix[1][3]) / z;

    // Prevents rounded corners.
    out = ImTrunc(out);

    return true;
}

void CMath::TransformAABB(const matrix3x4_t& transform, const Vector& minsIn, const Vector& maxsIn, Vector& minsOut, Vector& maxsOut) {
    const Vector localCenter = (minsIn + maxsIn) * 0.5f;
    const Vector localExtent = maxsIn - localCenter;

    const auto& mat = transform.m_flMatVal;
    const Vector worldAxisX{mat[0][0], mat[0][1], mat[0][2]};
    const Vector worldAxisY{mat[1][0], mat[1][1], mat[1][2]};
    const Vector worldAxisZ{mat[2][0], mat[2][1], mat[2][2]};

    const Vector worldCenter = localCenter.Transform(&transform);
    const Vector worldExtent{
        localExtent.DotProductAbsolute(worldAxisX),
        localExtent.DotProductAbsolute(worldAxisY),
        localExtent.DotProductAbsolute(worldAxisZ),
    };

    minsOut = worldCenter - worldExtent;
    maxsOut = worldCenter + worldExtent;
}

Vector CMath::CalculateAngle(const Vector& src, const Vector& dst) const { return (dst - src).ToAngle(); }

float CMath::Fov(const Vector& angSrc, const Vector& angDst) const {
    Vector delta = (angSrc - angDst);
    delta.NormalizeAngle();
    return std::hypotf(delta.x, delta.y);
}

float CMath::DistanceFromRay(const Vector& pos, const Vector& start, const Vector& end) {
    const Vector ap = pos - start;
    const Vector ab = end - start;
    const float ab2 = ab.LengthSqr();
    const float ap_ab = ap.DotProduct(ab);
    const float t = ap_ab / ab2;

    if (t < 0.0f) return ap.Length();
    if (t > 1.0f) return (pos - end).Length();
    return (start + ab * t - pos).Length();
}

float CMath::DistanceBetweenLines(const Vector& start1, const Vector& end1, const Vector& start2, const Vector& end2) {
    const Vector u = end1 - start1;
    const Vector v = end2 - start2;
    const Vector w = start1 - start2;
    const float a = u.LengthSqr();
    const float b = u.DotProduct(v);
    const float c = v.LengthSqr();
    const float d = u.DotProduct(w);
    const float e = v.DotProduct(w);
    const float D = a * c - b * b;
    float sc, sN, sD = D;
    float tc, tN, tD = D;

    if (D < FLT_EPSILON) {
        sN = 0.0f;
        sD = 1.0f;
        tN = e;
        tD = c;
    } else {
        sN = b * e - c * d;
        tN = a * e - b * d;
        if (sN < 0.0f) {
            sN = 0.0f;
            tN = e;
            tD = c;
        } else if (sN > sD) {
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }

    if (tN < 0.0f) {
        tN = 0.0f;
        if (-d < 0.0f)
            sN = 0.0f;
        else if (-d > a)
            sN = sD;
        else {
            sN = -d;
            sD = a;
        }
    } else if (tN > tD) {
        tN = tD;
        if (-d + b < 0.0f)
            sN = 0;
        else if (-d + b > a)
            sN = sD;
        else {
            sN = -d + b;
            sD = a;
        }
    }

    sc = std::abs(sN) < FLT_EPSILON ? 0.0f : sN / sD;
    tc = std::abs(tN) < FLT_EPSILON ? 0.0f : tN / tD;

    const Vector dP = w + (u * sc) - (v * tc);
    return dP.Length();
}

void CMath::TransformMatrix(const CTransform& transform, matrix3x4_t& out) {
    QuaternionMatrix(transform.m_Orientation, transform.m_Position, out);
}

void CMath::QuaternionMatrix(const Quaternion& q, const Vector& origin, matrix3x4_t& out) { out = q.ToMatrix(origin); }
