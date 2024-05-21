#pragma once

#define RATIO_THRESHOLD 0.0099999998f

class CGlobalVars {
   public:
    static CGlobalVars* Get();

    float realTime; // 0x0 (0x4)
    int frameCount; // 0x4 (0x4)
    float frameTime; // 0x8 (0x4)
    float absoluteFrameTime; // 0xC (0x4)
    int maxClients; // 0x10 (0x4)
    PAD(0xC); // 0x14 (0xC)
    PAD(0x8); // 0x20 (0x8)
    float intervalPerTick; // 0x28 (0x4)
    float currentTime; // 0x2C (0x4)
    float currentTime2; // 0x30 (0x4)
    PAD(0xC); // 0x34 (0xC)
    int currentTick; // 0x40 (0x4)
    float currentTickRatio; // 0x44 (0x4)
    PAD(0x138); // 0x48 (0x138)
    char* currentMap; // 0x180 (0x8)
    char* currentMapName; // 0x188 (0x8)

    auto GetCurrentTickRounded() { 
        float ratio = currentTickRatio;
        int tick = currentTick;
        if (ratio >= 1.f - RATIO_THRESHOLD) 
            return std::make_pair(tick + 1, 0.f);
        else if (ratio <= RATIO_THRESHOLD) 
			return std::make_pair(tick, 0.f);
		return std::make_pair(tick, ratio);
    }
};

#define TICK_INTERVAL 1.f / 64.f
#define TICKS_TO_TIME(ticks) (TICK_INTERVAL * static_cast<float>(ticks))
#define TIME_TO_TICKS(time) static_cast<int>(0.5f + static_cast<float>(time) / TICK_INTERVAL)
