#pragma once

class CEconItemSystem;

class CSource2Client {
   public:
    static CSource2Client* Get();

    CEconItemSystem* GetEconItemSystem();
};
