#include "pch.hpp"

#include <interfaces/cgameevent.hpp>

#include <constants/constants.hpp>
#include <memory/memory.hpp>

#include <virtual/virtual.hpp>

const char* CGameEvent::GetName() { 
	return vt::CallMethod<const char*>(this, 1); 
}

CCSPlayerController* CGameEvent::GetPlayerController(const char* key) { 
	GameEventStruct_t unk(key);
	return vt::CallMethod<CCSPlayerController*>(this, 16, &unk); 
}
