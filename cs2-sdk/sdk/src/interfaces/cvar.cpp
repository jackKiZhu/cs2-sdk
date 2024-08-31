#include "pch.hpp"

#include <interfaces/cvar.hpp>
#include <logger/logger.hpp>

#include <memory/memory.hpp>
#include <constants/constants.hpp>

#include <hash/fnv1a.hpp>

#include <virtual/virtual.hpp>

CCVar* CCVar::Get() {
    static const auto inst = CMemory::GetInterface(CConstants::TIER_LIB, "VEngineCvar007");
    return inst.Get<CCVar*>();
}

void CCVar::UnlockHiddenConvars() {
    for (int i = m_ConVars.Head(); i != m_ConVars.InvalidIndex(); i = m_ConVars.Next(i)) {
        ConVar* convar = m_ConVars.Element(i);
        if (convar == nullptr) continue;
        convar->m_nFlags &= ~(FCVAR_HIDDEN | FCVAR_DEVELOPMENTONLY);
    }
}

CCVar::CVarIterator_t CCVar::GetFirstCvarIterator() {
    CVarIterator_t iterator = -1;

#ifdef _WIN32
    vt::CallMethod<void>(this, 12, &iterator);
#elif __linux__
    iterator = vt::CallMethod<CVarIterator_t>(this, 12);
#endif

    return iterator;
}

CCVar::CVarIterator_t CCVar::GetNextCvarIterator(CVarIterator_t prev) {
    CVarIterator_t iterator = prev;

#ifdef _WIN32
    vt::CallMethod<void>(this, 13, &iterator, prev);
#elif __linux__
    iterator = vt::CallMethod<CVarIterator_t>(this, 13, prev);
#endif

    return iterator;
}

ConVar* CCVar::GetCvarByIndex(CVarIterator_t index) { return vt::CallMethod<ConVar*>(this, 37, index); }

ConVar* CCVar::GetCvarByName(const char* name) {
    const uint32_t hash = FNV1A::Hash(name);

    CVarIterator_t it = GetFirstCvarIterator();
    do {
        ConVar* cvar = GetCvarByIndex(it);

        if (cvar && FNV1A::Hash(cvar->m_Name) == hash) {
            CLogger::Log("[cvar] {} -> {}", name, SDK_LOG_PTR(cvar));
            return cvar;
        }

        it = GetNextCvarIterator(it);
    } while ((int16_t)it != -1);

    CLogger::Log("[cvar] Couldn't find cvar named '{}'.", name);
    return nullptr;
}

ConVar* CCVar::GetCvarByHash(uint32_t hash) { 
  	for (int i = m_ConVars.Head(); i != m_ConVars.InvalidIndex(); i = m_ConVars.Next(i)) {
        ConVar* convar = m_ConVars.Element(i);
            if (convar == nullptr) continue;
        if (FNV1A::Hash(convar->m_Name) == hash) return convar;
    }
    return nullptr;
}
