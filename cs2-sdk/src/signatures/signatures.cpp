#include "pch.hpp"

#include <signatures/signatures.hpp>
#include <constants/constants.hpp>

#define STB_OMIT_TESTS
#include <stb/stb.hh>

#define SDK_SIG(sig) stb::simple_conversion::build<stb::fixed_string{sig}>::value

namespace signatures {
    CSigScan GetBaseEntity("CGameEntitySystem::GetBaseEntity", CConstants::CLIENT_LIB,
                           {
#ifdef _WIN32
                               {SDK_SIG("8B D3 E8 ? ? ? ? 48 8B F8 48 85 C0 74 76"), [](CPointer& ptr) { ptr.Absolute(3, 0); }},
                               {SDK_SIG("81 FA ? ? ? ? 77 36")},
#elif __linux__
                               {SDK_SIG("44 89 E6 E8 ? ? ? ? 48 89 C3 48 85 C0 0F 85 ? ? ? ?"), [](CPointer& ptr) { ptr.Absolute(4, 0); }},
#endif
                           });

    CSigScan GetHighestEntityIndex("CGameEntitySystem::GetHighestEntityIndex", CConstants::CLIENT_LIB,
                                   {
#ifdef _WIN32
                                       {SDK_SIG("33 DB E8 ? ? ? ? 8B 08"), [](CPointer& ptr) { ptr.Absolute(3, 0); }},
                                       {SDK_SIG("E8 ? ? ? ? 33 DB 39 5C 24 40"), [](CPointer& ptr) { ptr.Absolute(1, 0); }},
#elif __linux__
                                       {SDK_SIG("E8 ? ? ? ? 41 39 C4 7F 50"), [](CPointer& ptr) { ptr.Absolute(1, 0); }},
#endif
                                   });

    CSigScan GetCSGOInput("CCSGOInput", CConstants::CLIENT_LIB,
                          {
#ifdef _WIN32
                              {SDK_SIG("48 8D 0D ? ? ? ? E8 ? ? ? ? 66 0F 6F 05"), [](CPointer& ptr) { ptr.Absolute(3, 0); }},
#elif __linux__
                              {SDK_SIG("4C 8D 35 ? ? ? ? 48 8D 55 D4"), [](CPointer& ptr) { ptr.Absolute(3, 0).Dereference(1); }},
#endif
                          });

    // '%s:  %f tick(%d) curtime(%f)'
    CSigScan GetGlobalVars("GlobalVars", CConstants::CLIENT_LIB,
                          {
                               {SDK_SIG("48 89 0D ? ? ? ? 48 89 41"), [](CPointer& ptr) { ptr.Absolute(3, 0); }},
                          });

    CSigScan GetMatricesForView("CRenderGameSystem::GetMatricesForView", CConstants::CLIENT_LIB,
                                {
#ifdef _WIN32
                                    {SDK_SIG("40 53 48 81 EC ? ? ? ? 49 8B C1")},
                                    {SDK_SIG("48 89 44 24 ? E8 ? ? ? ? 48 8B 9C 24 ? ? ? ? B0 01"),
                                     [](CPointer& ptr) { ptr.Absolute(6, 0); }},
#elif __linux__
                                    {SDK_SIG("55 48 89 D7 4C 89 C2")},
#endif
                                });

    CSigScan GetHitboxSet("C_BaseEntity::GetHitboxSet", CConstants::CLIENT_LIB,
                          {
#ifdef _WIN32
                              {SDK_SIG("E8 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 44 8D 48 07"), [](CPointer& ptr) { ptr.Absolute(1, 0); }},
                              {SDK_SIG("41 8B D6 E8 ? ? ? ? 4C 8B F8"), [](CPointer& ptr) { ptr.Absolute(4, 0); }},
#elif __linux__
                              {SDK_SIG("E8 ? ? ? ? 48 89 85 ? ? ? ? 48 85 C0 74 0F"), [](CPointer& ptr) { ptr.Absolute(1, 0); }},
#endif
                          });

