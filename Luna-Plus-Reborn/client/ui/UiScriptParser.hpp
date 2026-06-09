#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include <rendering/UIRenderer.hpp>

struct UiScriptRect { float x, y, w, h; };
struct UiScriptUV { int atlas; float u1, v1, u2, v2; };
struct UiScriptCoord { float x, y; };
struct UiScriptColor { uint8_t r, g, b; };

struct UiElement {
    std::string type;
    std::string id;
    std::string text;
    std::string func_name;
    std::string message;
    int text_id = 0;
    int btn_text_id = 0;
    int tooltip_msg_id = 0;
    int font_idx = 0;
    int open_sound = -1;
    int close_sound = -1;
    int pushup = 0;
    int command_num = 0;
    int spin_min = 0, spin_max = 100, spin_unit = 10;
    int limit_line_num = 20;
    int limit_bytes = 256;
    int line_height = 0;
    int list_bar_interval = 0;
    int btn_click_sound = -1;
    int tab_num = 0;
    int component_id = 0;
    float scale_x = 1.0f;
    float scale_y = 1.0f;
    float list_scale_x = 1.0f, list_scale_y = 1.0f;
    float gauge_width = 0;
    float gauge_piece_width = 0;
    float gauge_effect_piece_width = 0;
    float gauge_piece_height_scale = 1.0f;
    float gauge_effect_piece_height_scale = 1.0f;
    float gauge_min_val = 0, gauge_max_val = 100;
    float gauge_cur_val = 0, gauge_ne_val = 0;
    float edit_w = 0;
    float edit_h = 0;
    float spin_w = 0, spin_h = 0;
    float grid_pad_x = 1.0f;
    float grid_pad_y = 1.0f;
    float balloon_border = 8.0f;
    bool shadow = false;
    bool secret = false;
    bool read_only = false;
    bool auto_scroll = true;
    bool passive = false;
    bool line_upward = true;
    bool show_scroll = false;
    bool moveable = false;
    bool active = true;
    bool auto_close = false;
    bool save_position = false;
    bool save_activity = false;
    bool remain_over_image = false;
    bool use_toprow_change = false;
    bool has_extend_color = false;
    uint8_t extend_color_count = 0;
    UiScriptUV gauge_piece_img{-1, 0,0,0,0};
    UiScriptUV gauge_effect_piece_img{-1, 0,0,0,0};
    UiScriptRect rect{0,0,0,0};
    UiScriptRect caption_rect{0,0,0,0};
    UiScriptRect grid_cell{0,0,0,0};
    UiScriptRect text_rect{0,0,0,0};
    UiScriptRect img_src_rect{0,0,0,0};
    UiScriptRect list_bar_point{0,0,0,0};
    UiScriptCoord text_xy{5, 6};
    UiScriptCoord valid_xy{0, 0};
    UiScriptCoord shadow_text_xy{0, 0};
    UiScriptCoord gauge_pos{0, 0};
    UiScriptUV basic_img{-1, 0,0,0,0};
    UiScriptUV over_img{-1, 0,0,0,0};
    UiScriptUV list_over_img{-1, 0,0,0,0};
    UiScriptUV select_img{-1, 0,0,0,0};
    UiScriptUV press_img{-1, 0,0,0,0};
    UiScriptUV focus_img{-1, 0,0,0,0};
    UiScriptUV tooltip_img{-1, 0,0,0,0};
    UiScriptUV top_img{-1, 0,0,0,0};
    UiScriptUV middle_img{-1, 0,0,0,0};
    UiScriptUV down_img{-1, 0,0,0,0};
    UiScriptUV head_img{-1, 0,0,0,0};
    UiScriptUV body_img{-1, 0,0,0,0};
    UiScriptUV list_bar_img{-1, 0,0,0,0};
    UiScriptUV checkbox_img{-1, 0,0,0,0};
    UiScriptUV check_img{-1, 0,0,0,0};
    UiScriptUV icon_cell_bg{-1, 0,0,0,0};
    UiScriptUV dragover_bg{-1, 0,0,0,0};
    UiScriptUV balloon_top{-1, 0,0,0,0};
    UiScriptUV balloon_middle{-1, 0,0,0,0};
    UiScriptUV balloon_bottom{-1, 0,0,0,0};
    UiScriptUV balloon_tail{-1, 0,0,0,0};
    uint32_t fg_color = 0xffffffff;
    uint32_t text_color = 0xffffffff;
    uint32_t caret_color = 0x00000000;
    uint32_t shadow_color = 0x0a0a0a;
    uint32_t tooltip_color = 0xffffff;
    uint32_t combo_text_color = 0xffffff;
    uint32_t column_color = 0xffff00;
    uint32_t select_column_color = 0x00ffff;
    uint32_t extend_colors[5] = {0,0,0,0,0};
    int text_align = 0;
    int alpha = 255;
    bool text_alpha = false;
    int grid_cols = 0;
    int grid_rows = 0;
    int view_cols = 0, view_rows = 0;
    int list_max_line = 0;
    int middle_num = 0;
    int min_middle_num = 0, max_middle_num = 0;
    int select_combo_idx = 0;
    int top_height = 0, middle_height = 0, down_height = 0;
    int head_height = 0, body_height = 0;
    int sort_type = 0;
    int tab_style = 0;
    int rows_msg_id = 0;
    int max_sprite = 0;
    bool sprite_loop = true;
    int traveling_type = 0;
    int tooltip_line_size = 0;
    bool checked = false;
    uint32_t image_rgba = 0xffffffff;
    std::vector<UiScriptRect> icon_cells;
    std::vector<UiElement> children;
    std::vector<std::string> add_strings;
    std::vector<std::string> init_combo_list;
    std::vector<std::pair<float, float>> spin_minmax;
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
