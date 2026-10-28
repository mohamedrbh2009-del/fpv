// fall_logic.h
// الجزء الرياضي فقط (لا يعتمد على اللعبة ولا على AML) ليمكن اختباره على الكمبيوتر.
// كل السرعات هنا بالمتر/الثانية.
#pragma once
#include <cmath>
#include <algorithm>

namespace fall {

struct Params {
    float minFallSpeed   = 5.0f;        // أقل سرعة سقوط (م/ث) لبدء التأثير
    float intensityScale = 1.0f / 15.f; // 15 م/ث = شدة قصوى (1.0)
    float swingFreq      = 12.0f;       // تردد تأرجح الذراعين (راديان/ثانية)
    float armSwing       = 0.8f;        // أقصى زاوية تأرجح للذراعين (راديان)
    float spineTilt      = 0.3f;        // أقصى ميلان للجذع (راديان)
};

struct Pose {
    float leftArm    = 0.f;
    float rightArm   = 0.f;
    float spinePitch = 0.f;
    float spineRoll  = 0.f;
    float intensity  = 0.f;
};

// vz: السرعة العمودية (سالبة = سقوط). vx, vy: السرعة الأفقية.
// time: الزمن بالثواني. onGround: هل الشخصية على الأرض.
inline Pose ComputePose(float vx, float vy, float vz, float time, bool onGround,
                        const Params& p = Params()) {
    Pose out;
    if (onGround || vz > -p.minFallSpeed) return out;

    out.intensity = std::clamp((-vz) * p.intensityScale, 0.f, 1.f);
    const float phase = time * p.swingFreq;
    out.leftArm  = std::sin(phase) * p.armSwing * out.intensity;
    out.rightArm = std::sin(phase + 3.14159265f) * p.armSwing * out.intensity;

    // الجذع يميل باتجاه الحركة الأفقية (تقريب بسيط)
    const float h = std::sqrt(vx * vx + vy * vy);
    const float dir = std::clamp(h / 5.f, 0.f, 1.f);
    out.spinePitch = p.spineTilt * out.intensity * dir;
    out.spineRoll  = p.spineTilt * 0.5f * out.intensity * std::sin(phase * 0.5f);
    return out;
}

// نوع الهبوط حسب سرعة السقوط (م/ث) لحظة لمس الأرض.
enum class Landing { Soft, Roll, Collapse };

inline const char* LandingName(Landing l) {
    switch (l) {
        case Landing::Soft:     return "soft";
        case Landing::Roll:     return "roll";
        default:                return "collapse";
    }
}

inline Landing ClassifyLanding(float impactVz) {
    const float s = -impactVz;
    if (s < 8.0f)  return Landing::Soft;
    if (s < 16.0f) return Landing::Roll;
    return Landing::Collapse;
}

} // namespace fall
