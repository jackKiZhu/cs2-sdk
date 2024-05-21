#include <pch.hpp>

#include <hacks/aimbot/lagcomp.hpp>
#include <hacks/global/global.hpp>

#include <interfaces/enginetrace.hpp>
#include <interfaces/ccsgoinput.hpp>
#include <interfaces/globalvars.hpp>
#include <interfaces/cvar.hpp>

#include <bindings/playerpawn.hpp>
#include <bindings/playercontroller.hpp>
#include <bindings/gamescenenode.hpp>
#include <bindings/collisionproperty.hpp>

#include <math/types/vector.hpp>

#include <cache/entities/player.hpp>
#include <math/math.hpp>

#include <logger/logger.hpp>
#include <vars/vars.hpp>

#include <algorithm>

void CRecord::Store(C_CSPlayerPawn* pawn) {
    valid = false;

    CGameSceneNode* gameSceneNode = pawn->m_pGameSceneNode();
    if (!gameSceneNode) return;

    CSkeletonInstance* skeleton = gameSceneNode->GetSkeleton();
    if (!skeleton) return;

    CCollisionProperty* collision = pawn->m_pCollision();
    if (!collision) return;

    this->mins = collision->m_vecMins();
    this->maxs = collision->m_vecMaxs();
    this->simulationTime = pawn->m_flSimulationTime();
    this->flags = pawn->m_fFlags();
    this->eyeAngles = pawn->m_angEyeAngles();
    this->rotation = gameSceneNode->m_angRotation();
    this->absRotation = gameSceneNode->m_angAbsRotation();
    this->velocity = pawn->m_vecVelocity();
    this->absVelocity = pawn->m_vecAbsVelocity();
    this->origin = gameSceneNode->m_vecOrigin();
    this->absOrigin = gameSceneNode->m_vecAbsOrigin();
    pawn->GetEyePos(&this->eyePos);

    skeleton->CalculateWorldSpaceBones(FLAG_HITBOX);
    const uint32_t boneCount = skeleton->m_modelState().m_hModel().Get()->BoneCount();
    if (boneCount > 0) {
        this->boneMatrix.resize(boneCount);
        memcpy(this->boneMatrix.data(), skeleton->m_modelState().bones, sizeof(BoneData_t) * boneCount);
        valid = true;
    }
}

void CRecord::Apply(C_CSPlayerPawn* pawn, bool clean) {
    if (!valid) return;

    CGameSceneNode* gameSceneNode = pawn->m_pGameSceneNode();
    if (!gameSceneNode) return;

    CSkeletonInstance* skeleton = gameSceneNode->GetSkeleton();
    if (!skeleton) return;

    CCollisionProperty* collision = pawn->m_pCollision();
    if (!collision) return;

    collision->SetCollisionBounds(this->mins, this->maxs);

    pawn->m_flSimulationTime() = this->simulationTime;
    pawn->m_fFlags() = this->flags;

    pawn->m_angEyeAngles() = this->eyeAngles;
    gameSceneNode->m_angRotation() = this->rotation;
    gameSceneNode->m_angAbsRotation() = this->absRotation;

    pawn->m_vecVelocity() = this->velocity;
    pawn->m_vecAbsVelocity() = clean ? this->absVelocity : this->velocity;

    gameSceneNode->m_vecOrigin() = this->origin;
    pawn->SetOrigin(clean ? this->absOrigin : this->origin);

    const uint32_t boneCount = skeleton->m_modelState().m_hModel().Get()->BoneCount();
    if (boneCount > 0 && this->boneMatrix.size() >= boneCount)
        memcpy(skeleton->m_modelState().bones, this->boneMatrix.data(), sizeof(BoneData_t) * boneCount);
}

