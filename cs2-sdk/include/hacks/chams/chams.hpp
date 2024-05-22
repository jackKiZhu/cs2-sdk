#pragma once

#include <array>

#include <types/handle.hpp>
#include <types/keyvalues.hpp>

class CSceneData;
class CMaterial2;

class CChams {
   public:
    static CChams& Get() {
        static CChams inst;
        return inst;
    }

    enum MaterialType_t
    {
      MAT_FLAT,
      MAT_FLAT_Z, 

      MAT_REGULAR,
      MAT_REGULAR_Z,

      MAT_TYPE_COUNT
    };

    void Initialize();
    void Shutdown();

    bool IsEnabled();
   
    bool OnDrawObject(void* animatableSceneObjectDesc, void* dx11, CSceneData* meshDraw, int dataCount, void* sceneView, void* sceneLayer,
                      void* unk, void* unk2);

    CStrongHandle<CMaterial2> CreateMaterial(const char* name);

   private:
    CMaterial2* CreateMaterial(const char* name, const char* materialVMAT, const char* shaderType, bool blendMode, bool translucent, bool disableZ);

    bool OverrideMaterial(void* animatableSceneObjectDesc, void* dx11, CSceneData* meshDraw, int dataCount, void* sceneView,
                          void* sceneLayer, void* unk, void* unk2);

    std::array<CMaterial2*, MAT_TYPE_COUNT> materials;
};
