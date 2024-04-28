#include "pch.hpp"

#include <interfaces/cgameevent.hpp>

#include <constants/constants.hpp>
#include <memory/memory.hpp>

#include <virtual/virtual.hpp>

const char* CGameEvent::GetName() { 
	return vt::CallMethod<const char*>(this, 1); 
}

bool CGameEvent::GetBool(const std::string_view token) { 
	KeyString_t str(token.data());
	return vt::CallMethod<bool>(this, 6, &token, false);
}

int CGameEvent::GetIntFromHash(const std::string_view token) {
    KeyString_t str(token.data());
    return vt::CallMethod<int>(this, 7, &token, 0);
}

float CGameEvent::GetFloat(const std::string_view token) { 
		KeyString_t str(token.data());
    return vt::CallMethod<float>(this, 9, &token, 0.f);
}

const char* CGameEvent::GetString(const std::string_view token) { 
		KeyString_t str(token.data());
    return vt::CallMethod<const char*>(this, 10, &token, nullptr);
}

CCSPlayerController* CGameEvent::GetPlayerController(const std::string_view token) { 
		KeyString_t str(token.data());
    return vt::CallMethod<CCSPlayerController*>(this, 16, &token);
}

// client.dll; 48 89 5C 24 08 48 89 74 24 10 48 89 7C 24 18 41 56 48 83 EC 30 48 8B 01 41 8B F0 4C 8B F1 41 B0 01 48 8D 4C 24 20 48 8B DA 48
// 8B 78
int64_t CGameEvent::GetInt(const std::string_view eventName) { return 0; }