TargetData_t CLagComp::Find() {
    const Vector end = CGlobal::Get().eyePos + CGlobal::Get().rcsAngles.ToVector().Normalized() * 4096.f;

    float bestRecordDistance = std::numeric_limits<float>::max();
    data = {};

    const std::lock_guard<std::mutex> lock(CMatchCache::GetLock());
    for (const auto& it : CMatchCache::GetCachedEntities()) {
        const auto& cachedEntity = it.second;
        if (!cachedEntity->IsValid() || cachedEntity->GetType() != CCachedBaseEntity::Type::PLAYER) continue;

        CCachedPlayer* cachedPlayer = dynamic_cast<CCachedPlayer*>(cachedEntity.get());
        if (!cachedPlayer || !cachedPlayer->IsEnemyWithTeam(CGlobal::Get().player->GetTeam())) continue;

        CCSPlayerController* controller = cachedPlayer->Get();
        if (!controller->m_bPawnIsAlive()) continue;

        C_CSPlayerPawn* pawn = controller->m_hPawn().Get();
        if (!pawn || pawn->m_bGunGameImmunity()) continue;

        Vector eyePos;
        pawn->GetEyePos(&eyePos);
        const float distance = eyePos.DistTo(CGlobal::Get().eyePos);
        if (CGlobal::Get().vdata && distance > CGlobal::Get().vdata->m_flRange()) continue;

        GameTrace_t trace;
        if (!CEngineTrace::Get()->TraceShape(CGlobal::Get().eyePos, eyePos, CGlobal::Get().pawn, 0x1C1003, 4, &trace) ||
            trace.fraction < 0.97f) {
            cachedPlayer->Reset();
            continue;
        }

        cachedPlayer->visibleSince += CGlobalVars::Get()->intervalPerTick;
        if (cachedPlayer->visibleSince < 0.15f) continue;

        const Vector eyeDir = pawn->m_angEyeAngles().ToVector().Normalized();

        size_t i = 0;
        if (!g_Vars.m_Backtrack)
          i = cachedPlayer->records.size() - 2;
        for (; i < cachedPlayer->records.size(); ++i) {
            CRecord& record = cachedPlayer->records[i];
            if (!record.IsValid()) continue;

            const Vector& bonePos = record.boneMatrix[BONE_HEAD_0].position;
            // TODO: check for visibility for the record and not the player.
            Vector angle = CMath::Get().CalculateAngle(CGlobal::Get().eyePos, bonePos);
            cachedPlayer->dot = -eyeDir.DotProduct(angle.ToVector().Normalized());

            const float distance = CMath::Get().DistanceFromRay(record.eyePos, CGlobal::Get().eyePos, end);
            if (distance >= bestRecordDistance) continue;

            bestRecordDistance = distance;
            data.player = cachedPlayer;
            data.controller = controller;
            data.pawn = pawn;
            data.records = &cachedPlayer->records;
            data.bestRecord = &record;
            data.bestRecordIndex = i;
            data.aimPos = bonePos;
        }
    }

    if (!data.player) return data;

    data.bestBone = BONE_HEAD_0;

#if 0
    CRecord backup(data.pawn);
    data.bestRecord->Apply(data.pawn);
    for (int bone = 0; bone < data.player->boneCount)
    data.aimPos = .
    backup->Restore(data.pawn);
#endif

    return data;
}

