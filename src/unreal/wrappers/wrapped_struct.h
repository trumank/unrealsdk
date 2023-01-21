#ifndef UNREAL_WRAPPERS_WRAPPED_STRUCT_H
#define UNREAL_WRAPPERS_WRAPPED_STRUCT_H

#include "unreal/structs/fname.h"
#include "unreal/wrappers/prop_traits.h"

namespace unrealsdk::unreal {

class UStruct;

class WrappedStruct {
   public:
    UStruct* type;
    void* base;

    /**
     * @brief Gets a property on this struct.
     *
     * @tparam T The type of the property.
     * @param name The property's name.
     * @param idx The fixed array index to get the value at. Defaults to 0.
     * @return The property's new value.
     */
    template <typename T>
    [[nodiscard]] typename PropTraits<T>::Value get(const FName& name, size_t idx = 0);

    /**
     * @brief Sets a property on this struct
     *
     * @tparam T The type of the property.
     * @param name The property's name.
     * @param idx The fixed array index to set the value at. Defaults to 0.
     * @param value The property's value.
     */
    template <typename T>
    void set(const FName& name, typename PropTraits<T>::Value value) {
        this->set<T>(name, value, 0);
    }
    template <typename T>
    void set(const FName& name, size_t idx, typename PropTraits<T>::Value value);
};

}  // namespace unrealsdk::unreal

#endif /* UNREAL_WRAPPERS_WRAPPED_STRUCT_H */
