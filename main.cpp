// main.cpp - مود AML: كشف سقوط الشخصيات (مرحلة 1: تسجيل فقط).
// يجلب الدوال بأسمائها من libGTASA.so، ويحسب سرعة السقوط من حركة عظمة الجذع،
// فلا يحتاج مواضع حقول داخل الشخصية.
#include <mod/amlmod.h>
#include <mod/logger.h>
#include <time.h>
#include <unordered_map>

#include "fall_logic.h"
#include "offsets.h"

MYMOD(net.example.fallsoft, FallSoft, 0.2, YourName)

struct RwV3d { float x, y, z; };

typedef void (*GetBonePosition_t)(void* ped, RwV3d& out, unsigned int boneId, bool updateSkin);
typedef void (*PedPreRender_t)(void* ped);

static GetBonePosition_t GetBonePosition = nullptr;
static PedPreRender_t    PedPreRender_orig = nullptr;

struct Track {
    RwV3d  pos   = {0, 0, 0};
    double t     = 0;
    float  vz    = 0;
    bool   valid = false;
};
static std::unordered_map<void*, Track> g_track;

static double NowSeconds() {
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

// تُنفَّذ لكل شخصية معروضة كل إطار.
static void PedPreRender_hook(void* ped) {
    PedPreRender_orig(ped); // أولاً دع اللعبة تحدّث الأنيميشن كالمعتاد

    RwV3d p;
    GetBonePosition(ped, p, syms::TRACK_BONE, false);
    const double now = NowSeconds();

    if (g_track.size() > 1024) g_track.clear(); // منع نمو الذاكرة
    Track& tr = g_track[ped];

    if (tr.valid) {
        const double dt = now - tr.t;
        if (dt > 0.001 && dt < 0.25) {
            const float vx = (float)((p.x - tr.pos.x) / dt);
            const float vy = (float)((p.y - tr.pos.y) / dt);
            const float vz = (float)((p.z - tr.pos.z) / dt);

            // تجاهل القفزات الغريبة (انتقال فوري أو إعادة ظهور)
            if (vz > -80.f && vz < 80.f) {
                fall::Pose pose = fall::ComputePose(vx, vy, vz, (float)now, false);

                // لحظة الهبوط: كان يسقط بقوة والآن توقف تقريباً
                if (tr.vz < -fall::Params().minFallSpeed && vz > -2.0f) {
                    logger->Info("landing: impact=%.1f m/s type=%s",
                                 tr.vz, fall::LandingName(fall::ClassifyLanding(tr.vz)));
                }

                // أثناء السقوط: سجّل بين وقت وآخر فقط
                static int counter = 0;
                if (pose.intensity > 0.3f && (++counter % 30) == 0) {
                    logger->Info("falling: vz=%.1f intensity=%.2f L=%.2f R=%.2f",
                                 vz, pose.intensity, pose.leftArm, pose.rightArm);
                }
                // TODO المرحلة 2: طبّق pose على عظام الذراعين والجذع.
                tr.vz = vz;
            }
        }
    }
    tr.pos = p;
    tr.t = now;
    tr.valid = true;
}

extern "C" void OnModLoad() {
    logger->SetTag("FallSoft");

    void* lib = aml->GetLibHandle("libGTASA.so");
    if (!lib) {
        logger->Error("libGTASA.so غير موجودة");
        return;
    }

    const uintptr_t pre = aml->GetSym(lib, syms::PED_PRERENDER);
    const uintptr_t gbp = aml->GetSym(lib, syms::PED_GETBONEPOS);
    if (!pre || !gbp) {
        logger->Error("لم أجد الرموز: PreRender=%p GetBonePosition=%p", (void*)pre, (void*)gbp);
        return;
    }

    GetBonePosition = (GetBonePosition_t)gbp;
    const bool ok = aml->Hook((void*)pre, (void*)PedPreRender_hook, (void**)&PedPreRender_orig);
    logger->Info("hook CPed::PreRender: %s", ok ? "ok" : "FAILED");
}