    CSigScan HitboxToWorldTransforms("C_BaseEntity::HitboxToWorldTransforms", CConstants::CLIENT_LIB,
                                     {
#ifdef _WIN32
                                         {SDK_SIG("E8 ? ? ? ? 45 33 F6 4C 63 E0"), [](CPointer& ptr) { ptr.Absolute(1, 0); }},
#elif __linux__
                                         {SDK_SIG("E8 ? ? ? ? 41 C7 45 ? ? ? ? ? 4C 89 F7"), [](CPointer& ptr) { ptr.Absolute(1, 0); }},
#endif
                                     });

    CSigScan GetFunctions("CSAC::GetFunctions", CConstants::CLIENT_LIB, 
        {
            {SDK_SIG("48 89 4C 24 ? 48 83 EC ? 48 8B 44 24 ? 48 83 78 ? ? 74 ? 48 8B 44 24")},
        });

    CSigScan GetFunctions2("CSAC::GetFunctions2", CConstants::CLIENT_LIB,
                           {
                               {SDK_SIG("48 89 4C 24 ? 48 83 EC ? 48 8B 44 24 ? 48 83 78 ? ? 74 ? E9")},
                           });

   CSigScan CSVCMsg_UserMessage_Setup("CSVCMsg_UserMessage_t::Setup", CConstants::CLIENT_LIB,
                          {
                              {SDK_SIG("40 53 57 41 56 48 83 EC ? 49 8B 00")},
                          });

      CSigScan GetUserCmd("GetUserCmd", CConstants::CLIENT_LIB,
                              {
                                  {SDK_SIG("40 57 48 83 EC ? 41 8B F8")},
                              });

            CSigScan SetViewAngles("SetViewAngles", CConstants::CLIENT_LIB,
                          {
                              {SDK_SIG("85 D2 0F 85 ? ? ? ? 57")},
                          });


    CSigScan CanFire("C_BasePlayerWeapon::CanFire", CConstants::CLIENT_LIB,
                             {
                                 {SDK_SIG("40 53 48 83 EC ? 48 8B 41 ? 48 8B D9 48 8D 4C 24 ? 8B 50 ? E8 ? ? ? ? F3 0F 10 05")},
                             });

        CSigScan GetEngineTrace("EngineTrace", CConstants::CLIENT_LIB,
                     {
                         {SDK_SIG("4C 8B 3D ? ? ? ? 24 C9 0C 49 66 0F 7F 45"), [](CPointer& ptr) { ptr.Absolute(3, 0).Dereference(); }},
                     });

        CSigScan TraceShape("CEngineTrace::TraceShape", CConstants::CLIENT_LIB,
                                {
                                    {SDK_SIG("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 48 89 4C 24 ? 55 41 54 41 55 41 56 41 57 48 8D AC 24")},
                                });

                CSigScan TraceShape2("CEngineTrace::TraceShape2", CConstants::CLIENT_LIB,
                            {
                                {SDK_SIG("4C 8B DC 49 89 5B ? 49 89 6B ? 49 89 73 ? 57 41 56 41 57 48 81 EC")},
                            });

        // E8 ? ? ? ? 48 85 C0 74 ? 44 38 60
        // 48 63 41 ? 48 8B 0D
        CSigScan GetSurfaceData("GetSurfaceData", CConstants::CLIENT_LIB,
                            {
                                {SDK_SIG("48 63 41 ? 48 8B 0D")},
                            });

        CSigScan GameTraceCtor("GameTraceCtor", CConstants::CLIENT_LIB,
                                {
                                    {SDK_SIG("48 89 5C 24 ? 57 48 83 EC ? 48 8B D9 33 FF 48 8B 0D")},
                                });

        CSigScan CTraceFilter("CTraceFilter", CConstants::CLIENT_LIB,
                               {
                {SDK_SIG("48 8D 05 ? ? ? ? 44 89 75 ? 48 89 45 ? 44 89 75 ? C7 45 ? ? ? ? ? 44 88 65 ? C6 45 ? ? 48 89 75"),
                 [](CPointer& ptr) { ptr.Absolute(3, 0); }},
                               });

        CSigScan FireEventClientSide("FireEventClientSide", CConstants::CLIENT_LIB,
                                {
                                    {SDK_SIG("48 89 5C 24 ? 56 57 41 54 48 83 EC 30 48 8B F2")},
                                });

