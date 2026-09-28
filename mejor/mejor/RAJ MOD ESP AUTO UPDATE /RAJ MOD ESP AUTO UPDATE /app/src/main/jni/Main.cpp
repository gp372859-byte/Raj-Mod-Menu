#include "Includes.h"
#include "Includes/Logger.h"
#include "Includes/Color.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "Includes/MonoString.h"
#include "Includes/Strings.h"
#include "KittyMemory/MemoryPatch.h"
#include "Menu/Setup.h"
#include "AutoHook/AutoHook.h"
#include "Includes/Vector2.h"
#include "Includes/Vector3.h"
#include "Includes/Rect.h"
#include "ESP/ESP.h"
#include "ESP/ESPManager.h"
#include "ESP/StructESP.h"

#define LIB OBFUSCATE("libil2cpp.so")

#include "Includes/Macros.h"

// ✅ Missing Notifier
#include "MissingNotifier.h"

// ✅ Money + Speed + Auto Kill
#include "MoneyHack.h"

#include <algorithm>
#include <cmath>
#include <cstring>

ESP espOverlay;

// ✅ IsDead
bool (*get_IsDead)(void *player) = nullptr;

bool SafeIsDead(void* player) {
    if (player == nullptr) return false;
    if (get_IsDead == nullptr) return false;
    return get_IsDead(player);
}

bool(*this_ScreenResolution)(...);
bool SetResolution(int width, int height, bool fullscreen)
{
    return true;
}

enum LineOrigin {
    Top = 0,
    Center = 1,
    Bottom = 2
};

struct variables {
    bool Esp = false;
    bool ESPLine = false;
    bool ESPBox = false;
    bool ESPName = false;
    bool ESPDistance = false;
    bool ESPObject = false;

    float LineThickness = 3.0f;
    float BoxThickness = 3.0f;

    Color ESPLineColor = Color::White();
    Color ESPBoxColor = Color::White();
    int ESPLinePos = LineOrigin::Top;
} var;


