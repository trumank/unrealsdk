#include "unrealsdk/pch.h"

#include "unrealsdk/game/bl2/bl2.h"
#include "unrealsdk/hook_manager.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/classes/ufunction.h"
#include "unrealsdk/unreal/classes/uobject.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/structs/fframe.h"
#include "unrealsdk/unreal/wrappers/bound_function.h"
#include "unrealsdk/unreal/wrappers/property_proxy.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer.h"
#include "unrealsdk/unreal/wrappers/unreal_pointer_funcs.h"
#include "unrealsdk/unreal/wrappers/wrapped_struct.h"
#include "unrealsdk/unrealsdk.h"

#if defined(UE3) && defined(ARCH_X64) && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

namespace {

// This function is actually thiscall, but MSVC won't let us declare static thiscall functions
// As a workaround, declare it fastcall, and add a dummy edx arg.
// NOLINTNEXTLINE(modernize-use-using)
typedef void(__fastcall* process_event_func)(UObject* obj,
                                             UFunction* func,
                                             void* params,
                                             void* /*null*/);
process_event_func process_event_ptr;

const constinit Pattern<13> PROCESS_EVENT_SIG{
    "40 55 41 55 41 56 48 81 EC C0 00 00 00"
};

void __fastcall process_event_hook(UObject* obj,
                                   UFunction* func,
                                   void* params,
                                   void* null) {
    try {
        // This arg seems to be in the process of being deprecated, no usage in ghidra, always seems
        // to be null, and it's gone in later ue versions. Gathering some extra info just in case.
        //if (null != nullptr) {
        //    LOG(DEV_WARNING, L"Null param had a value in process event during func {} on obj {}",
        //        func->get_path_name(), obj->get_path_name());
        //}

            //LOG(INFO, "HUH -1 obj={:p} func={:p}", reinterpret_cast<void*>(obj), reinterpret_cast<void*>(func));
        auto data = hook_manager::impl::preprocess_hook("ProcessEvent", func, obj);
            //LOG(INFO, "HUH 0");
        if (data != nullptr) {
            // Copy args so that hooks can't modify them, for parity with call function
            //LOG(INFO, "HUH 0.05");
            const WrappedStruct args_base{func, params};
            //LOG(INFO, "HUH 0.1");
            WrappedStruct args = args_base.copy_params_only();
            //LOG(INFO, "HUH 0.2");
            hook_manager::Details hook{obj, &args, {func->find_return_param()}, {func, obj}};
            //LOG(INFO, "HUH 1");

            const bool block_execution =
                hook_manager::impl::run_hooks_of_type(*data, hook_manager::Type::PRE, hook);
            //LOG(INFO, "HUH 2");

            if (!block_execution) {
                process_event_ptr(obj, func, params, null);
            }
            //LOG(INFO, "HUH 3");

            if (hook.ret.has_value()) {
                hook.ret.copy_to(reinterpret_cast<uintptr_t>(params));
            }
            //LOG(INFO, "HUH 4");

            if (!hook_manager::impl::has_post_hooks(*data)) {
                return;
            }

            if (hook.ret.prop != nullptr && !hook.ret.has_value() && !block_execution) {
                hook.ret.copy_from(reinterpret_cast<uintptr_t>(params));
            }

            if (!block_execution) {
                hook_manager::impl::run_hooks_of_type(*data, hook_manager::Type::POST, hook);
            }

            hook_manager::impl::run_hooks_of_type(*data, hook_manager::Type::POST_UNCONDITIONAL,
                                                  hook);

            return;
        }
    } catch (const std::exception& ex) {
        LOG(ERROR, "An exception occurred during the ProcessEvent hook: {}", ex.what());
    }

    //LOG(INFO, "process event continuing");
    process_event_ptr(obj, func, params, null);
}
static_assert(std::is_same_v<decltype(&process_event_hook), process_event_func>,
              "process_event signature is incorrect");

}  // namespace

void BL2Hook::hook_process_event(void) {
    detour(PROCESS_EVENT_SIG.sigscan(), process_event_hook, &process_event_ptr, "ProcessEvent");
}

void BL2Hook::process_event(UObject* object, UFunction* func, void* params) const {
    process_event_hook(object, func, params, nullptr);
}

namespace {

// NOLINTNEXTLINE(modernize-use-using)
typedef void(__fastcall* call_function_func)(UObject* obj,
                                             FFrame* stack,
                                             void* params,
                                             UFunction* func);
call_function_func call_function_ptr;

const constinit Pattern<20> CALL_FUNCTION_SIG{
    "40 55 53 56 57 41 54 41 55 41 56 41 57 48 81 EC C8 04 00 00"
};

void __fastcall call_function_hook(UObject* obj,
                                   FFrame* stack,
                                   void* result,
                                   UFunction* func) {
    try {
        auto data = hook_manager::impl::preprocess_hook("ProcessEvent", func, obj);
        if (data != nullptr) {
            WrappedStruct args{func};
            auto original_code = stack->extract_current_args(args);

            hook_manager::Details hook{obj, &args, {func->find_return_param()}, {func, obj}};

            const bool block_execution =
                hook_manager::impl::run_hooks_of_type(*data, hook_manager::Type::PRE, hook);

            if (block_execution) {
                stack->Code++;
            } else {
                stack->Code = original_code;
                call_function_ptr(obj, stack, result, func);
            }

            if (hook.ret.has_value()) {
                // Result is a pointer directly to where the property should go, remove the offset
                hook.ret.copy_to(reinterpret_cast<uintptr_t>(result)
                                 - hook.ret.prop->Offset_Internal);
            }

            if (!hook_manager::impl::has_post_hooks(*data)) {
                return;
            }

            if (hook.ret.prop != nullptr && !hook.ret.has_value() && !block_execution) {
                hook.ret.copy_from(reinterpret_cast<uintptr_t>(result)
                                   - hook.ret.prop->Offset_Internal);
            }

            if (!block_execution) {
                hook_manager::impl::run_hooks_of_type(*data, hook_manager::Type::POST, hook);
            }

            hook_manager::impl::run_hooks_of_type(*data, hook_manager::Type::POST_UNCONDITIONAL,
                                                  hook);

            return;
        }
    } catch (const std::exception& ex) {
        LOG(ERROR, "An exception occurred during the CallFunction hook: {}", ex.what());
    }

    call_function_ptr(obj, stack, result, func);
}
static_assert(std::is_same_v<decltype(&call_function_hook), call_function_func>,
              "call_function signature is incorrect");

}  // namespace

void BL2Hook::hook_call_function(void) {
    detour(CALL_FUNCTION_SIG.sigscan(), call_function_hook, &call_function_ptr, "CallFunction");
}

}  // namespace unrealsdk::game

#endif
