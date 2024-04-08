#pragma once

class CGCClientSharedObjectCache;

struct SOID_t {
    uint64_t id;
    uint32_t type;
    PAD(0x4);
};

class CSGCClient {
   public:
    CGCClientSharedObjectCache* FindSOCache(SOID_t id, bool createIfMissing = true);
};

class CSGCClientSystem {
   public:
    static CSGCClientSystem* Get();

    CSGCClient* GetSGCClient();
};
