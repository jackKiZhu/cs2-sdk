#pragma once

#include <array>

#include <math/types/vector.hpp>

#include <hacks/aimbot/pid.hpp>

#include <interfaces/ccsgoinput.hpp>

#include <types/model.hpp>

#include <cache/cache.hpp>

class CRecord
{
   public:
    bool valid;
    float simTime;
    std::array<BoneData_t, 1024> boneMatrix;
    Vector head;
};

class CAimbot {
   public:
    static CAimbot& Get() {
        static CAimbot inst;
        return inst;
    }
};
