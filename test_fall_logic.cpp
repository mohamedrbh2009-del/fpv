// اختبار بسيط للمنطق الرياضي على الكمبيوتر (بدون اللعبة).
#include <cassert>
#include <cstdio>
#include <cstring>
#include "fall_logic.h"

int main() {
    using namespace fall;

    // على الأرض: لا تأثير
    Pose a = ComputePose(0, 0, -10.0f, 1.0f, true);
    assert(a.intensity == 0.f);

    // سقوط بطيء جداً: لا تأثير
    Pose b = ComputePose(0, 0, -1.0f, 1.0f, false);
    assert(b.intensity == 0.f);

    // سقوط سريع: شدة عليا ومحدودة بـ 1
    Pose c = ComputePose(2.0f, 0.0f, -20.0f, 0.5f, false);
    assert(c.intensity == 1.f);
    assert(std::fabs(c.leftArm + c.rightArm) < 1e-5f); // الذراعان متعاكستان

    // سقوط متوسط: شدة بين 0 و 1
    Pose d = ComputePose(0, 0, -9.0f, 0.5f, false);
    assert(d.intensity > 0.f && d.intensity < 1.f);

    // تصنيف الهبوط
    assert(ClassifyLanding(-3.0f)  == Landing::Soft);
    assert(ClassifyLanding(-12.0f) == Landing::Roll);
    assert(ClassifyLanding(-25.0f) == Landing::Collapse);
    assert(std::strcmp(LandingName(Landing::Roll), "roll") == 0);

    std::puts("all tests passed");
    return 0;
}
