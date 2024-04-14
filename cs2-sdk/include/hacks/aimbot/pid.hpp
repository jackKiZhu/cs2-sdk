#pragma once

#define _USE_MATH_DEFINES

#include <cmath>

struct PIDConfig_t {
    float m_KP;  // Proportional
    float m_KI;  // Integral
    float m_kd;  // Derivative
    float m_Damp;
};

struct PIDController_t {
    PIDController_t() : p(0.f), i(0.f) {}

    float Step(float err, float dt, const PIDConfig_t& cfg) {
        if (dt == 0.f) return 0.f;

        const float d = (err - p) / dt;
        p = err;
        i += err * dt;

        // If integral and proportional disagree, make integral shut up
        if (p * i <= 0.f) i *= cfg.m_Damp;

        return cfg.m_KP * p + cfg.m_KI * i + cfg.m_kd * d;
    }

    void Reset() {
        p = 0.f;
        i = 0.f;
    }

   private:
    float p;
    float i;
};
