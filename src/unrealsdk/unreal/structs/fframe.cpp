
#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/structs/fframe.h"
#include "unrealsdk/unreal/wrappers/wrapped_struct.h"
#include "unrealsdk/unrealsdk.h"

namespace unrealsdk::unreal {

typedef void (* execFn)(class UObject*, struct FFrame*, void*);
typedef execFn GNatives[0x1000];

uint8_t* FFrame::extract_current_args(WrappedStruct& args) {
    auto args_addr = reinterpret_cast<uintptr_t>(args.base.get());
    uint8_t* original_code = this->Code;

    for (auto prop = reinterpret_cast<UProperty*>(args.type->Children);
         *this->Code != FFrame::EXPR_TOKEN_END_FUNCTION_PARAMS;
         prop = reinterpret_cast<UProperty*>(prop->Next)) {
        if ((prop->PropertyFlags & UProperty::PROP_FLAG_RETURN) != 0) {
            continue;
        }

        uint16_t inst = *(reinterpret_cast<uint16_t*>(this->Code++));
        auto gn = reinterpret_cast<GNatives*>(0x142551690);
        (*gn)[inst](this->Object, this, reinterpret_cast<void*>(args_addr + prop->Offset_Internal));

        //unrealsdk::fframe_step(this, this->Object,
        //                       reinterpret_cast<void*>(args_addr + prop->Offset_Internal));
    }

    return original_code;
}

}  // namespace unrealsdk::unreal
