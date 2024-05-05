#pragma once

#include <sigscan/sigscan.hpp>

namespace signatures {
    extern CSigScan GetBaseEntity, GetHighestEntityIndex;
    extern CSigScan GetCSGOInput;
    extern CSigScan GetGlobalVars;
    extern CSigScan GetMatricesForView;
    extern CSigScan GetHitboxSet;
    extern CSigScan HitboxToWorldTransforms;
    extern CSigScan GetFunctions;
    extern CSigScan GetFunctions2;
    extern CSigScan CSVCMsg_UserMessage_Setup;
    extern CSigScan GetUserCmd;
    extern CSigScan SetViewAngles;
    extern CSigScan CanFire;
    extern CSigScan GetEngineTrace;
    extern CSigScan TraceShape;
    extern CSigScan TraceShape2;
    extern CSigScan GetSurfaceData;
    extern CSigScan GameTraceCtor;
    extern CSigScan CTraceFilter;
    extern CSigScan FireEventClientSide;
    extern CSigScan AddStattrakEntity;
    extern CSigScan GetInventoryManager;
    extern CSigScan GetCCSGCClientSystem;
    extern CSigScan SetModel;
    extern CSigScan FindSOCache;
    extern CSigScan CreateBaseTypeCache;
    extern CSigScan CEconItem;
    extern CSigScan AddNametagEntity;
    extern CSigScan SetDynamicAttributeValueUInt;
    extern CSigScan SetMeshGroupMask;
    extern CSigScan GetSortedItemDefinitionMap;
    extern CSigScan GetAlternateIconsMap;
    extern CSigScan GetPaintKits;
    extern CSigScan FindViewmodelMaterial;
    extern CSigScan IsLoadoutAllowed;
    extern CSigScan GetInaccuracy;
    extern CSigScan GetWeaponData;
    extern CSigScan GrenadePtr;
    extern CSigScan GetBool;
    extern CSigScan LineGoesThroughSmoke;
    extern CSigScan SetButtonStates;
    extern CSigScan MoveData;
}  // namespace signatures
