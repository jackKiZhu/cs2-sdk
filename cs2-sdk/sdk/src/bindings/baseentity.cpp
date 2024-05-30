#include "pch.hpp"

#include <bindings/baseentity.hpp>
#include <logger/logger.hpp>

#include <math/types/bbox.hpp>
#include <math/math.hpp>

#include <signatures/signatures.hpp>
#include <virtual/virtual.hpp>

#include <interfaces/schemasystem.hpp>
#include <interfaces/cvar.hpp>

#include <imgui/imgui_internal.h>

bool C_BaseEntity::IsPlayerController() {
    static CSchemaClassInfo* playerControllerClass =
        CSchemaSystem::Get()->FindTypeScopeForModule(CConstants::CLIENT_LIB)->FindDeclaredClass("CCSPlayerController");
    return Schema_DynamicBinding() == playerControllerClass;
}

bool C_BaseEntity::IsWeapon() {
    static CSchemaClassInfo* weaponBaseClass =
        CSchemaSystem::Get()->FindTypeScopeForModule(CConstants::CLIENT_LIB)->FindDeclaredClass("C_CSWeaponBase");

    CSchemaClassInfo* entClass = Schema_DynamicBinding();
    if (!entClass) return false;

    return entClass->InheritsFrom(weaponBaseClass);
}

bool C_BaseEntity::IsProjectile() {
    static CSchemaClassInfo* baseGrenadeClass =
        CSchemaSystem::Get()->FindTypeScopeForModule(CConstants::CLIENT_LIB)->FindDeclaredClass("C_BaseGrenade");

    CSchemaClassInfo* entClass = Schema_DynamicBinding();
    if (!entClass) return false;

    return entClass->InheritsFrom(baseGrenadeClass);
}

bool C_BaseEntity::IsPlantedC4() {
    static CSchemaClassInfo* plantedC4Class =
        CSchemaSystem::Get()->FindTypeScopeForModule(CConstants::CLIENT_LIB)->FindDeclaredClass("C_PlantedC4");
    return Schema_DynamicBinding() == plantedC4Class;
}

bool C_BaseEntity::IsChicken() {
    static CSchemaClassInfo* chickenClass =
        CSchemaSystem::Get()->FindTypeScopeForModule(CConstants::CLIENT_LIB)->FindDeclaredClass("C_Chicken");
    return Schema_DynamicBinding() == chickenClass;
}

bool C_BaseEntity::IsHostage() {
    static CSchemaClassInfo* hostageClass =
        CSchemaSystem::Get()->FindTypeScopeForModule(CConstants::CLIENT_LIB)->FindDeclaredClass("C_Hostage");
    return Schema_DynamicBinding() == hostageClass;
}

bool C_BaseEntity::IsPlayerPawn() { 
    static CSchemaClassInfo* basePlayerPawnClass =
        CSchemaSystem::Get()->FindTypeScopeForModule(CConstants::CLIENT_LIB)->FindDeclaredClass("C_BasePlayerPawn");
    CSchemaClassInfo* entClass = Schema_DynamicBinding();
    if (!entClass) return false;

    return entClass->InheritsFrom(basePlayerPawnClass);
}

bool C_BaseEntity::IsViewmodel() { 
    static CSchemaClassInfo* baseViewmodelClass =
        CSchemaSystem::Get()->FindTypeScopeForModule(CConstants::CLIENT_LIB)->FindDeclaredClass("C_BaseViewModel");
    CSchemaClassInfo* entClass = Schema_DynamicBinding();
    if (!entClass) return false;

    return entClass->InheritsFrom(baseViewmodelClass);
}

bool C_BaseEntity::IsViewmodelV() { 
    return vt::CallMethod<bool>(this, 242); 
}

bool C_BaseEntity::CalculateBBoxByCollision(BBox_t& out) {
    CGameSceneNode* node = m_pGameSceneNode();
    if (!node) return false;

    CCollisionProperty* collision = m_pCollision();
    if (!collision) return false;

    const Vector mins = collision->m_vecMins(), maxs = collision->m_vecMaxs();

    out.Invalidate();

    for (int i = 0; i < 8; ++i) {
        const Vector worldPoint{i & 1 ? maxs.x : mins.x, i & 2 ? maxs.y : mins.y, i & 4 ? maxs.z : mins.z};

        const matrix3x4_t& nodeToWorld = node->m_nodeToWorld().ToMatrix();
        if (!CMath::Get().WorldToScreen(worldPoint.Transform(&nodeToWorld), out.m_Vertices[i])) return false;

        out.m_Mins = ImMin(out.m_Mins, out.m_Vertices[i]);
        out.m_Maxs = ImMax(out.m_Maxs, out.m_Vertices[i]);
    }

    return true;
}

