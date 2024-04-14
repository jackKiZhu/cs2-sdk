#pragma once

class CMeshData;
class CMaterial2;

class CChams {
   public:
    static CChams& Get() {
        static CChams inst;
        return inst;
    }

    void Initialize();
    void Shutdown();

    bool IsEnabled();
   
    bool OnDrawObject(void* animatableSceneObjectDesc, void* dx11, CMeshData* meshDraw, int dataCount, void* sceneView, void* sceneLayer,
                      void* unk, void* unk2);

   private:
    CMaterial2* CreateMaterial(const char* name, const char* materialVMAT, const char* shaderType, bool blendMode, bool translucent,
                               bool disableZ);
    bool OverrideMaterial(void* animatableSceneObjectDesc, void* dx11, CMeshData* meshDraw, int dataCount, void* sceneView,
                          void* sceneLayer, void* unk, void* unk2);

    bool initialized;
};
