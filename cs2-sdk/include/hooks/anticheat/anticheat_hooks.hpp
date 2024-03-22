class CAntiCheatHooks {
   public:
    static CAntiCheatHooks& Get() {
        static CAntiCheatHooks inst;
        return inst;
    }

    void Initialize();
};
