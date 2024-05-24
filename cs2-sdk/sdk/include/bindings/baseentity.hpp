#pragma once

#include <bindings/entityinstance.hpp>
#include <bindings/gamescenenode.hpp>

#include <bindings/collisionproperty.hpp>
#include <bindings/hitbox.hpp>

#include <virtual/virtual.hpp>

struct BBox_t;

class C_BaseEntity : public CEntityInstance {
   public:
    bool IsPlayerController();
    bool IsWeapon();
    bool IsProjectile();
    bool IsPlantedC4();
    bool IsChicken();
    bool IsHostage();
    bool IsPlayerPawn();
    bool IsViewmodel();

    bool IsViewmodelV(); // 242

    //bool IsPlayerControllerV(); 144
    //bool IsPlayerPawnV(); 147
    //bool IsWeaponV(); 150

    bool CalculateBBoxByCollision(BBox_t& out);
    bool CalculateBBoxByHitbox(BBox_t& out);

    CHitBoxSet* GetHitboxSet(int i);
    int HitboxToWorldTransforms(CHitBoxSet* hitBoxSet, CTransform* hitboxToWorld);
    bool GetHitboxPosition(int i, Vector& out);

    SCHEMA(CGameSceneNode*, m_pGameSceneNode, "C_BaseEntity", "m_pGameSceneNode");
    SCHEMA(CBaseHandle, m_hOwnerEntity, "C_BaseEntity", "m_hOwnerEntity");
    SCHEMA(CBaseHandle, m_hGroundEntity, "C_BaseEntity", "m_hGroundEntity");
    SCHEMA(CCollisionProperty*, m_pCollision, "C_BaseEntity", "m_pCollision");
    SCHEMA(uint8_t, m_iTeamNum, "C_BaseEntity", "m_iTeamNum");
    SCHEMA(uint32_t, m_fFlags, "C_BaseEntity", "m_fFlags");
    SCHEMA(int, m_iHealth, "C_BaseEntity", "m_iHealth");
    SCHEMA(int, m_nSubclassID, "C_BaseEntity", "m_nSubclassID");
    SCHEMA(int, m_nSimulationTick, "C_BaseEntity", "m_nSimulationTick");
    SCHEMA(float, m_flSimulationTime, "C_BaseEntity", "m_flSimulationTime");
    SCHEMA(Vector, m_vecVelocity, "C_BaseEntity", "m_vecVelocity");
    SCHEMA(Vector, m_vecAbsVelocity, "C_BaseEntity", "m_vecAbsVelocity");
    SCHEMA_EXTENDED(void*, GetVData, CConstants::CLIENT_LIB, "C_BaseEntity", "m_nSubclassID", 0x8);

    VIRTUAL_METHOD(void, GetEyePos, 166, (Vector* pos), pos); // 166 ?
    VIRTUAL_METHOD(void, GetEyeAngles, 167, (Vector* angles), angles);

    bool IsEnemy(C_BaseEntity* other);
    void SetOrigin(const Vector& pos);
};
