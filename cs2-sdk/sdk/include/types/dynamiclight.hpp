#pragma once

class CDynamicLight
{
   public:
    PAD(0x4);
    Vector origin;
    float radius;
    Color_t color;
    float die;
    float exponent;
    PAD(40);
    void* sceneLayer;
};
