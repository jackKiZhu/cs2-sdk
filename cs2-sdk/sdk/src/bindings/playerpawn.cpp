#include "pch.hpp"

#include <bindings/playerpawn.hpp>
#include <bindings/weapon.hpp>

#include <interfaces/schemasystem.hpp>

bool C_CSPlayerPawnBase::IsPlayerPawn() {
    static CSchemaClassInfo* playerPawnClass =
        CSchemaSystem::Get()->FindTypeScopeForModule(CConstants::CLIENT_LIB)->FindDeclaredClass("C_CSPlayerPawn");
    return Schema_DynamicBinding() == playerPawnClass;
}

bool C_CSPlayerPawnBase::IsObserverPawn() {
    static CSchemaClassInfo* observerPawnClass =
        CSchemaSystem::Get()->FindTypeScopeForModule(CConstants::CLIENT_LIB)->FindDeclaredClass("C_CSObserverPawn");
    return Schema_DynamicBinding() == observerPawnClass;
}

C_BasePlayerWeapon* C_BasePlayerPawn::GetActiveWeapon() { 
    CPlayerWeaponServices* weaponServices = m_pWeaponServices();
    if (!weaponServices) return nullptr;
    return weaponServices->m_hActiveWeapon().Get();
}
