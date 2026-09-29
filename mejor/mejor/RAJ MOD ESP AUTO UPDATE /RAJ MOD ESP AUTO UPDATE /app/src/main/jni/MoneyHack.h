#pragma once
#include "Includes.h"
#include "Includes/Logger.h"
#include "Includes/Macros.h"
#include "KittyMemory/MemoryPatch.h"
#include "MissingNotifier.h"
#include <string>
#include <cstring>
#include <dlfcn.h>

// ============================================================
// ⚠️ IMPORTANT
// 32-bit aur 64-bit offsets ALAG hote hain!
// 32-bit offsets: armeabi-v7a/libil2cpp.so se (Il2CppDumper)
// 64-bit offsets: arm64-v8a/libil2cpp.so se (Il2CppDumper)
// Same offset dono arch mein kaam NAHI karega!
// Agar kisi arch ka offset nahi hai toh us #define ko comment kar do
// → us arch pe wo feature skip ho jayega (crash nahi)
// ============================================================

// ============================================================
// ✅ ARCHITECTURE DETECTION
// ============================================================
inline bool Is64BitBinary(const char* libName) {
    void* handle = dlopen(libName, RTLD_NOW);
    if (!handle) return false;

    void* sym = dlsym(handle, "il2cpp_init");
    if (!sym) sym = dlsym(handle, "il2cpp_domain_get");
    dlclose(handle);

    if (!sym) return false;

    uintptr_t addr = (uintptr_t)sym;
    return (addr > 0xFFFFFFFF);
}

// ============================================================
// ✅ DUAL ARCH PATCH STRUCT
// ============================================================
struct DualPatch {
    uintptr_t offset32;   // 32-bit offset (0x0 = skip)
    uintptr_t offset64;   // 64-bit offset (0x0 = skip)
    const char* hex32;    // ARM32 hex
    const char* hex64;    // ARM64 hex
};

// ============================================================
// ✅ SAFE PATCH (Dual Arch) — crash-safe
// ============================================================
inline bool SafePatchDual(const char* libName, const DualPatch& dp, bool enable)
{
    bool is64 = Is64BitBinary(libName);

    uintptr_t offset = is64 ? dp.offset64 : dp.offset32;
    const char* hexBytes = is64 ? dp.hex64 : dp.hex32;

    if (offset == 0) {
        LOGD("⚠️ %s offset not provided, skipping", is64 ? "64-bit" : "32-bit");
        return false;
    }
    if (hexBytes == nullptr) {
        LOGD("⚠️ %s hex not provided, skipping", is64 ? "64-bit" : "32-bit");
        return false;
    }

    void* addr = (void*)getAbsoluteAddress(libName, offset);
    if (addr == nullptr) {
        LOGD("❌ Address null at 0x%lX", (unsigned long)offset);
        return false;
    }

    uintptr_t addrVal = (uintptr_t)addr;
    if (is64) {
        if (addrVal < 0x1000) {
            LOGD("⚠️ Invalid 64-bit address: 0x%lX", (unsigned long)addrVal);
            return false;
        }
    } else {
        if (addrVal < 0x1000 || addrVal > 0xFFFFFFFF) {
            LOGD("⚠️ Invalid 32-bit address: 0x%lX", (unsigned long)addrVal);
            return false;
        }
    }

    MemoryPatch patch = MemoryPatch::createWithHex(libName, offset, hexBytes);
    if (!patch.isValid()) {
        LOGD("❌ Invalid patch at 0x%lX", (unsigned long)offset);
        return false;
    }

    if (enable) return patch.Modify();
    else        return patch.Restore();
}

// ============================================================
// ✅ UNLIMITED MONEY (Dual Arch)
// ============================================================
inline std::string MoneyStatus = "Money Hack:\n";
inline bool MoneySetupDone = false;

// ✅ 32-bit ARM32 hex (ARM mode return trick)
static const char* HEX_ARM32 = "FF 09 0C E3 9A 0B 43 E3 1E FF 2F E1";

// ✅ 64-bit ARM64 hex (return 0: MOV W0, #0 ; RET)
static const char* HEX_ARM64 = "00 00 80 52 C0 03 5F D6";

// ✅ Dual patches — 32-bit aur 64-bit offsets ALAG daalo
// Agar 64-bit offset nahi mila toh 0x0 rakho (skip ho jayega)
inline DualPatch DP_Gold   = { 0x3347B98, 0x0, HEX_ARM32, HEX_ARM64 };
inline DualPatch DP_Silver = { 0x3347A1C, 0x0, HEX_ARM32, HEX_ARM64 };
inline DualPatch DP_Xp     = { 0x334661C, 0x0, HEX_ARM32, HEX_ARM64 };
inline DualPatch DP_Karma  = { 0x3348A40, 0x0, HEX_ARM32, HEX_ARM64 };
inline DualPatch DP_Gas    = { 0x3346958, 0x0, HEX_ARM32, HEX_ARM64 };

