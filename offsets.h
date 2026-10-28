// offsets.h
// أسماء رموز (symbols) مُصدَّرة من libGTASA.so نسخة 2.00 (32 بت)، أُخذت بفحص الملف نفسه.
// لا حاجة لعناوين رقمية: يجلب المود العنوان بالاسم وقت التشغيل عبر aml->GetSym.
#pragma once

namespace syms {

// CPed::PreRender()  -- تُستدعى لكل شخصية معروضة كل إطار
constexpr const char* PED_PRERENDER   = "_ZN4CPed9PreRenderEv";

// CPed::GetBonePosition(RwV3d& out, unsigned int boneId, bool updateSkinBones)
constexpr const char* PED_GETBONEPOS  = "_ZN4CPed15GetBonePositionER5RwV3djb";

// رقم عظمة نتتبعها لحساب سرعة الجسم. أي عظمة من الجذع تصلح للسرعة.
// (عند الحاجة لعظمة الرأس لاحقاً سنعاير الأرقام بالتسجيل.)
constexpr unsigned TRACK_BONE = 3;

} // namespace syms
