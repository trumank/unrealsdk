#include "pch.h"

#include "unreal/classes/uobject.h"
#include "unreal/wrappers/gobjects.h"

#ifdef UE4
#include "unreal/structs/gobjects.h"
#else
#include "unreal/structs/tarray.h"
#endif

namespace unrealsdk::unreal {

#pragma region Iterator

GObjects::Iterator::Iterator(const GObjects& gobjects, size_t idx) : gobjects(gobjects), idx(idx) {}

GObjects::Iterator::reference GObjects::Iterator::operator*() const {
    return this->gobjects.obj_at(idx);
}

GObjects::Iterator& GObjects::Iterator::operator++() {
    ++this->idx;
    return *this;
}
GObjects::Iterator GObjects::Iterator::operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
}

bool GObjects::Iterator::operator==(const GObjects::Iterator& rhs) const {
    return this->idx == rhs.idx;
};
bool GObjects::Iterator::operator!=(const GObjects::Iterator& rhs) const {
    return !(*this == rhs);
};


GObjects::Iterator GObjects::end(void) const {
    return {*this, this->size()};
}

#pragma endregion

GObjects::GObjects(void) : internal(nullptr) {}
GObjects::GObjects(internal_type internal) : internal(internal) {}

#if defined(UE4)

GObjects::Iterator GObjects::begin(void) const {
    return {*this, static_cast<size_t>(std::max(this->internal->ObjFirstGCIndex, 0))};
}

size_t GObjects::size(void) const {
    return this->internal->ObjLastNonGCIndex - std::max(this->internal->ObjFirstGCIndex, 0);
}

UObject* GObjects::obj_at(size_t idx) const {
    if (idx > this->internal->ObjObjects.Count) {
        throw std::out_of_range("GObjects index out of range");
    }
    return this->internal->ObjObjects.at(idx)->Object;
}

#elif defined(UE3)

GObjects::Iterator GObjects::begin(void) const {
    return {*this, 0};
}

size_t GObjects::size(void) const {
    return this->internal->size();
}

UObject* GObjects::obj_at(size_t idx) const {
    return this->internal->at(idx);
}

#else
#error Unknown UE version
#endif

}  // namespace unrealsdk::unreal