// ================================[ ESP CONFIG ]================================ //
void DrawESP(ESP esp, int screenWidth, int screenHeight) {

    // ✅ Missing Notifications
    DrawMissingNotifications(esp, screenWidth, screenHeight);

    // ✅ Money Status
    if (MoneySetupDone) {
        esp.DrawText(Color::Yellow(), MoneyStatus.c_str(), Vector2(20, 100), 20.0f);
    }

    // ✅ ESP Object Count
    if (var.ESPObject) {
        int enemyCount = (int32_t)players.size();
        Vector2 circlePos(screenWidth / 2.0f, screenHeight * 0.05f);
        float circleRadius = 40.0f;
        esp.DrawFilledCircle(Color::Red(), circlePos, circleRadius);
        std::string enemyText = std::to_string(enemyCount);
        Vector2 textPos(circlePos.X, circlePos.Y + 10.0f);
        esp.DrawText(Color::White(), enemyText.c_str(), textPos, 25.0f);
    }

    if (!var.Esp) return;

    for (int i = 0; i < (int)players.size(); ++i) {
        void *Player = players[i];
        if (Player == nullptr || get_camera == nullptr) continue;

        if (SafeIsDead(Player)) {
            players.erase(players.begin() + i);
            --i;
            continue;
        }

        Vector3 position = get_position(getTransform(Player));

        if (isnan(position.x) || isnan(position.y) || isnan(position.z)) {
            players.erase(players.begin() + i);
            --i;
            continue;
        }

        Vector3 PosPlayer    = WorldToScreenPoint(get_camera(), Vector3(position.x, position.y + 1.70f, position.z));
        Vector3 NewPosPlayer = WorldToScreenPoint(get_camera(), Vector3(position.x, position.y - 0.30f, position.z));

        if (NewPosPlayer.z < 1.f) continue;

        float boxHeight = abs(screenHeight - NewPosPlayer.y - (screenHeight - PosPlayer.y));
        float boxWidth  = boxHeight * 0.6f;

        if (boxHeight <= 0 || boxWidth <= 0 || boxHeight > screenHeight * 2) continue;

        Rect rect = Rect(PosPlayer.x - (boxWidth / 2), screenHeight - PosPlayer.y, boxWidth, boxHeight);

        if (var.ESPLine) {
            Vector2 origin;
            switch (var.ESPLinePos) {
                case 0: origin = Vector2(screenWidth / 2.0f, 0); break;
                case 1: origin = Vector2(screenWidth / 2.0f, screenHeight / 2.0f); break;
                case 2:
                default: origin = Vector2(screenWidth / 2.0f, screenHeight); break;
            }
            Vector2 target;
            if (var.ESPLinePos == 0) target = Vector2(PosPlayer.x, screenHeight - PosPlayer.y);
            else target = Vector2(NewPosPlayer.x, screenHeight - NewPosPlayer.y);

            esp.SetLine(origin, target);
            esp.DrawLine(var.ESPLineColor, var.LineThickness);
        }

        if (var.ESPBox) {
            esp.DrawBox(var.ESPBoxColor, var.BoxThickness, rect);
        }

        if (var.ESPDistance) {
            int distance = (int) Vector2::Distance(Vector2(screenWidth, screenHeight), Vector2(NewPosPlayer.x, NewPosPlayer.y));
            std::string distances = float_to_string(distance / 100);
            float centerX = rect.x + (rect.w / 2.0f);
            float textY = rect.y + rect.h + 20.0f;
            esp.DrawText(Color::White(), ("(" + distances + ")").c_str(), Vector2(centerX, textY), 15.0f);
        }

        if (var.ESPName) {
            MonoString* Nick = Getname(Player);
            const char* playerName = (Nick && Nick->toChars()) ? Nick->toChars() : "Unknown";

            float fontSize = 23.0f;
            float paddingX = 90.0f;
            float paddingY = 15.0f;

            float textWidth = strlen(playerName) * (fontSize * 0.6f);
            float textHeight = fontSize;

            float centerX = rect.x + rect.w / 2.0f;
            float nameY = rect.y - textHeight - paddingY;

            Rect background(
                    centerX - textWidth / 2.0f - paddingX / 2.0f,
                    nameY - paddingY / 2.0f,
                    textWidth + paddingX,
                    textHeight + paddingY
            );

            esp.DrawFilledRect(Color(0, 0, 0, 180), background);
            float textX = centerX - textWidth / 2.0f;
            float textY = background.y + paddingY / 2.0f;
            esp.DrawText(Color::Yellow(), playerName, Vector2(textX, textY), fontSize);
        }
    }
}


//======================| Enemy Update |========================== //
void (*old_NpcControlUpdate)(...);
void new_NpcControlUpdate(void* player) {
    if (player != nullptr) {
        if (var.Esp) {
            bool isDead = SafeIsDead(player);
            if (!isDead) {
                if (!playerFind(player)) players.push_back(player);
                if (players.size() > 99) players.clear();
            } else {
                auto it = std::find(players.begin(), players.end(), player);
                if (it != players.end()) players.erase(it);
            }
        }
    }
    if (old_NpcControlUpdate != nullptr) {
        old_NpcControlUpdate(player);
    }
}

// ======================| Enemy OnDestroy |========================== //
void (*old_NpcControlOnDestroy)(...);
void new_NpcControlOnDestroy(void *player) {
    if (player != nullptr) {
        auto it = std::find(players.begin(), players.end(), player);
        if (it != players.end()) players.erase(it);
    }
    if (old_NpcControlOnDestroy != nullptr) {
        old_NpcControlOnDestroy(player);
    }
}

