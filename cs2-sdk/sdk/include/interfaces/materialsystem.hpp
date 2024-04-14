#pragma once

class CMaterial2
{
  public:
    virtual const char* GetName() = 0;
    virtual const char* GetSharedName() = 0;
};

struct MaterialKeyVar_t
{
    MaterialKeyVar_t(uint64_t key, const char* name) : key(key), name(name) {}
    MaterialKeyVar_t(const char *name, bool findKey = false) : name(name)
    {
      if ( findKey )
      {
        key = 0;
      }
    }

    uint64_t key;
    const char* name;

    uint64_t FindKey(const char* name);
};

class CObjectInfo
{
    PAD(0xB0);

   public:
    int id;
};

class CSceneAnimatableObject
{
    PAD(0xB0);
   public:
    int owner;

};

class CMaterialSystem {
   public:
    static CMaterialSystem* Get();
};
