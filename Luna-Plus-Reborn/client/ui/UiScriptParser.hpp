#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <rendering/UIRenderer.hpp>

struct UiScriptRect { float x, y, w, h; };
struct UiScriptUV { int atlas; float u1, v1, u2, v2; };

struct UiElement {
    std::string type;
    std::string id;
    std::string text;
    UiScriptRect rect{0,0,0,0};
    UiScriptRect caption_rect{0,0,0,0};
    UiScriptRect grid_cell{0,0,0,0};
    UiScriptUV basic_img{-1, 0,0,0,0};
    UiScriptUV over_img{-1, 0,0,0,0};
    UiScriptUV press_img{-1, 0,0,0,0};
    UiScriptUV icon_cell_bg{-1, 0,0,0,0};
    UiScriptUV dragover_bg{-1, 0,0,0,0};
    uint32_t fg_color = 0xffffffff;
    int text_align = 0;
    int alpha = 255;
    int grid_cols = 0;
    int grid_rows = 0;
    std::string func_name;
    bool moveable = false;
    bool active = true;
    std::vector<UiScriptRect> icon_cells;
    std::vector<UiElement> children;
};

class UiScriptParser {
public:
    static UiElement ParseFile(const std::string& path);

private:
    struct ParserContext {
        std::ifstream file;
        std::string current_line;
    };

    static UiElement ParseBlock(ParserContext& ctx, const std::string& name);
    static UiScriptUV ParseImage(const std::string& line);
    static UiScriptRect ParseRect(const std::string& line);
    static uint32_t ParseColor(const std::string& line);
    static std::string Trim(const std::string& s);
};