        CSigScan AddStattrakEntity("AddStattrakEntity", CConstants::CLIENT_LIB,
                                     {
                                         {SDK_SIG("40 55 41 55 48 8D 6C 24 ? 48 81 EC ? ? ? ? 4C 8B E9")},
                                     });

        CSigScan GetInventoryManager("InventoryManager", CConstants::CLIENT_LIB,
                                     {
                                         {SDK_SIG("E8 ? ? ? ? 48 63 BB ? ? ? ? 48 8D 68 28 83 FF FF"),
                                          [](CPointer& ptr) {  ptr.Absolute(1, 0).Absolute(3, 0); }},
                                     });

        CSigScan GetCCSGCClientSystem("CCSGCClientSystem", CConstants::CLIENT_LIB,
                                     {
                                         {SDK_SIG("48 8D 0D ? ? ? ? FF 90 ? ? ? ? 84 C0"), [](CPointer& ptr) { ptr.Absolute(3, 0); }},
                                     });

        CSigScan SetModel("C_BaseEntityModel::SetModel", CConstants::CLIENT_LIB,
                                   {
                                       {SDK_SIG("48 89 5C 24 ? 48 89 7C 24 ? 55 48 8B EC 48 83 EC ? 48 8B F9 4C 8B C2")},
                                   });

        CSigScan FindSOCache("CCSGCClient::FindSOCache", CConstants::CLIENT_LIB,
                                      {
                                          {SDK_SIG("48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 54 41 56 41 57 48 83 EC ? 48 8D B9")},
                                      });

        CSigScan CreateBaseTypeCache("CreateBaseTypeCache", CConstants::CLIENT_LIB,
                             {
                                 {SDK_SIG("40 53 48 83 EC ? 4C 8B 49 ? 44 8B D2")},
                             });

        CSigScan CEconItem("CEconItem::CEconItem", CConstants::CLIENT_LIB,
                                     {
                                         {SDK_SIG("48 83 EC ? B9 ? ? ? ? E8 ? ? ? ? 48 85 C0 74 ? 48 8D 0D ? ? ? ? C7 40")},
                                     });

        CSigScan AddNametagEntity("AddNametagEntity", CConstants::CLIENT_LIB,
                           {
                               {SDK_SIG("40 55 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 4C 8B F9 4C 8D 05")},
                           });

        CSigScan SetDynamicAttributeValueUInt("SetDynamicAttributeValueUInt", CConstants::CLIENT_LIB,
                                  {
                                      {SDK_SIG("49 8B C0 48 8B CA 48 8B D0 4D 8B C1 E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 49 8B C0 48 8B CA 48 8B D0")},
                                  });

        CSigScan SetMeshGroupMask("SetMeshGroupMask", CConstants::CLIENT_LIB,
                                              {
                                                  {SDK_SIG("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8D 99 ? ? ? ? 48 8B 71")},
                                              });

        // nuko is the greatest
        CSigScan GetSortedItemDefinitionMap("GetSortedItemDefinitionMap", CConstants::CLIENT_LIB,
                                  {
                                                {SDK_SIG("49 8B 80 ? ? ? ? 48 63 CB 48 C1 E1 ? 8B 14 01 83 FA"), [](CPointer& ptr) { ptr.Offset(3).Dereference(); }},
                                  });

        // xref: "%s_large", "Alternate icon definition index '%s' (%llu) must be greater than zero"
        CSigScan GetAlternateIconsMap("GetAlternateIconsMap", CConstants::CLIENT_LIB,
                                  {
                                                {SDK_SIG("48 8B 8F ? ? ? ? 48 8D 15 ? ? ? ? 48 83 C1"), [](CPointer& ptr) { ptr.Offset(3).Dereference(); }},
                                  });

        CSigScan GetPaintKits("GetPaintKits", CConstants::CLIENT_LIB,
                                  {
                                                {SDK_SIG("49 8B 83 ? ? ? ? 45 39 04 02 74 ? 49 8B 7C 02"), [](CPointer& ptr) { ptr.Offset(3).Dereference(); }},
                                  });

