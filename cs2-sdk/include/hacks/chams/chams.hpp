#pragma once

#include <array>

#include <types/handle.hpp>
#include <types/keyvalues.hpp>

class ISceneObjectDesc;
class IRenderContext;
class CMeshDrawPrimitive_t;
class ISceneView;
class ISceneLayer;
class SceneSystemPerFrameStats_t;
class CMaterial2;

class CChams {
   public:
    static CChams& Get() {
        static CChams inst;
        return inst;
    }

    enum MaterialType_t
    {
      MAT_BLOOM,
      MAT_GLOW, 
      MAT_GLOW2, 

      MAT_TYPE_COUNT
    };

    void Initialize();
    void Shutdown();

    bool IsEnabled();
   
    bool OnDrawObject(ISceneObjectDesc* const desc, IRenderContext* ctx, CMeshDrawPrimitive_t* renderList, int numRenderablesToDraw,
                      const ISceneView* view, ISceneLayer* layer, SceneSystemPerFrameStats_t* const perFrameStats,
                      const CMaterial2* material);

   private:
    bool OverrideMaterial(ISceneObjectDesc* const desc, IRenderContext* ctx, CMeshDrawPrimitive_t* renderList, int numRenderablesToDraw,
                          const ISceneView* view, ISceneLayer* layer, SceneSystemPerFrameStats_t* const perFrameStats,
                          const CMaterial2* material);

    std::array<CMaterial2*, MAT_TYPE_COUNT> materials;
};
