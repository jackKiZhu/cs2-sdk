#include <pch.hpp>

#include <bindings/weapon.hpp>

#include <interfaces/globalvars.hpp>

bool C_CSWeaponBaseGun::CanPrimaryAttack(const int weaponType, const float serverTime) { 
	if (weaponType == WEAPONTYPE_KNIFE) return true;
	if (m_bBurstMode() && m_iBurstShotsRemaining() > 0) return true;
    if (m_iClip1() <= 0) return false;
    // cocking

    // 40 53 48 83 EC ? 48 8B 41 ? 48 8B D9 48 8D 4C 24 ? 8B 50 ? E8 ? ? ? ? F3 0F 10 05
    float attackTickRatio = m_flNextPrimaryAttackTickRatio();
    int attackTick = m_nNextPrimaryAttackTick();
    if (attackTickRatio >= (1.f - RATIO_THRESHOLD) || attackTickRatio < RATIO_THRESHOLD) {
        if (attackTickRatio >= (1.f - RATIO_THRESHOLD)) 
            attackTick++;
        attackTickRatio = 0.f;
    }

    const auto& [tick, ratio] = CGlobalVars::Get()->GetCurrentTickRounded();
    if (tick > attackTick) return true;
    if (tick == attackTick) return attackTickRatio <= ratio;
	return false; 
}

bool C_CSWeaponBaseGun::CanSecondaryAttack(const int weaponType, const float serverTime) { 
	if (weaponType == WEAPONTYPE_KNIFE) return true;
    if (m_bBurstMode() && m_iBurstShotsRemaining() > 0) return true;
    if (m_iClip1() <= 0) return false;
    // cocking

    // 40 53 48 83 EC ? 48 8B 41 ? 48 8B D9 48 8D 4C 24 ? 8B 50 ? E8 ? ? ? ? F3 0F 10 05
    float attackTickRatio = m_flNextSecondaryAttackTickRatio();
    int attackTick = m_nNextSecondaryAttackTick();
    if (attackTickRatio >= (1.f - RATIO_THRESHOLD) || attackTickRatio < RATIO_THRESHOLD) {
        if (attackTickRatio >= (1.f - RATIO_THRESHOLD)) attackTick++;
        attackTickRatio = 0.f;
    }

    const auto& [tick, ratio] = CGlobalVars::Get()->GetCurrentTickRounded();
    if (tick > attackTick) return true;
    if (tick == attackTick) return attackTickRatio <= ratio;
    return false;
}
