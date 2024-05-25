#ifndef UNREALSDK_GAME_BL2_BL2_H
#define UNREALSDK_GAME_BL2_BL2_H

#include "unrealsdk/pch.h"

#include "unrealsdk/game/abstract_hook.h"
#include "unrealsdk/game/selector.h"

#if defined(UE3) && defined(ARCH_X64) && !defined(UNREALSDK_IMPORTING)

namespace unrealsdk::game {

class BL2Hook : public AbstractHook {
   protected:
    /**
     * @brief Hex edits out the `obj dump` array limit message.
     */
    virtual void hexedit_array_limit_message(void) const;

    /**
     * @brief Finds `FName::Init`, and sets up such that `fname_init` may be called.
     */
    void find_fname_init(void);

    // Deliberately storing in a void pointer member, because the type changes in bl2/tps
    void* fname_init_ptr = nullptr;

    /**
     * @brief Hooks the antidebug functions and disables them.
     */
    static void hook_antidebug(void);

    /**
     * @brief Hex edits out the protection on the set command.
     */
    static void hexedit_set_command(void);

    /**
     * @brief Hex edits out the `obj dump` array limit.
     */
    static void hexedit_array_limit(void);

    /**
     * @brief Hooks `UObject::ProcessEvent` and points it at the hook manager.
     */
    static void hook_process_event(void);

    /**
     * @brief Hooks `UObject::CallFunction` and points it at the hook manager.
     */
    static void hook_call_function(void);

    /**
     * @brief Finds GObjects, and populates the wrapper member.
     */
    static void find_gobjects(void);

    /**
     * @brief Finds GNames, and sets up such that `gnames` may be called.
     */
    static void find_gnames(void);

    /**
     * @brief Finds `FFrame::Step`, and sets up such that `fframe_step` may be called.
     */
    static void find_fframe_step(void);

    /**
     * @brief Finds `GMalloc`, and sets up such that `malloc`, `realloc`, and `free` may be called.
     */
    static void find_gmalloc(void);

    /**
     * @brief Finds `StaticConstructObject`, and sets up such that `construct_object` may be called.
     */
    static void find_construct_object(void);

    /**
     * @brief Finds `LoadPackage`, and sets up such that `load_package` may be called.
     */
    static void find_load_package(void);

    /**
     * @brief Creates a console and sets the bind (if required), and hooks logging onto it.
     */
    static void inject_console(void);

   public:
    void hook(void) override;

    [[nodiscard]] const unreal::GObjects& gobjects(void) const override;
    [[nodiscard]] const unreal::GNames& gnames(void) const override;
    void fname_init(unreal::FName* name, const wchar_t* str, int32_t number) const override;
    void fframe_step(unreal::FFrame* frame, unreal::UObject* obj, void* param) const override;
    [[nodiscard]] void* u_malloc(size_t len) const override;
    [[nodiscard]] void* u_realloc(void* original, size_t len) const override;
    void u_free(void* data) const override;
    void process_event(unreal::UObject* object,
                       unreal::UFunction* func,
                       void* params) const override;
    [[nodiscard]] unreal::UObject* construct_object(unreal::UClass* cls,
                                                    unreal::UObject* outer,
                                                    const unreal::FName& name,
                                                    decltype(unreal::UObject::ObjectFlags) flags,
                                                    unreal::UObject* template_obj) const override;
    void uconsole_output_text(const std::wstring& str) const override;
    [[nodiscard]] bool is_console_ready(void) const override;
    [[nodiscard]] std::wstring uobject_path_name(const unreal::UObject* obj) const override;
    [[nodiscard]] unreal::UObject* find_object(unreal::UClass* cls,
                                               const std::wstring& name) const override;
    void ftext_as_culture_invariant(unreal::FText* text,
                                    unreal::TemporaryFString&& str) const override;
    [[nodiscard]] unreal::UObject* load_package(const std::wstring& name,
                                                uint32_t flags) const override;
};

template <>
struct GameTraits<BL2Hook> {
    static constexpr auto NAME = "Borderlands 2";

    static bool matches_executable(std::string_view executable) {
        return executable == "Borderlands2.exe" || executable == "TinyTina.exe" || executable == "SpitfireGame.exe";
    }
};

}  // namespace unrealsdk::game

#endif

#endif /* UNREALSDK_GAME_BL2_BL2_H */