void SetGold(bool enable) {
    if (!SafePatchDual("libil2cpp.so", DP_Gold, enable))
        LOGD("⚠️ Gold patch skipped/failed");
}
void SetSilver(bool enable) {
    if (!SafePatchDual("libil2cpp.so", DP_Silver, enable))
        LOGD("⚠️ Silver patch skipped/failed");
}
void SetXp(bool enable) {
    if (!SafePatchDual("libil2cpp.so", DP_Xp, enable))
        LOGD("⚠️ XP patch skipped/failed");
}
void SetKarma(bool enable) {
    if (!SafePatchDual("libil2cpp.so", DP_Karma, enable))
        LOGD("⚠️ Karma patch skipped/failed");
}
void SetGas(bool enable) {
    if (!SafePatchDual("libil2cpp.so", DP_Gas, enable))
        LOGD("⚠️ Gas patch skipped/failed");
}

// ============================================================
// 🏃 SPEED HACK (Dual Arch) — FIXED with string literals
// ============================================================
inline float Player_Speed = 0;
inline float (*old_playerspeed32)(void *instance) = nullptr;
inline float (*old_playerspeed64)(void *instance) = nullptr;

inline float playerspeed(void *instance) {
    if (instance != NULL && Player_Speed > 1.0f) return Player_Speed;
    bool is64 = Is64BitBinary("libil2cpp.so");
    if (is64 && old_playerspeed64 != nullptr) return old_playerspeed64(instance);
    if (!is64 && old_playerspeed32 != nullptr) return old_playerspeed32(instance);
    return 1.0f;
}

// ✅ String literals (OBFUSCATE inhe support karta hai)
// Nahi chahiye toh comment kar do
#define SPEED_OFF_32_LIT "0x1D79748"     // 32-bit
// #define SPEED_OFF_64_LIT "0x???????"  // 64-bit (agar hai toh uncomment karo)

inline void SetupSpeedHook() {
    bool is64 = Is64BitBinary("libil2cpp.so");

    if (is64) {
#ifdef SPEED_OFF_64_LIT
        void* addr = (void*)getAbsoluteAddress("libil2cpp.so", string2Offset(SPEED_OFF_64_LIT));
        if (addr == nullptr) { LOGD("⚠️ Speed 64-bit addr null, skip"); return; }
        HOOK_LIB("libil2cpp.so", SPEED_OFF_64_LIT, playerspeed, old_playerspeed64);
        if (old_playerspeed64 != nullptr) LOGD("✅ Speed hook OK (64-bit)");
        else LOGD("⚠️ Speed hook failed (64-bit)");
#else
        LOGD("⚠️ Speed 64-bit offset not provided, skip");
#endif
    } else {
#ifdef SPEED_OFF_32_LIT
        void* addr = (void*)getAbsoluteAddress("libil2cpp.so", string2Offset(SPEED_OFF_32_LIT));
        if (addr == nullptr) { LOGD("⚠️ Speed 32-bit addr null, skip"); return; }
        HOOK_LIB("libil2cpp.so", SPEED_OFF_32_LIT, playerspeed, old_playerspeed32);
        if (old_playerspeed32 != nullptr) LOGD("✅ Speed hook OK (32-bit)");
        else LOGD("⚠️ Speed hook failed (32-bit)");
#else
        LOGD("⚠️ Speed 32-bit offset not provided, skip");
#endif
    }
}

// ============================================================
// 💀 AUTO KILL (Dual Arch) — FIXED with string literals
// ============================================================
inline bool Auto_Kill = false;
inline void (*_instakill32)(void *enemy) = nullptr;
inline void (*_instakill64)(void *enemy) = nullptr;
inline void (*old_enemy_update32)(void *enemy) = nullptr;
inline void (*old_enemy_update64)(void *enemy) = nullptr;

inline void enemy_update_hook(void *enemy) {
    bool is64 = Is64BitBinary("libil2cpp.so");
    if (enemy != NULL && Auto_Kill) {
        if (is64 && _instakill64 != nullptr) _instakill64(enemy);
        else if (!is64 && _instakill32 != nullptr) _instakill32(enemy);
    }
    if (is64 && old_enemy_update64 != nullptr) old_enemy_update64(enemy);
    else if (!is64 && old_enemy_update32 != nullptr) old_enemy_update32(enemy);
}

// ✅ String literals — nahi chahiye toh comment kar do
#define AK_KILL_32_LIT   "0x37C4044"      // 32-bit
// #define AK_KILL_64_LIT   "0x???????"   // 64-bit (agar hai toh uncomment karo)
#define AK_UPDATE_32_LIT "0x37C2ACC"      // 32-bit
// #define AK_UPDATE_64_LIT "0x???????"   // 64-bit (agar hai toh uncomment karo)

