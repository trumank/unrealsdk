#include "unrealsdk/pch.h"

#include "unrealsdk/game/bl2/bl2.h"
#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/wrappers/gnames.h"
#include "unrealsdk/unreal/wrappers/gobjects.h"

#if defined(UE3) && defined(ARCH_X64) && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

namespace {

GObjects gobjects_wrapper{};

const constinit Pattern<19> GOBJECTS_SIG{
    "63 44 24 ?? 85 C0 78 ?? 3B 05 ?? ?? ?? ?? 7D ?? 48 8B C8"
    ,
    10};

}  // namespace

void BL2Hook::find_gobjects(void) {
    auto gobject_res = GOBJECTS_SIG.sigscan();
    LOG(MISC, "GObjects scan: {}", gobject_res);
    auto gobjects_ptr = reinterpret_cast<GObjects::internal_type>(read_offset(gobject_res) - 8);
    LOG(MISC, "GObjects: {:p}", reinterpret_cast<void*>(gobjects_ptr));

    gobjects_wrapper = GObjects(gobjects_ptr);
}

const GObjects& BL2Hook::gobjects(void) const {
    return gobjects_wrapper;
};

namespace {

GNames gnames_wrapper{};

const constinit Pattern<16> GNAMES_SIG{
    "48 8B DA 48 8B F1 85 FF 78 ?? 8B 05 ????????"
    ,
    12};

}  // namespace

void BL2Hook::find_gnames(void) {
    auto gnames_ptr = reinterpret_cast<GNames::internal_type>(read_offset(GNAMES_SIG.sigscan()) - 8);
    LOG(MISC, "GNames: {:p}", reinterpret_cast<void*>(gnames_ptr));

    gnames_wrapper = GNames(gnames_ptr);
}

const GNames& BL2Hook::gnames(void) const {
    return gnames_wrapper;
}

}  // namespace unrealsdk::game

#endif
