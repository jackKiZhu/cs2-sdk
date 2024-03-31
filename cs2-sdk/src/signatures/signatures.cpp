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
                               {SDK_SIG("48 89 0D ?? ?? ?? ?? 48 89 41"), [](CPointer& ptr) { ptr.Absolute(3, 0).Dereference(); }},
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
}  // namespace signatures
