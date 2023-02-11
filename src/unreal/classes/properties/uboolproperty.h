#ifndef UNREAL_CLASSES_PROPERTIES_UBOOLPROPERTY_H
#define UNREAL_CLASSES_PROPERTIES_UBOOLPROPERTY_H

#include "pch.h"

#include "unreal/classes/uproperty.h"
#include "unreal/wrappers/prop_traits.h"

namespace unrealsdk::unreal {

#if defined(_MSC_VER) && defined(ARCH_X86)
#pragma pack(push, 0x4)
#endif

class UBoolProperty : public UProperty {
   private:
    friend PropTraits<UBoolProperty>;

    // NOLINTBEGIN(readability-identifier-naming)

#ifdef UE4
    uint8_t FieldSize;
    uint8_t ByteOffset;
    uint8_t ByteMask;
    uint8_t FieldMask;
#else
    uint32_t FieldMask;
#endif

    // NOLINTEND(readability-identifier-naming)
};

template <>
struct PropTraits<UBoolProperty> {
    using Value = bool;
    static inline const wchar_t* const CLASS = L"BoolProperty";

    static Value get(const UBoolProperty* prop, uintptr_t addr);
    static void set(const UBoolProperty* prop, uintptr_t addr, Value value);
};

#if defined(_MSC_VER) && defined(ARCH_X86)
#pragma pack(pop)
#endif

}  // namespace unrealsdk::unreal

#endif /* UNREAL_CLASSES_PROPERTIES_UBOOLPROPERTY_H */
