#include <pch.hpp>

#include <hacks/aimbot/lagcomp.hpp>
#include <hacks/global/global.hpp>

#include <interfaces/enginetrace.hpp>
#include <interfaces/ccsgoinput.hpp>
#include <interfaces/globalvars.hpp>
#include <interfaces/cvar.hpp>
#include <interfaces/engineclient.hpp>

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
        if (!cachedPlayer || !cachedPlayer->IsEnemyWithTeam(CGlobal::Get().player->GetTeam()) || cachedPlayer == CGlobal::Get().player)
            continue;

        CCSPlayerController* controller = cachedPlayer->Get();
        if (!controller->m_bPawnIsAlive()) continue;

        C_CSPlayerPawn* pawn = controller->m_hPawn().Get();
        if (!pawn || pawn->m_bGunGameImmunity()) continue;

        Vector eyePos;
        pawn->GetEyePos(&eyePos);
        const float distance = eyePos.DistTo(CGlobal::Get().eyePos);
        if (CGlobal::Get().vdata && distance > CGlobal::Get().vdata->m_flRange()) continue;

        GameTrace_t trace = GameTrace_t();
        TraceFilter_t filter = TraceFilter_t(0x1C3003, CGlobal::Get().pawn, nullptr, 4);

        CEngineTrace::Get()->TraceShape(CGlobal::Get().eyePos, eyePos,
                                        CGlobal::Get().pawn,
                                        0x1C3003, 4,
                                        &trace);
        if (trace.hitEntity != pawn /*&& trace.fraction < 0.9f*/) {
            cachedPlayer->Reset();
            continue;
        }

        cachedPlayer->visibleSince += CGlobalVars::Get()->intervalPerSubtick;
        if (cachedPlayer->visibleSince < 0.15f) continue;

        const Vector eyeDir = pawn->m_angEyeAngles().ToVector().Normalized();

        size_t i = 0;
        if ((!g_Vars.m_Backtrack || !g_Vars.m_AimAtRecords) && !cachedPlayer->records.empty()) {
            CRecord& record = cachedPlayer->records.back();
            if (!record.IsValid()) continue;

            const Vector& bonePos = record.boneMatrix[BONE_HEAD_0].position;
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
        } else {
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
    static auto sv_maxunlag = CCVar::Get()->GetCvarByName("sv_maxunlag");
    const int maxTicks = TIME_TO_TICKS(sv_maxunlag->GetValue<float>());
    const bool alive = CGlobal::Get().controller->m_bPawnIsAlive();

    const std::lock_guard<std::mutex> lock(CMatchCache::GetLock());
    for (const auto& it : CMatchCache::GetCachedEntities()) {
        const auto& cachedEntity = it.second;
        if (!cachedEntity->IsValid() || cachedEntity->GetType() != CCachedBaseEntity::Type::PLAYER) continue;

        CCachedPlayer* cachedPlayer = dynamic_cast<CCachedPlayer*>(cachedEntity.get());
        if (!cachedPlayer || !cachedPlayer->IsEnemyWithTeam(CGlobal::Get().player->GetTeam()) || cachedPlayer == CGlobal::Get().player) {
            continue;
        }

        CCSPlayerController* controller = cachedPlayer->Get();
        if (!alive || !controller->m_bPawnIsAlive()) {
            cachedPlayer->records.clear();
            continue;
        }

        C_CSPlayerPawn* pawn = controller->m_hPawn().Get();
        if (!pawn || pawn->m_bGunGameImmunity()) {
            cachedPlayer->records.clear();
            continue;
        }

        CRecord record(pawn);
        if (!record.valid || !cachedPlayer->records.empty() && cachedPlayer->records.back().simulationTime == record.simulationTime)
            continue;

        cachedPlayer->records.push_back(std::move(record));

        if (cachedPlayer->records.size() > maxTicks - 1)
            cachedPlayer->records.erase(cachedPlayer->records.begin(), cachedPlayer->records.end() - (maxTicks - 1));
    }
}

void CLagComp::PreModifyInput(CInputFrame* msg, CCSGOInputHistoryEntryPB* historyEntry) {
    calculatedInterpolations = false;
    if (!CEngineClient::Get()->IsInGame()) return;
    if (!historyEntry) return;
    if (!g_Vars.m_Backtrack) return;
    if (!data.player || data.player->records.empty() || !data.bestRecord) return;
    if (!CGlobal::Get().pawn) return;
    CGameSceneNode* node = CGlobal::Get().pawn->m_pGameSceneNode();
    if (!node) return;

    // const float cl_interp = CLagComp::Get().GetClientInterp();
    Tickfrac_t tf;
    tf.tick = TIME_TO_TICKS(CLagComp::Get().data.bestRecord->simulationTime);
    tf.fraction = 0.f;

    backup = *msg;
    restore = true;

    msg->player = tf;
    msg->render = tf;
}