// ================================================ //
void *hack_thread(void *) {

    do {
        sleep(5);
    } while (!isLibraryLoaded(LIB));

    // ✅ Money
    SetupMoneyStatus();

    // ✅ Speed
    SetupSpeedHook();

    // ✅ Auto Kill
    SetupAutoKillHook();

    // ✅ ESP Hooks
    auto PlayerEntity = new LoadClass("", OBFUSCATE("Enemy"));
    if (PlayerEntity == nullptr || PlayerEntity->thisclass == nullptr) {
        NotifyMissing("Enemy");
    } else {
        DWORD UpDate    = PlayerEntity->GetMethodOffsetByName(OBFUSCATE("FixedUpdate"), 0);
        DWORD OnDestroy = PlayerEntity->GetMethodOffsetByName(OBFUSCATE("OnDestroy"), 0);

        if (UpDate != 0) {
            HOOK_AU((void *)UpDate, (void *)new_NpcControlUpdate, old_NpcControlUpdate);
            LOGD("✅ FixedUpdate hook OK");
        } else {
            NotifyMissing("FixedUpdate.Update");
        }

        if (OnDestroy != 0) {
            HOOK_AU((void *)OnDestroy, (void *)new_NpcControlOnDestroy, old_NpcControlOnDestroy);
            LOGD("✅ OnDestroy hook OK");
        } else {
            NotifyMissing("Enemy.OnDestroy");
        }

        DWORD IsDeadOff = PlayerEntity->GetMethodOffsetByName(OBFUSCATE("get_alive"), 0);
        if (IsDeadOff != 0) {
            get_IsDead = (bool (*)(void *))IsDeadOff;
            LOGD("✅ get_IsDead = 0x%X", IsDeadOff);
        } else {
            NotifyMissing("Enemy.get_IsDead");
        }
    }

    // ✅ Unity Classes
    auto Trans = new LoadClass("UnityEngine", OBFUSCATE("Transform"));
    auto Comp  = new LoadClass("UnityEngine", OBFUSCATE("Component"));
    auto Cam   = new LoadClass("UnityEngine", OBFUSCATE("Camera"));

    getAddr::Position     = Trans->GetMethodOffsetByName(OBFUSCATE("get_position_Injected"), 1);
    if (getAddr::Position == 0) NotifyMissing("Transform.get_position_Injected");

    getAddr::SetPosition  = Trans->GetMethodOffsetByName(OBFUSCATE("set_position_Injected"), 1);
    if (getAddr::SetPosition == 0) NotifyMissing("Transform.set_position_Injected");

    getAddr::Transform    = Comp->GetMethodOffsetByName(OBFUSCATE("get_transform"), 0);
    if (getAddr::Transform == 0) NotifyMissing("Component.get_transform");

    getAddr::WorldTScrn   = Cam->GetMethodOffsetByName(OBFUSCATE("WorldToScreenPoint_Injected"), 3);
    if (getAddr::WorldTScrn == 0) NotifyMissing("Camera.WorldToScreenPoint_Injected");

    getAddr::Camera       = Cam->GetMethodOffsetByName(OBFUSCATE("get_main"), 0);
    if (getAddr::Camera == 0) NotifyMissing("Camera.get_main");

    return NULL;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_android_support_Menu_DrawOn(JNIEnv *env, jclass type, jobject espView, jobject canvas) {
    espOverlay = ESP(env, espView, canvas);
    if (espOverlay.isValid()){
        DrawESP(espOverlay, espOverlay.getWidth(), espOverlay.getHeight());
    }
}


jobjectArray GetFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    const char *features[] = {

            OBFUSCATE("Category_ESP"),
            OBFUSCATE("100_Toggle_Enable ESP"),
            OBFUSCATE("101_Toggle_ESP Line"),
            OBFUSCATE("102_Toggle_ESP Box"),
            OBFUSCATE("103_Toggle_ESP Object"),
            OBFUSCATE("104_Toggle_ESP Distance"),
            OBFUSCATE("105_Toggle_ESP Name"),

            OBFUSCATE("Category_ESP CONFIG"),
            OBFUSCATE("200_Spinner_Line Color_Branco,Verde,Azul Ciano,Vermelho,Preto,Amarelo"),
            OBFUSCATE("201_Spinner_Box Color_Branco,Verde,Azul Ciano,Vermelho,Preto,Amarelo"),
            OBFUSCATE("202_Spinner_Line Position_Top,Center,Bottom"),
            OBFUSCATE("203_SeekBar_Line Thickness_1_20"),
            OBFUSCATE("204_SeekBar_Box Thickness_1_20"),

            OBFUSCATE("Category_Speed"),
            OBFUSCATE("500_SeekBar_Speed_1_20"),

            OBFUSCATE("Category_Auto Kill"),
            OBFUSCATE("600_Toggle_Auto Kill"),

            OBFUSCATE("Category_Unlimited Money"),
            OBFUSCATE("300_Toggle_Unlimited Gold"),
            OBFUSCATE("301_Toggle_Unlimited Silver"),
            OBFUSCATE("302_Toggle_Unlimited XP"),
            OBFUSCATE("303_Toggle_Unlimited Karma"),
            OBFUSCATE("304_Toggle_Unlimited Gas"),
    };

    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    return (ret);
}

