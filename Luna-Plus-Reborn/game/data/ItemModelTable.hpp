#pragma once
#include <string>
#include <unordered_map>

namespace Luna {
    struct ItemModelInfo {
        std::string model_path;
        std::string bone_name;
    };

    inline std::unordered_map<uint32_t, ItemModelInfo> g_ItemModels = {
        // Weapons (Simplified to existing obj for demo)
        {1001, {"assets_converted/mod_objs/[r]_wing_001.obj", "Bip01 Spine2"}},
        
        // Parts
        {5001, {"assets_converted/mod_objs/[r]_head_006.obj", "Bip01 Head"}},
    };

    inline ItemModelInfo GetItemModel(uint32_t item_id) {
        if (g_ItemModels.count(item_id)) return g_ItemModels[item_id];
        return {"", ""};
    }
}
