#pragma once

#define _USE_MATH_DEFINES

#include <cmath>

struct PIDConfig_t {
    float m_kp;  // Proportional
    float m_ki;  // Integral
    float m_kd;  // Derivative
    float m_damp;
};

struct PIDController_t {
    PIDController_t() : p(0.f), i(0.f) {}

    float Step(float err, float dt, const PIDConfig_t& cfg) {
        if (dt == 0.f) return 0.f;

        const float d = (err - p) / dt;
        p = err;
        i += err * dt;

        // If integral and proportional disagree, make integral shut up
        if (p * i <= 0.f) i *= cfg.m_damp;

        return cfg.m_kp * p + cfg.m_ki * i + cfg.m_kd * d;
    }

    void Reset() {
        p = 0.f;
        i = 0.f;
    }

   private:
    float p;
    float i;
};
