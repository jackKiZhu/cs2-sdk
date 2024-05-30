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
    PAD(0x1C); // 0x14 (0x1C)
    float intervalPerSubtick;  // 0x30 (0x4)
    float currentTime;         // 0x34 (0x4)
    float currentTime2;        // 0x38 (0x4)
    PAD(0xC); 
    int currentTick;  // 0x48 (0x4)
    float currentTickRatio;  // 0x4C (0x4)
    void* netChannel;        
    PAD(0x158);          
    char* currentMap;
    char* currentMapName; 

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
