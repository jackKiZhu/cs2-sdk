#pragma once

#include <types/econitemschema.hpp>

class CEconItemSystem {
   public:
    auto GetEconItemSchema() { return *reinterpret_cast<CEconItemSchema**>((uintptr_t)(this) + 0x8); }
};
