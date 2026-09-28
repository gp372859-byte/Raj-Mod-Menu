#pragma once
#include "Includes.h"
#include "Includes/Logger.h"
#include "Includes/Macros.h"
#include "KittyMemory/MemoryPatch.h"
#include "MissingNotifier.h"
#include <string>
#include <cstring>

// ==========================================
// ✅ SAFE PATCH HELPER
// ==========================================
inline bool SafePatch(const char* libName, uintptr_t offset,
                      const char* hexBytes, bool enable)
{
    if (offset == 0) return false;

    void* addr = (void*)getAbsoluteAddress(libName, offset);
    if (addr == nullptr) return false;

    MemoryPatch patch = MemoryPatch::createWithHex(libName, offset, hexBytes);
    if (!patch.isValid()) {
        LOGD("❌ SafePatch: invalid patch at 0x%lX", (unsigned long)offset);
        return false;
    }

    if (enable) return patch.Modify();
    else        return patch.Restore();
}

// ==========================================
// ✅ UNLIMITED MONEY
// ==========================================
inline std::string MoneyStatus = "Money Hack:\n";
inline bool MoneySetupDone = false;

static const char* PATCH_BYTES = "FF 09 0C E3 9A 0B 43 E3 1E FF 2F E1";

inline uintptr_t g_OffGold   = 0x3347B98;
inline uintptr_t g_OffSilver = 0x3347A1C;
inline uintptr_t g_OffXp     = 0x334661C;
inline uintptr_t g_OffKarma  = 0x3348A40;
inline uintptr_t g_OffGas    = 0x3346958;

void SetGold(bool enable) {
    if (!SafePatch("libil2cpp.so", g_OffGold, PATCH_BYTES, enable))
        NotifyMissing("Gold offset 0x3347B98");
}
void SetSilver(bool enable) {
    if (!SafePatch("libil2cpp.so", g_OffSilver, PATCH_BYTES, enable))
        NotifyMissing("Silver offset 0x3347A1C");
}
void SetXp(bool enable) {
    if (!SafePatch("libil2cpp.so", g_OffXp, PATCH_BYTES, enable))
        NotifyMissing("XP offset 0x334661C");
}
void SetKarma(bool enable) {
    if (!SafePatch("libil2cpp.so", g_OffKarma, PATCH_BYTES, enable))
        NotifyMissing("Karma offset 0x3348A40");
}
void SetGas(bool enable) {
    if (!SafePatch("libil2cpp.so", g_OffGas, PATCH_BYTES, enable))
        NotifyMissing("Gas offset 0x3346958");
}

// ==========================================
// 🏃 SPEED HACK
// ==========================================
inline float Player_Speed = 0;
inline float (*old_playerspeed)(void *instance) = nullptr;

inline float playerspeed(void *instance) {
    if (instance != NULL && Player_Speed > 1.0f) return Player_Speed;
    if (old_playerspeed != nullptr) return old_playerspeed(instance);
    return 1.0f;
}

inline void SetupSpeedHook() {
    void* addr = (void*)getAbsoluteAddress("libil2cpp.so", string2Offset("0x1D79748"));
    if (addr == nullptr) {
        NotifyMissing("Speed offset 0x1D79748");
        return;
    }
    HOOK_LIB("libil2cpp.so", "0x1D79748", playerspeed, old_playerspeed);
    if (old_playerspeed != nullptr) LOGD("✅ Speed hook OK");
    else NotifyMissing("Speed hook failed");
}

// ==========================================
// 💀 AUTO KILL
// ==========================================
inline bool Auto_Kill = false;
inline void (*_instakill)(void *enemy) = nullptr;
inline void (*old_enemy_update)(void *enemy) = nullptr;

inline void enemy_update_hook(void *enemy) {
    if (enemy != NULL && Auto_Kill && _instakill != nullptr) {
        _instakill(enemy);
    }
    if (old_enemy_update != nullptr) {
        old_enemy_update(enemy);
    }
}

inline void SetupAutoKillHook() {
    void* killAddr = (void*)getAbsoluteAddress("libil2cpp.so", string2Offset("0x37C4044"));
    if (killAddr == nullptr) {
        NotifyMissing("AutoKill instakill 0x37C4044");
        return;
    }
    _instakill = (void (*)(void *))killAddr;

    void* updAddr = (void*)getAbsoluteAddress("libil2cpp.so", string2Offset("0x37C2ACC"));
    if (updAddr == nullptr) {
        NotifyMissing("AutoKill update 0x37C2ACC");
        return;
    }
    HOOK_LIB("libil2cpp.so", "0x37C2ACC", enemy_update_hook, old_enemy_update);
    if (old_enemy_update != nullptr) LOGD("✅ Auto Kill hook OK");
    else NotifyMissing("AutoKill hook failed");
}

// ==========================================
// ✅ Setup — Money Status
// ==========================================
void SetupMoneyStatus() {
    std::string status = "Money Hack:\n";

    auto PlayerInfo = new LoadClass(OBFUSCATE("WalkingZombie"), OBFUSCATE("CPlayerInfo"));
    if (PlayerInfo == nullptr || PlayerInfo->thisclass == nullptr) {
        MoneyStatus = "❌ CPlayerInfo NOT FOUND";
        MoneySetupDone = true;
        NotifyMissing("WalkingZombie.CPlayerInfo");
        return;
    }

    status += "✅ Class OK\n";

    DWORD xpOff = PlayerInfo->GetMethodOffsetByName(OBFUSCATE("get_Xp"), 0);
    if (xpOff != 0) { status += "✅ get_Xp\n"; g_OffXp = xpOff; }
    else { status += "❌ get_Xp\n"; NotifyMissing("CPlayerInfo.get_Xp"); }

    DWORD gasOff = PlayerInfo->GetMethodOffsetByName(OBFUSCATE("get_Gas"), 0);
    if (gasOff != 0) { status += "✅ get_Gas\n"; g_OffGas = gasOff; }
    else { status += "❌ get_Gas\n"; NotifyMissing("CPlayerInfo.get_Gas"); }

    DWORD goldOff = PlayerInfo->GetMethodOffsetByName(OBFUSCATE("get_CoinsGold"), 0);
    if (goldOff != 0) { status += "✅ get_CoinsGold\n"; g_OffGold = goldOff; }
    else { status += "❌ get_CoinsGold\n"; NotifyMissing("CPlayerInfo.get_CoinsGold"); }

    DWORD silverOff = PlayerInfo->GetMethodOffsetByName(OBFUSCATE("get_CoinsSilver"), 0);
    if (silverOff != 0) { status += "✅ get_CoinsSilver\n"; g_OffSilver = silverOff; }
    else { status += "❌ get_CoinsSilver\n"; NotifyMissing("CPlayerInfo.get_CoinsSilver"); }

    DWORD karmaOff = PlayerInfo->GetMethodOffsetByName(OBFUSCATE("get_Karma"), 0);
    if (karmaOff != 0) { status += "✅ get_Karma\n"; g_OffKarma = karmaOff; }
    else { status += "❌ get_Karma\n"; NotifyMissing("CPlayerInfo.get_Karma"); }

    MoneyStatus = status;
    MoneySetupDone = true;
}
