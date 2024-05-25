#include "unrealsdk/pch.h"

#include "unrealsdk/game/bl2/bl2.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/classes/uobject.h"
#include "unrealsdk/unreal/structs/fframe.h"
#include "unrealsdk/unreal/structs/fname.h"
#include "unrealsdk/unreal/structs/fstring.h"
#include "unrealsdk/unreal/structs/ftext.h"
#include "unrealsdk/version_error.h"

#if defined(UE3) && defined(ARCH_X64) && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

void BL2Hook::hook(void) {
    // Make sure to do antidebug asap
    hook_antidebug();

    hook_process_event();
    hook_call_function();

    find_gobjects();
    find_gnames();
    find_fname_init();
    find_fframe_step();
    find_gmalloc();
    find_construct_object();
    find_load_package();

    inject_console();

    hexedit_set_command();
    hexedit_array_limit();
    hexedit_array_limit_message();
}

#if defined(__MINGW32__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"  // thiscall on non-class
#endif

#pragma region FName::Init

namespace {

const constinit Pattern<19> FNAME_INIT_SIG{
    "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC D0 0C 00 00"
};

}

void BL2Hook::find_fname_init(void) {
    this->fname_init_ptr = FNAME_INIT_SIG.sigscan<void*>();
    LOG(MISC, "FName::Init: {:p}", this->fname_init_ptr);
}

void BL2Hook::fname_init(FName* name, const wchar_t* str, int32_t number) const {
    // NOLINTNEXTLINE(modernize-use-using)  - need a typedef for the __thiscall
    typedef void*(__thiscall * fname_init_func)(FName * name, const wchar_t* str, int32_t number,
                                                int32_t find_type, int32_t split_name);

    reinterpret_cast<fname_init_func>(this->fname_init_ptr)(name, str, number, 1, 1);
}

#pragma endregion

#pragma region FFrame::Step

namespace {

// NOLINTNEXTLINE(modernize-use-using)
typedef void(__thiscall* fframe_step_func)(UObject* obj, FFrame* stack, void* param);
fframe_step_func fframe_step_ptr;

const constinit Pattern<30> FFRAME_STEP_SIG{
    // matches multiple but the first is the one we want anyway
    "48 8b 42 24 48 ff c0 44 0f b6 48 ff 48 89 42 24 41 8b c1 4c 8d 0d ?? ?? ?? ?? 49 ff 24 c1"
};

}  // namespace

void BL2Hook::find_fframe_step(void) {
    fframe_step_ptr = FFRAME_STEP_SIG.sigscan<fframe_step_func>();
    LOG(MISC, "FFrame::Step: {:p}", reinterpret_cast<void*>(fframe_step_ptr));
}
void BL2Hook::fframe_step(FFrame* frame, UObject* obj, void* param) const {
    fframe_step_ptr(obj, frame, param);
}

#pragma endregion

#pragma region FText::AsCultureInvariant

void BL2Hook::ftext_as_culture_invariant(unreal::FText* /*text*/,
                                         unreal::TemporaryFString&& /*str*/) const {
    throw_version_error("FTexts are not implemented in UE3");
}

#pragma endregion

#if defined(__MINGW32__)
#pragma GCC diagnostic pop
#endif

}  // namespace unrealsdk::game

#endif
