#pragma once

class CVisuals {
   public:
    static CVisuals& Get() {
        static CVisuals inst;
        return inst;
    }

    void OnFrameStageNotify(int stage);

   private:
    float minExposureBackup;
    float maxExposureBackup;
};