bool C_BaseEntity::CalculateBBoxByHitbox(BBox_t& out) {
    constexpr int MAX_HITBOXES = 64;

    CHitBoxSet* hitBoxSet = GetHitboxSet(0);
    if (!hitBoxSet) return false;

    const CUtlVector<CHitBox>& hitBoxes = hitBoxSet->m_HitBoxes();
    if (hitBoxes.m_Size > MAX_HITBOXES) {
        CLogger::Log("[bbox] hitBoxTransforms[{}] way too small! (Debug?)", MAX_HITBOXES);
        return false;
    }

    CTransform hitBoxTransforms[MAX_HITBOXES];
    int hitBoxCount = HitboxToWorldTransforms(hitBoxSet, hitBoxTransforms);
    if (!hitBoxCount) return false;

    Vector mins{std::numeric_limits<float>::max()}, maxs{-std::numeric_limits<float>::max()};
    for (int i = 0; i < hitBoxCount; ++i) {
        const matrix3x4_t hitBoxMatrix = hitBoxTransforms[i].ToMatrix();
        auto hitBox = hitBoxes.AtPtr(i);

        Vector worldMins, worldMaxs;
        CMath::Get().TransformAABB(hitBoxMatrix, hitBox->m_vMinBounds(), hitBox->m_vMaxBounds(), worldMins, worldMaxs);

        mins = mins.Min(worldMins);
        maxs = maxs.Max(worldMaxs);
    }

    out.Invalidate();

    for (int i = 0; i < 8; ++i) {
        const Vector worldPoint{i & 1 ? maxs.x : mins.x, i & 2 ? maxs.y : mins.y, i & 4 ? maxs.z : mins.z};

        if (!CMath::Get().WorldToScreen(worldPoint, out.m_Vertices[i])) return false;

        out.m_Mins = ImMin(out.m_Mins, out.m_Vertices[i]);
        out.m_Maxs = ImMax(out.m_Maxs, out.m_Vertices[i]);
    }

    return true;
}

CHitBoxSet* C_BaseEntity::GetHitboxSet(int i) { return signatures::GetHitboxSet.GetPtr().Call<CHitBoxSet* (*)(void*, int)>(this, i); }

int C_BaseEntity::HitboxToWorldTransforms(CHitBoxSet* hitBoxSet, CTransform* hitboxToWorld) {
    return signatures::HitboxToWorldTransforms.GetPtr().Call<int (*)(void*, CHitBoxSet*, CTransform*, int)>(this, hitBoxSet, hitboxToWorld,
                                                                                                            1024);
}

bool C_BaseEntity::GetHitboxPosition(int i, Vector& out) {    
    constexpr int MAX_HITBOXES = 64;

    CHitBoxSet* hitboxSet = GetHitboxSet(0);
    if (!hitboxSet) return false;

    CHitBox* hitbox = hitboxSet->m_HitBoxes().AtPtr(i);
    if (!hitbox) return false;

    CTransform hitBoxTransforms[MAX_HITBOXES];
    int hitBoxCount = HitboxToWorldTransforms(hitboxSet, hitBoxTransforms);
    if (!hitBoxCount) return false;

    const matrix3x4_t hitBoxMatrix = hitBoxTransforms[i].ToMatrix();
    Vector worldMins, worldMaxs;
    CMath::Get().TransformAABB(hitBoxMatrix, hitbox->m_vMinBounds(), hitbox->m_vMaxBounds(), worldMins, worldMaxs);

    out = (worldMins + worldMaxs) * 0.5f;
    return true; 
}

bool C_BaseEntity::IsEnemy(C_BaseEntity* other) { 
    static ConVar* mp_teammates_are_enemies = CCVar::Get()->GetCvarByName("mp_teammates_are_enemies");
    return mp_teammates_are_enemies->GetValue<bool>() ? true : m_iTeamNum() != other->m_iTeamNum();
}

void C_BaseEntity::SetOrigin(const Vector& pos) {
    static auto fn = signatures::SetOrigin.GetPtrAs<void (*)(void*, const Vector&)>();
    if (fn) fn(this, pos);
}

void C_BaseEntity::UpdateSubclass() {
    static auto fn = signatures::UpdateSubclass.GetPtrAs<void (*)(void*)>();
    if (fn) fn(this);
}
