#include "pch.hpp"
#include <interfaces/enginetrace.hpp>

#include <virtual/virtual.hpp>
#include <signatures/signatures.hpp>

#include <bindings/playerpawn.hpp>

CEngineTrace* CEngineTrace::Get() { 
	static CEngineTrace* engineTrace = signatures::GetEngineTrace.GetPtrAs<CEngineTrace*>();
    return engineTrace;
}

bool CEngineTrace::TraceShape(Ray_t* ray, const Vector& start, const Vector& end, TraceFilter_t* filter, GameTrace_t* trace) {
    static auto TraceShapeFn = signatures::TraceShape.GetPtrAs<bool(__thiscall*)(CEngineTrace*, Ray_t*, const Vector&, const Vector&, TraceFilter_t*, GameTrace_t*)>();
    if (!TraceShapeFn) return false;
	return TraceShapeFn(this, ray, start, end, filter, trace);
}

bool CEngineTrace::TraceShape(const Vector& start, const Vector& end, C_BaseEntity* skip, uint64_t mask, char a6, GameTrace_t* trace) {
    static auto TraceShapeFn = signatures::TraceShape2.GetPtrAs<bool(__thiscall*)(CEngineTrace*, const Vector&, const Vector&, C_BaseEntity*, uint64_t, char, GameTrace_t*)>();
    if (!TraceShapeFn) return false;
    return TraceShapeFn(this, start, end, skip, mask, a6, trace);
}

GameTrace_t::GameTrace_t() { 
	//static auto Ctor = signatures::GameTraceCtor.GetPtrAs<void(__thiscall*)(GameTrace_t*)>();
	//if (Ctor) Ctor(this);
}

TraceFilter_t::TraceFilter_t(std::uint64_t mask, C_CSPlayerPawn* skip1, C_CSPlayerPawn* skip2, int layer) {
    vt = 0x0;
    this->mask = mask;
    this->v1[0] = this->v1[1] = 0;
    m_v2 = 7;
    m_v3 = layer;
    m_v4 = 0x49;
    m_v5 = 0;

    if (skip1 != nullptr) {
        skipHandles[0] = skip1->GetRefEHandle().GetEntryIndex();
        skipHandles[2] = skip1->GetOwnerHandleIndex();
        collisions[0] = skip1->m_pCollision()->collisionMask;
    }

    if (skip2 != nullptr) {
        skipHandles[0] = skip2->GetRefEHandle().GetEntryIndex();
        skipHandles[2] = skip2->GetOwnerHandleIndex();
        collisions[0] = skip2->m_pCollision()->collisionMask;
    }
}
