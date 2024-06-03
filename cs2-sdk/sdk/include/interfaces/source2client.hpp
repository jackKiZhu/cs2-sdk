#pragma once

class CEconItemSystem;

class CSource2Client {
   public:
    static CSource2Client* Get();

    CEconItemSystem* GetEconItemSystem();
};

// 48 8B C2 C3 CC CC CC CC CC CC CC CC CC CC CC CC 48 8B 0D cool gadget
