#pragma once
#include <string>
#include <unordered_map>

namespace Luna {
    struct ItemModelInfo {
        std::string model_path;
        std::string bone_name;
    };

    // Hardcoded fallback entries (minimal set for testing)
    inline std::unordered_map<uint32_t, ItemModelInfo> g_ItemModels = {
        {1001, {"assets_converted/mod_objs/[r]_wing_001.obj", "Bip01 Spine2"}},
        {5001, {"assets_converted/mod_objs/[r]_head_006.obj", "Bip01 Head"}},
    };

    // Try to include auto-generated entries from the pipeline
    // If the generated file exists, its entries will be merged at runtime
    inline void InitItemModels() {
        // Generated entries are loaded via g_ItemModelsGenerated
        // when available at build time
    }

    inline ItemModelInfo GetItemModel(uint32_t item_id) {
        auto it = g_ItemModels.find(item_id);
        if (it != g_ItemModels.end()) return it->second;
        return {"", ""};
    }

    // Merge generated entries into the main table
    inline void MergeGeneratedModels(
        const std::unordered_map<uint32_t, ItemModelInfo>& generated) {
        for (const auto& [id, info] : generated) {
            if (!g_ItemModels.count(id)) {
                g_ItemModels[id] = info;
            }
        }
    }
}

// Include auto-generated entries if available at build time
#if __has_include("ItemModelTable_generated.hpp")
#include "ItemModelTable_generated.hpp"
#endif
