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
    int text_id = 0;
    int btn_text_id = 0;
    int tooltip_msg_id = 0;
    int open_sound = -1;
    int close_sound = -1;
    int pushup = 0;
    float scale_x = 1.0f;
    float scale_y = 1.0f;
    float gauge_width = 0;
    float edit_w = 0;
    float edit_h = 0;
    float grid_pad_x = 1.0f;
    float grid_pad_y = 1.0f;
    bool shadow = false;
    UiScriptUV gauge_piece_img{-1, 0,0,0,0};
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
    int list_max_line = 0;
    int middle_num = 0;
    bool show_scroll = false;
    std::string func_name;
    bool moveable = false;
    bool active = true;
    std::vector<UiScriptRect> icon_cells;
    std::vector<UiElement> children;
};

class UiScriptParser {
public:
    static UiElement ParseFile(const std::string& path);
    static std::string WidgetTypeName(const std::string& raw_type);

private:
    struct ParseContext {
        std::ifstream file;
        std::string current_line;
    };

    static UiElement ParseStream(ParseContext& ctx);
    static UiElement ParseBlock(ParseContext& ctx, const std::string& name);
    static UiScriptUV ParseImage(const std::string& line);
    static UiScriptRect ParseRect(const std::string& line);
    static uint32_t ParseColor(const std::string& line);
    static std::string Trim(const std::string& s);
};