void Changes(JNIEnv *env, jclass clazz, jobject obj,
                                        jint featNum, jstring featName, jint value,
                                        jboolean boolean, jstring str) {

    switch (featNum) {
        case 100: var.Esp = boolean; break;
        case 101: var.ESPLine = boolean; break;
        case 102: var.ESPBox = boolean; break;
        case 103: var.ESPObject = boolean; break;
        case 104: var.ESPDistance = boolean; break;
        case 105: var.ESPName = boolean; break;

        case 200:
            if(value == 0)      var.ESPLineColor = Color::White();
            else if(value == 1) var.ESPLineColor = Color::Green();
            else if(value == 2) var.ESPLineColor = Color::Blue();
            else if(value == 3) var.ESPLineColor = Color::Red();
            else if(value == 4) var.ESPLineColor = Color::Black();
            else if(value == 5) var.ESPLineColor = Color::Yellow();
            break;

        case 201:
            if(value == 0)      var.ESPBoxColor = Color::White();
            else if(value == 1) var.ESPBoxColor = Color::Green();
            else if(value == 2) var.ESPBoxColor = Color::Blue();
            else if(value == 3) var.ESPBoxColor = Color::Red();
            else if(value == 4) var.ESPBoxColor = Color::Black();
            else if(value == 5) var.ESPBoxColor = Color::Yellow();
            break;

        case 202:
            if (value == 0) var.ESPLinePos = Top;
            else if (value == 1) var.ESPLinePos = Center;
            else if (value == 2) var.ESPLinePos = Bottom;
            break;

        case 203: if (value >= 1) var.LineThickness = value; break;
        case 204: if (value >= 1) var.BoxThickness = value; break;

        // ✅ Speed
        case 500: Player_Speed = (float)value; break;

        // ✅ Auto Kill
        case 600: Auto_Kill = boolean; break;

        // ✅ Money
        case 300: SetGold(boolean); break;
        case 301: SetSilver(boolean); break;
        case 302: SetXp(boolean); break;
        case 303: SetKarma(boolean); break;
        case 304: SetGas(boolean); break;
    }
}

__attribute__((constructor))
void lib_main() {
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}


int RegisterMenu(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Icon"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(Icon)},
            {OBFUSCATE("IconWebViewData"),  OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(IconWebViewData)},
            {OBFUSCATE("IsGameLibLoaded"),  OBFUSCATE("()Z"), reinterpret_cast<void *>(isGameLibLoaded)},
            {OBFUSCATE("Init"),  OBFUSCATE("(Landroid/content/Context;Landroid/widget/TextView;Landroid/widget/TextView;)V"), reinterpret_cast<void *>(Init)},
            {OBFUSCATE("SettingsList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(SettingsList)},
            {OBFUSCATE("GetFeatureList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(GetFeatureList)},
    };

    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Menu"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterPreferences(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Changes"), OBFUSCATE("(Landroid/content/Context;ILjava/lang/String;IZLjava/lang/String;)V"), reinterpret_cast<void *>(Changes)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Preferences"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterMain(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("CheckOverlayPermission"), OBFUSCATE("(Landroid/content/Context;)V"), reinterpret_cast<void *>(CheckOverlayPermission)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Main"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;

    return JNI_OK;
}

extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (RegisterMenu(env) != 0)
        return JNI_ERR;
    if (RegisterPreferences(env) != 0)
        return JNI_ERR;
    if (RegisterMain(env) != 0)
        return JNI_ERR;
    return JNI_VERSION_1_6;
}