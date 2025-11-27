#include "hkx.h"
#include "resource_manager.h"

void parse_anim(const char *hkxPath)
{
    auto hkxData = godot::TESVResourceManager::get_file_raw(hkxPath);
    HKX hkx(hkxData);
    hkx.rootContainer->m_namedVariants;
}