void CLagComp::PostModifyInput(CInputFrame* msg, CCSGOInputHistoryEntryPB* historyEntry) {
    if (!CEngineClient::Get()->IsInGame()) return;
    if (!historyEntry) return;
    if (!g_Vars.m_Backtrack) return;
    if (!data.player || data.player->records.empty() || !data.bestRecord) return;
    if (!CGlobal::Get().pawn) return;
    CGameSceneNode* node = CGlobal::Get().pawn->m_pGameSceneNode();
    if (!node) return;
    if (!historyEntry->cl_interp || !historyEntry->sv_interp0 || !historyEntry->sv_interp1) return;

    if (restore) {
        *msg = backup;
        historyEntry->nRenderTickCount = backup.render.tick;
        historyEntry->flRenderTickFraction = backup.render.fraction;
        historyEntry->nPlayerTickCount = backup.player.tick;
        historyEntry->flPlayerTickFraction = backup.player.fraction;
        restore = false;
    }
}

void CLagComp::Test() {
    if (!CEngineClient::Get()->IsInGame()) return;
    if (!g_Vars.m_Backtrack) return;
    if (!data.player || data.player->records.empty() || !data.bestRecord) return;
    if (!CGlobal::Get().pawn) return;
    if (CGlobal::Get().cmd->csgoUserCmd.attack1HistoryIndex == -1) return;
    CGameSceneNode* node = CGlobal::Get().pawn->m_pGameSceneNode();
    if (!node) return;
    // if (!historyEntry->cl_interp || !historyEntry->sv_interp0 || !historyEntry->sv_interp1) return;
    const auto& frameHistory = CCSGOInput::Get()->frameHistory;
    if (frameHistory.m_Size <= 0) return;

    
    CInputFrame* frameInput = frameHistory.AtPtr(frameHistory.m_Size - 1);
    const Tickfrac_t playerBackup = frameInput->player;

    frameInput->player.tick = TIME_TO_TICKS(CLagComp::Get().data.bestRecord->simulationTime);
    frameInput->player.fraction = 0.f;

    InterpInfo_t cl, sv0, sv1;
    if (!node->CalculateInterpInfos(&cl, &sv0, &sv1, &frameInput->player)) {
        CLogger::Log("Failed to calculate interpolation infos");
        //return;
    }

    frameInput->player = playerBackup;

    CCSGOInputHistoryEntryPB* entry = CGlobal::Get().cmd->GetInputHistoryEntry(CGlobal::Get().cmd->csgoUserCmd.attack1HistoryIndex);
    if (!entry) {
        CLogger::Log("Failed to get input history entry");
        return;
    }

    // fix visual bugs and bad aim punch cache
    entry->flRenderTickFraction = frameInput->render.fraction;
    entry->nRenderTickCount = frameInput->render.tick;

    // if (entry->pTargetViewCmd) entry->pTargetViewCmd->viewAngles;
    //  if (entry->pViewCmd)

    if (entry->cl_interp) {
        entry->cl_interp->srcTick = cl.srcTick;
        entry->cl_interp->fraction = cl.fraction;
    }

    if (entry->sv_interp0) {
        entry->sv_interp0->srcTick = sv0.srcTick;
        entry->sv_interp0->fraction = sv0.fraction;
    }

    if (entry->sv_interp1) {
        entry->sv_interp1->srcTick = sv1.srcTick;
        entry->sv_interp1->fraction = sv1.fraction;
    }
}

std::tuple<float, float, float> CLagComp::GetSubtickSimtime() {
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
    CRecord* first = prevDist < nextDist ? prev : data.bestRecord;
    CRecord* second = prevDist < nextDist ? data.bestRecord : next;
    std::get<0>(result) = first->simulationTime;
    std::get<1>(result) = second->simulationTime;
    std::get<2>(result) = 0.f;

    if (first == second) return result;

    const float bestFullDist = (prevDist < nextDist ? prevDist : nextDist) + bestDist;
    const float fraction = prevDist < nextDist ? prevDist / bestFullDist : bestDist / bestFullDist;
    std::get<2>(result) = fraction;
    return result;
}

float CLagComp::GetClientInterp() {
    const float cl_interp = signatures::GetClientInterp.GetPtrAs<float (*)()>()();
    return cl_interp * TICK_INTERVAL;
}
