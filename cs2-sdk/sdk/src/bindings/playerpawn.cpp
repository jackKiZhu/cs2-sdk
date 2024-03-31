#include "pch.hpp"

#include <bindings/playerpawn.hpp>
#include <bindings/weapon.hpp>

#include <interfaces/schemasystem.hpp>
#include <interfaces/globalvars.hpp>

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

C_CSWeaponBaseGun* C_BasePlayerPawn::GetActiveWeapon() { 
    CCSPlayer_WeaponServices* weaponServices = m_pWeaponServices();
    if (!weaponServices) return nullptr;
    return static_cast<C_CSWeaponBaseGun*>(weaponServices->m_hActiveWeapon().Get());
}

bool C_CSPlayerPawnBase::CanAttack(const float serverTime) { 
    if (CCSPlayer_WeaponServices* weaponServices = m_pWeaponServices(); weaponServices)
        if (m_bWaitForNoAttack() || weaponServices->m_flNextAttack() > serverTime) 
            return false;
    return true;
}
