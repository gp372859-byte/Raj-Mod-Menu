#pragma once
#include "Includes/Logger.h"
#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include "Includes/Color.h"
#include "Includes/Vector2.h"
#include "ESP/ESP.h"

// ==================== NOTIFICATION STRUCT ====================
struct MissingNotification {
    std::string text;
    std::chrono::steady_clock::time_point createdTime;
    float duration;
    Color color;
};

// ==================== GLOBAL LIST ====================
static std::vector<MissingNotification> g_MissingList;
static std::mutex g_MissingMutex;

// ==================== ADD NOTIFICATION ====================
inline void NotifyMissing(const std::string& name, float duration = 2.0f) {
    std::lock_guard<std::mutex> lock(g_MissingMutex);
    MissingNotification n;
    n.text = "NOT FOUND: " + name;
    n.createdTime = std::chrono::steady_clock::now();
    n.duration = duration;
    n.color = Color::Red();
    g_MissingList.push_back(n);
    LOGD("NOT FOUND: %s", name.c_str());
}

// ==================== DRAW NOTIFICATIONS ====================
inline void DrawMissingNotifications(ESP& esp, int screenWidth, int screenHeight) {
    std::lock_guard<std::mutex> lock(g_MissingMutex);
    
    auto now = std::chrono::steady_clock::now();
    float startY = 150.0f;
    float lineHeight = 30.0f;
    int drawIndex = 0;
    
    for (auto it = g_MissingList.begin(); it != g_MissingList.end(); ) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - it->createdTime).count() / 1000.0f;
        
        if (elapsed >= it->duration) {
            it = g_MissingList.erase(it);
            continue;
        }
        
        float alpha = 255.0f;
        if (elapsed > it->duration - 0.5f) {
            alpha = 255.0f * (it->duration - elapsed) / 0.5f;
        }
        
        Color c = it->color;
        c.a = alpha;
        
        Vector2 pos(20.0f, startY + drawIndex * lineHeight);
        esp.DrawText(c, it->text.c_str(), pos, 22.0f);
        
        drawIndex++;
        ++it;
    }
}

// ==================== CLEAR ALL ====================
inline void ClearMissingNotifications() {
    std::lock_guard<std::mutex> lock(g_MissingMutex);
    g_MissingList.clear();
}