void CLagComp::Update() {
    const float lastValidSimtime = LastValidSimtime();

    const std::lock_guard<std::mutex> lock(CMatchCache::GetLock());
    for (const auto& it : CMatchCache::GetCachedEntities()) {
        const auto& cachedEntity = it.second;
        if (!cachedEntity->IsValid() || cachedEntity->GetType() != CCachedBaseEntity::Type::PLAYER) continue;

        CCachedPlayer* cachedPlayer = dynamic_cast<CCachedPlayer*>(cachedEntity.get());
        if (!cachedPlayer || !cachedPlayer->IsEnemyWithTeam(CGlobal::Get().player->GetTeam())) {
            continue;
        }

        CCSPlayerController* controller = cachedPlayer->Get();
        if (!controller->m_bPawnIsAlive()) {
            cachedPlayer->records.clear();
            continue;
        }

        C_CSPlayerPawn* pawn = controller->m_hPawn().Get();
        if (!pawn || pawn->m_bGunGameImmunity()) {
            cachedPlayer->records.clear();
            continue;
        }

        for (CRecord& record : cachedPlayer->records)
            if (record.valid && record.simulationTime < lastValidSimtime) record.valid = false;

        CRecord record(pawn);
        if (record.valid) cachedPlayer->records.push_back(std::move(record));

        // remove every record that are invalid
        std::erase_if(cachedPlayer->records, [](const CRecord& record) { return !record.valid; });

        const size_t size = cachedPlayer->records.size();

        #if 0
        cachedPlayer->records.erase(std::unique(cachedPlayer->records.begin(), cachedPlayer->records.end(),
                                                [](const CRecord& a, const CRecord& b) { return a.simulationTime == b.simulationTime && a.origin == b.origin; }),
                                    cachedPlayer->records.end());
        #endif

        if (size != cachedPlayer->records.size()) {
            CLogger::Log("Removed {} duplicate records", size - cachedPlayer->records.size());
        }
    }
}

float CLagComp::LastValidSimtime() {
    static auto sv_maxunlag = CCVar::Get()->GetCvarByName("sv_maxunlag");
    // more to do
    return CGlobalVars::Get()->currentTime - sv_maxunlag->GetValue<float>();
}

std::tuple<float, float, float> CLagComp::GetOptimalSimtime() {
    CRecord* first = nullptr;
    CRecord* second = nullptr;
    float fraction = 0.f;
    std::tuple<float, float, float> result = {0.f, 0.f, -1.f};

    if (!data.player || !data.pawn || !data.bestRecord || !data.records || data.records->empty()) return result;

    CGameSceneNode* gameSceneNode = data.pawn->m_pGameSceneNode();
    if (!gameSceneNode) return result;

    CSkeletonInstance* skeleton = gameSceneNode->GetSkeleton();
    if (!skeleton) return result;

    const CModelState& modelState = skeleton->m_modelState();
    CModel* model = modelState.m_hModel().Get();
    if (!model) return result;

    uint32_t parent = model->GetBoneParent(data.bestBone);

    size_t nextIndex = std::min(data.bestRecordIndex + 1, data.records->size() - 1);
    size_t prevIndex = data.bestRecordIndex == 0 ? 0 : data.bestRecordIndex - 1;

    CRecord* next = &data.records->at(nextIndex);
    CRecord* prev = &data.records->at(prevIndex);

    const Vector end = CGlobal::Get().eyePos + CGlobal::Get().rcsAngles.ToVector().Normalized() * 4096.f;

    const float bestDist = CMath::Get().DistanceBetweenLines(
        CGlobal::Get().eyePos, end, data.bestRecord->boneMatrix[data.bestBone].position, data.bestRecord->boneMatrix[parent].position);

    if (bestDist > 100.f) return result;

    const float prevDist = CMath::Get().DistanceBetweenLines(CGlobal::Get().eyePos, end, prev->boneMatrix[data.bestBone].position,
                                                             prev->boneMatrix[parent].position);
    const float nextDist = CMath::Get().DistanceBetweenLines(CGlobal::Get().eyePos, end, next->boneMatrix[data.bestBone].position,
                                                             next->boneMatrix[parent].position);
    first = prevDist < nextDist ? prev : data.bestRecord;
    second = prevDist < nextDist ? data.bestRecord : next;
    std::get<0>(result) = first->simulationTime;
    std::get<1>(result) = second->simulationTime;
    std::get<2>(result) = 0.f;

    if (first == second) return result;

    const float bestFullDist = (prevDist < nextDist ? prevDist : nextDist) + bestDist;
    fraction = prevDist < nextDist ? prevDist / bestFullDist : bestDist / bestFullDist;
    std::get<2>(result) = fraction;
    return result;
}
