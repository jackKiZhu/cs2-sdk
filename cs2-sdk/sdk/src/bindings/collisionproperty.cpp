#include <pch.hpp>
#include <schemamgr/schema_manager.hpp>

#include <math/types/vector.hpp>

#include <memory/memory.hpp>
#include <signatures/signatures.hpp>

#include <bindings/collisionproperty.hpp>

void CCollisionProperty::SetCollisionBounds(const Vector& mins, const Vector& maxs) {
    static auto fn = signatures::SetCollisionBounds.GetPtrAs<void(*)(CCollisionProperty*, const Vector&, const Vector&)>();
    if (fn) fn(this, mins, maxs);
}
