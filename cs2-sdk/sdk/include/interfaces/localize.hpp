#pragma once

class CLocalize {
   public:
    static CLocalize* Get();

    const char* FindSafe(const char* tokenName);
};