        CSigScan FindViewmodelMaterial("FindViewmodelMaterial", CConstants::CLIENT_LIB,
                              {
                                  {SDK_SIG("48 81 C1 ? ? ? ? 45 33 C9 41 B8 ? ? ? ? E8 ? ? ? ? 4C 63 F0 49 C1 E6 ? 4C 03 B7")},
                              });

       CSigScan IsLoadoutAllowed("IsLoadoutAllowed", CConstants::CLIENT_LIB,
            {
                {SDK_SIG("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 0D ? ? ? ? 48 8B 01")},
            });

       CSigScan GetInaccuracy("GetInaccuracy", CConstants::CLIENT_LIB,
                                 {
                                     {SDK_SIG("48 89 5C 24 ? 57 48 81 EC ? ? ? ? 0F 29 B4 24 ? ? ? ? 48 8B D9")},
                                 });

       CSigScan GetWeaponData("GetWeaponData", CConstants::CLIENT_LIB,
                            {
                                {SDK_SIG("48 81 EC ? ? ? ? 48 85 C9 75 ? 33 C0 48 81 C4 ? ? ? ? C3 48 89 9C 24")},
                            });

        CSigScan GrenadePtr("GrenadePtr", CConstants::CLIENT_LIB,
                                    {
                                        {SDK_SIG("48 8D 0D ? ? ? ? E8 ? ? ? ? 48 85 C0 75 ? 48 8B 05 ? ? ? ? 48 8B 40 ? 80 38 ? 0F 84 ? ? ? ? 8B 05"),
                                         [](CPointer& ptr) { ptr.Absolute(3, 0); }},
                                    });

       CSigScan GetBool("GetBool", CConstants::CLIENT_LIB,
                              {
                                  {SDK_SIG("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B 71 ? 8B DA 48 8B 4E ? 48 8D 7E ? 48 0F BA E1 ? 72 ? 85 D2 7F ? 48 8B C7")},
                              });

       CSigScan LineGoesThroughSmoke("LineGoesThroughSmoke", CConstants::CLIENT_LIB,
                              {
                                  {SDK_SIG("48 8B C4 55 56 57 41 54 41 55 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 44 0F 29 40 ? 49 8B F8 44 0F 29 48 ? 45 0F 57 C9 4C 8B FA")},
                              });

       CSigScan SetButtonStates("SetButtonStates", CConstants::CLIENT_LIB,
                                     {
                                         {SDK_SIG("48 89 5C 24 ? 48 8B D9 41 BA ? ? ? ? 41 F6 C0")},
                                     });

       CSigScan MoveData("MoveData", CConstants::CLIENT_LIB,
                                {
                                    {SDK_SIG("40 53 56 41 56 48 83 EC ? 48 8B F1 48 63 CA")},
                                });

       CSigScan MemAlloc("g_pMemAlloc", CConstants::CLIENT_LIB,
                         {
                             {SDK_SIG("48 8B 05 ? ? ? ? 48 8B 08 48 8B 01 FF 50 ? 45 33 E4"), [](CPointer& ptr) { ptr.Absolute(3, 0); }},
                         });

       CSigScan FindKeyVar("FindKeyVar", CConstants::PARTICLES_LIB,
                         {
                             {SDK_SIG("48 89 5C 24 ? 57 48 81 EC ? ? ? ? 33 C0 8B DA")},
                         });

       CSigScan SetMaterialShaderType("SetMaterialShaderType", CConstants::PARTICLES_LIB,
                         {
                             {SDK_SIG("E8 ? ? ? ? 48 8D 05 ? ? ? ? C7 44 24 ? ? ? ? ? 41 B1 ? 48 89 44 24 ? 4C 8B C7 48 8D 54 24 ? 48 8B CE"), [](CPointer& ptr) { ptr.Absolute(1, 0); }},
                         });

       CSigScan SetMaterialFunction(
           "SetMaterialFunction", CConstants::PARTICLES_LIB,
           {
               {SDK_SIG("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 83 EC ? 0F B6 01 45 0F B6 F9 8B 2A 48 8B F9")},
           });

}  // namespace signatures
