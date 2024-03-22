#pragma once

class CMovement {
   public:
    static CMovement& Get() {
        static CMovement inst;
        return inst;
    }

    bool IsEnabled();

    void Run();
    
};
