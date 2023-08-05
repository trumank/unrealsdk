/*
====================================================================================================

This file contains forward declarations of all the C api functions used for the main `unrealsdk`
namespace. It is included directly in the other two files.

====================================================================================================
*/

#include "unrealsdk/pch.h"

#include "unrealsdk/unreal/classes/uobject.h"

namespace unrealsdk::unreal {

class GNames;
class GObjects;
class UClass;
class UFunction;
struct FFrame;
struct FName;
struct FText;
struct TemporaryFString;

}  // namespace unrealsdk::unreal

using namespace unrealsdk::unreal;

namespace unrealsdk {

UNREALSDK_CAPI([[nodiscard]] bool, is_initialized);
UNREALSDK_CAPI([[nodiscard]] bool, is_console_ready);
UNREALSDK_CAPI([[nodiscard]] const GObjects*, gobjects);
UNREALSDK_CAPI([[nodiscard]] const GNames*, gnames);
UNREALSDK_CAPI(void, fname_init, FName* name, const wchar_t* str, int32_t number);
UNREALSDK_CAPI(void, fframe_step, FFrame* frame, UObject* obj, void* param);
UNREALSDK_CAPI([[nodiscard]] void*, u_malloc, size_t len);
UNREALSDK_CAPI([[nodiscard]] void*, u_realloc, void* original, size_t len);
UNREALSDK_CAPI(void, u_free, void* data);
UNREALSDK_CAPI(void, process_event, UObject* object, UFunction* function, void* params);
UNREALSDK_CAPI([[nodiscard]] UObject*,
               construct_object,
               UClass* cls,
               UObject* outer,
               const FName* name = nullptr,
               decltype(UObject::ObjectFlags) flags = 0,
               UObject* template_obj = nullptr);
UNREALSDK_CAPI(void, uconsole_output_text, const wchar_t* str, size_t size);
UNREALSDK_CAPI([[nodiscard]] wchar_t*, uobject_path_name, const UObject* obj, size_t& size);
UNREALSDK_CAPI([[nodiscard]] UObject*,
               find_object,
               UClass* cls,
               const wchar_t* name,
               size_t name_size);

#ifdef UE4
UNREALSDK_CAPI(void, ftext_as_culture_invariant, FText* text, TemporaryFString&& str);
#endif

}  // namespace unrealsdk
