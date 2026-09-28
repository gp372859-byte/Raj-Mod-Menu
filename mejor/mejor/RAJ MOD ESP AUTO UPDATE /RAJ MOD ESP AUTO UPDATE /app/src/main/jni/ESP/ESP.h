#ifndef ESP_H
#define ESP_H

#include <jni.h>

// --- Suas estruturas básicas ---

class ESP {
private:
    // Objetos Java
    JNIEnv* _env;
    jobject _cvsView;
    jobject _cvs;

    // Pontos padrão da linha
    Vector2 _lineStart{0.0f, 0.0f};
    Vector2 _lineEnd{0.0f, 0.0f};
    bool    _lineReady{false};

public:
    /* --------------------------------------------------------------------- */
    /*  Construtores                                                         */
    /* --------------------------------------------------------------------- */
    ESP() = default;

    ESP(JNIEnv* env, jobject cvsView, jobject cvs)
            : _env(env), _cvsView(cvsView), _cvs(cvs) {}

    /* --------------------------------------------------------------------- */
    /*  Utilidades gerais                                                    */
    /* --------------------------------------------------------------------- */
    bool isValid() const {
        return _env && _cvsView && _cvs;
    }

    int getWidth() const {
        if (isValid()) {
            jclass canvas = _env->GetObjectClass(_cvs);
            jmethodID width = _env->GetMethodID(canvas, "getWidth", "()I");
            return _env->CallIntMethod(_cvs, width);
        }
        return 0;
    }

    int getHeight() const {
        if (isValid()) {
            jclass canvas = _env->GetObjectClass(_cvs);
            jmethodID height = _env->GetMethodID(canvas, "getHeight", "()I");
            return _env->CallIntMethod(_cvs, height);
        }
        return 0;
    }

    /* --------------------------------------------------------------------- */
    /*  Linha – NOVO fluxo simplificado                                      */
    /* --------------------------------------------------------------------- */

    /// Define os pontos a serem usados pela DrawLine(color,thickness)
    void SetLine(const Vector2& start, const Vector2& end) {
        _lineStart  = start;
        _lineEnd    = end;
        _lineReady  = true;
    }

    /// Versão simplificada: usa os pontos já definidos por SetLine()
    void DrawLine(Color color, float thickness) {
        if (_lineReady)
            DrawLine(color, thickness, _lineStart, _lineEnd);
    }

    /// Versão original (mantida) – pontos explícitos
    void DrawLine(Color color, float thickness, Vector2 start, Vector2 end) {
        if (isValid()) {
            jclass canvasView = _env->GetObjectClass(_cvsView);
            jmethodID drawline = _env->GetMethodID(
                    canvasView, "DrawLine",
                    "(Landroid/graphics/Canvas;IIIIFFFFF)V");

            _env->CallVoidMethod(_cvsView, drawline, _cvs,
                                 (int)color.a, (int)color.r, (int)color.g, (int)color.b,
                                 thickness,
                                 start.X, start.Y, end.X, end.Y);
        }
    }

    /* --------------------------------------------------------------------- */
    /*  Texto                                                                */
    /* --------------------------------------------------------------------- */
    void DrawText(Color color, const char* txt, Vector2 pos, float size) {
        if (isValid()) {
            jclass canvasView = _env->GetObjectClass(_cvsView);
            jmethodID drawtext = _env->GetMethodID(
                    canvasView, "DrawText",
                    "(Landroid/graphics/Canvas;IIIILjava/lang/String;FFF)V");

            _env->CallVoidMethod(_cvsView, drawtext, _cvs,
                                 (int)color.a, (int)color.r, (int)color.g, (int)color.b,
                                 _env->NewStringUTF(txt), pos.X, pos.Y, size);
        }
    }

    /* --------------------------------------------------------------------- */
    /*  Círculo preenchido                                                   */
    /* --------------------------------------------------------------------- */
    void DrawFilledCircle(Color color, Vector2 pos, float radius) {
        if (isValid()) {
            jclass canvasView = _env->GetObjectClass(_cvsView);
            jmethodID drawfilledcircle = _env->GetMethodID(
                    canvasView, "DrawFilledCircle",
                    "(Landroid/graphics/Canvas;IIIIFFF)V");

            _env->CallVoidMethod(_cvsView, drawfilledcircle, _cvs,
                                 (int)color.a, (int)color.r, (int)color.g, (int)color.b,
                                 pos.X, pos.Y, radius);
        }
    }


    void DrawFilledRect(Color color, Rect rect) {
        if (isValid()) {
            jclass canvasView = _env->GetObjectClass(_cvsView);
            jmethodID drawfilledrect = _env->GetMethodID(
                    canvasView, "DrawFilledRect",
                    "(Landroid/graphics/Canvas;IIIIFFFF)V");

            _env->CallVoidMethod(_cvsView, drawfilledrect, _cvs,
                                 (int)color.a, (int)color.r, (int)color.g, (int)color.b,
                                 rect.x, rect.y, rect.w, rect.h);
        }
    }


    /* --------------------------------------------------------------------- */
    /*  Caixa (retângulo)                                                    */
    /* --------------------------------------------------------------------- */
    void DrawBox(Color color, float stroke, Rect rect) {
        Vector2 v1(rect.x,           rect.y);
        Vector2 v2(rect.x + rect.w,  rect.y);
        Vector2 v3(rect.x + rect.w,  rect.y + rect.h);
        Vector2 v4(rect.x,           rect.y + rect.h);

        DrawLine(color, stroke, v1, v2);
        DrawLine(color, stroke, v2, v3);
        DrawLine(color, stroke, v3, v4);
        DrawLine(color, stroke, v4, v1);
    }

    /* --------------------------------------------------------------------- */
    /*  Barra de vida horizontal                                             */
    /* --------------------------------------------------------------------- */
    void DrawHorizontalHealthBar(
            Vector2 screenPos, float width,
            float maxHealth,   float currentHealth)
    {
        screenPos -= Vector2(0.0f, 8.0f);
        DrawBox(Color(0,0,0,255), 3, Rect(screenPos.X, screenPos.Y, width + 2, 5.0f));

        screenPos += Vector2(1.0f, 1.0f);

        float hpWidth = (currentHealth * width) / maxHealth;
        Color clr = Color(0, 255, 0, 255);
        if (currentHealth <= maxHealth * 0.6f) clr = Color(255, 255, 0, 255);
        if (currentHealth <= maxHealth * 0.3f) clr = Color(255,   0, 0, 255);

        DrawBox(clr, 3, Rect(screenPos.X, screenPos.Y, hpWidth, 3.0f));
    }

    /* --------------------------------------------------------------------- */
    /*  Retículo (crosshair)                                                 */
    /* --------------------------------------------------------------------- */
    void DrawCrosshair(Color clr, Vector2 center, float size = 20.0f) {
        float half = size * 0.5f;
        DrawLine(clr, 3, Vector2(center.X - half, center.Y), Vector2(center.X + half, center.Y));
        DrawLine(clr, 3, Vector2(center.X, center.Y - half), Vector2(center.X, center.Y + half));
    }
};

#endif // ESP_H
