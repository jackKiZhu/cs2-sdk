#include "pch.hpp"
#include <interfaces/enginetrace.hpp>

#include <virtual/virtual.hpp>
#include <signatures/signatures.hpp>

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