inline void SetupAutoKillHook() {
    bool is64 = Is64BitBinary("libil2cpp.so");

    if (is64) {
#ifdef AK_KILL_64_LIT
        void* killAddr = (void*)getAbsoluteAddress("libil2cpp.so", string2Offset(AK_KILL_64_LIT));
        if (killAddr == nullptr) { LOGD("⚠️ AutoKill kill addr null (64-bit), skip"); return; }
        _instakill64 = (void (*)(void *))killAddr;

#ifdef AK_UPDATE_64_LIT
        void* updAddr = (void*)getAbsoluteAddress("libil2cpp.so", string2Offset(AK_UPDATE_64_LIT));
        if (updAddr == nullptr) { LOGD("⚠️ AutoKill update addr null (64-bit), skip"); return; }
        HOOK_LIB("libil2cpp.so", AK_UPDATE_64_LIT, enemy_update_hook, old_enemy_update64);
        if (old_enemy_update64 != nullptr) LOGD("✅ AutoKill hook OK (64-bit)");
        else LOGD("⚠️ AutoKill hook failed (64-bit)");
#else
        LOGD("⚠️ AutoKill update offset not provided (64-bit)");
#endif
#else
        LOGD("⚠️ AutoKill offset not provided (64-bit), skip");
#endif
    } else {
#ifdef AK_KILL_32_LIT
        void* killAddr = (void*)getAbsoluteAddress("libil2cpp.so", string2Offset(AK_KILL_32_LIT));
        if (killAddr == nullptr) { LOGD("⚠️ AutoKill kill addr null (32-bit), skip"); return; }
        _instakill32 = (void (*)(void *))killAddr;

#ifdef AK_UPDATE_32_LIT
        void* updAddr = (void*)getAbsoluteAddress("libil2cpp.so", string2Offset(AK_UPDATE_32_LIT));
        if (updAddr == nullptr) { LOGD("⚠️ AutoKill update addr null (32-bit), skip"); return; }
        HOOK_LIB("libil2cpp.so", AK_UPDATE_32_LIT, enemy_update_hook, old_enemy_update32);
        if (old_enemy_update32 != nullptr) LOGD("✅ AutoKill hook OK (32-bit)");
        else LOGD("⚠️ AutoKill hook failed (32-bit)");
#else
        LOGD("⚠️ AutoKill update offset not provided (32-bit)");
#endif
#else
        LOGD("⚠️ AutoKill offset not provided (32-bit), skip");
#endif
    }
}

// ============================================================
// ✅ MONEY STATUS SETUP (Dual Arch)
// ============================================================
void SetupMoneyStatus() {
    std::string status = "Money Hack:\n";

    auto PlayerInfo = new LoadClass(OBFUSCATE("WalkingZombie"), OBFUSCATE("CPlayerInfo"));
    if (PlayerInfo == nullptr || PlayerInfo->thisclass == nullptr) {
        MoneyStatus = "❌ CPlayerInfo NOT FOUND";
        MoneySetupDone = true;
        LOGD("❌ CPlayerInfo not found");
        return;
    }

    bool is64 = Is64BitBinary("libil2cpp.so");
    status += "✅ Class OK\n";
    status += is64 ? "Mode: 64-bit\n" : "Mode: 32-bit\n";

    DWORD xpOff = PlayerInfo->GetMethodOffsetByName(OBFUSCATE("get_Xp"), 0);
    if (xpOff != 0) {
        if (is64) DP_Xp.offset64 = xpOff; else DP_Xp.offset32 = xpOff;
        status += "✅ get_Xp\n";
    } else status += "❌ get_Xp\n";

    DWORD gasOff = PlayerInfo->GetMethodOffsetByName(OBFUSCATE("get_Gas"), 0);
    if (gasOff != 0) {
        if (is64) DP_Gas.offset64 = gasOff; else DP_Gas.offset32 = gasOff;
        status += "✅ get_Gas\n";
    } else status += "❌ get_Gas\n";

    DWORD goldOff = PlayerInfo->GetMethodOffsetByName(OBFUSCATE("get_CoinsGold"), 0);
    if (goldOff != 0) {
        if (is64) DP_Gold.offset64 = goldOff; else DP_Gold.offset32 = goldOff;
        status += "✅ get_CoinsGold\n";
    } else status += "❌ get_CoinsGold\n";

    DWORD silverOff = PlayerInfo->GetMethodOffsetByName(OBFUSCATE("get_CoinsSilver"), 0);
    if (silverOff != 0) {
        if (is64) DP_Silver.offset64 = silverOff; else DP_Silver.offset32 = silverOff;
        status += "✅ get_CoinsSilver\n";
    } else status += "❌ get_CoinsSilver\n";

    DWORD karmaOff = PlayerInfo->GetMethodOffsetByName(OBFUSCATE("get_Karma"), 0);
    if (karmaOff != 0) {
        if (is64) DP_Karma.offset64 = karmaOff; else DP_Karma.offset32 = karmaOff;
        status += "✅ get_Karma\n";
    } else status += "❌ get_Karma\n";

    MoneyStatus = status;
    MoneySetupDone = true;
}
