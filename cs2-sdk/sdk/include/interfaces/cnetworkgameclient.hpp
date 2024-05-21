#pragma once

#include <virtual/virtual.hpp>
#include <memory/memory.hpp>
#include <constants/constants.hpp>

class CNetworkGameClient;

class INetworkGameClient {
   public:
    static INetworkGameClient* Get() {
        static const auto inst = CMemory::GetInterface(CConstants::ENGINE_LIB, "NetworkClientService_001");
        return inst.Get<INetworkGameClient*>();
    }

    PAD(0x250);
    int maxClients;
    PAD(0x8);
    int deltaTick;

    VIRTUAL_METHOD(CNetworkGameClient*, GetNetworkClient, 23, ());
};

class CNetworkGameClient {
   public:
    VIRTUAL_METHOD(float, GetClientInterp, 62, ());
};
