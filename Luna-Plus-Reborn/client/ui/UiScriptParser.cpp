#include "UiScriptParser.hpp"
#include "UiAtlasRegistry.hpp"
#include "UiStringTable.hpp"
#include <engine/gx_render/VFS.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <cstdlib>

std::string UiScriptParser::WidgetTypeName(const std::string& raw_type) {
    if (!raw_type.empty() && raw_type[0] == '$') return raw_type.substr(1);
    return raw_type;
}

UiElement UiScriptParser::ParseFile(const std::string& path) {
    ParseContext ctx;
    ctx.file.open(path);
    if (!ctx.file.is_open()) {
        std::string alt = path;
        if (alt.size() > 4 && alt.substr(alt.size() - 4) == ".txt")
            alt = alt.substr(0, alt.size() - 4);
        ctx.file.open(alt);
    }
    if (!ctx.file.is_open()) {
        spdlog::error("UiScriptParser: Failed to open {}", path);
        return {};
    }
    return ParseStream(ctx);
}

UiElement UiScriptParser::ParseStream(ParseContext& ctx) {
    while (std::getline(ctx.file, ctx.current_line)) {
        std::string line = Trim(ctx.current_line);
        if (line.empty() || line[0] == '@' || line[0] == ';') continue;
        if (line[0] == '$') {
            UiElement root = ParseBlock(ctx, line);
            spdlog::debug("UiScriptParser: parsed '{}' id='{}' at {:.0f},{:.0f} size {:.0f}x{:.0f} ({} children)",
                root.type, root.id, root.rect.x, root.rect.y, root.rect.w, root.rect.h, root.children.size());
            return root;
        }
    }
    return {};
}

UiElement UiScriptParser::ParseBlock(ParseContext& ctx, const std::string& name) {
    UiElement elem;
    elem.type = name;
    
    std::string line;
    bool found_brace = false;
    while (std::getline(ctx.file, line)) {
        line = Trim(line);
        if (line == "{") { found_brace = true; break; }
    }
    if (!found_brace) return elem;

    while (std::getline(ctx.file, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == ';') continue;
        if (!line.empty() && line[0] == '@') continue;
        if (line == "}") break;

        if (line[0] == '$') {
            elem.children.push_back(ParseBlock(ctx, line));
        } else if (line[0] == '#') {
            std::stringstream ss(line);
            std::string cmd; ss >> cmd;
            std::string rest; std::getline(ss, rest);
            rest = Trim(rest);

            if (cmd == "#POINT") {
                UiScriptRect r = ParseRect(rest);
                elem.rect.x = r.x; elem.rect.y = r.y;
                if (r.w != 0) elem.rect.w = r.w;
                if (r.h != 0) elem.rect.h = r.h;
            }
            else if (cmd == "#POSITION") {
                float x, y;
                if (sscanf(rest.c_str(), "%f %f", &x, &y) >= 2) { elem.rect.x = x; elem.rect.y = y; }
            }
            else if (cmd == "#SETWH") {
                float w, h;
                if (sscanf(rest.c_str(), "%f %f", &w, &h) == 2) { elem.rect.w = w; elem.rect.h = h; }
            }
            else if (cmd == "#FUNC") elem.func_name = rest;
            else if (cmd == "#ID") elem.id = rest;
            else if (cmd == "#FONTIDX") elem.font_idx = std::atoi(rest.c_str());
            else if (cmd == "#AUTOCLOSE") elem.auto_close = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#TEXT") {
                elem.text_id = std::atoi(rest.c_str());
                const char* localized = UiStringTable::Get(elem.text_id);
                if (localized) elem.text = localized;
            }
            else if (cmd == "#BTNTEXT") {
                elem.btn_text_id = std::atoi(rest.c_str());
                const char* localized = UiStringTable::Get(elem.btn_text_id);
                if (localized) elem.text = localized;
            }
            else if (cmd == "#BTNTEXTANI") {
                float ax, ay;
                if (sscanf(rest.c_str(), "%f %f", &ax, &ay) >= 1) {
                    // btn text animation offset stored in scale fields temporarily
                    elem.scale_x = ax;
                    elem.scale_y = (ay > 0) ? ay : ax;
                }
            }
            else if (cmd == "#TOOLTIPMSG") elem.tooltip_msg_id = std::atoi(rest.c_str());
            else if (cmd == "#TOOLTIPIMAGE") elem.tooltip_img = ParseImage(rest);
            else if (cmd == "#TOOLTIPCOL") {
                int tr, tg, tb;
                if (sscanf(rest.c_str(), "%d %d %d", &tr, &tg, &tb) >= 3)
                    elem.tooltip_color = (0xff << 24) | (tb << 16) | (tg << 8) | tr;
            }
            else if (cmd == "#TOOLTIPLINESIZE") elem.tooltip_line_size = std::atoi(rest.c_str());
            else if (cmd == "#TEXTXY") {
                float tx, ty;
                if (sscanf(rest.c_str(), "%f %f", &tx, &ty) >= 1) {
                    elem.text_xy.x = tx;
                    elem.text_xy.y = (ty > 0) ? ty : tx;
                }
            }
            else if (cmd == "#PUSHUP") elem.pushup = std::atoi(rest.c_str());
            else if (cmd == "#SHADOW") elem.shadow = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#SHADOWCOLOR") {
                int sr, sg, sb;
                if (sscanf(rest.c_str(), "%d %d %d", &sr, &sg, &sb) >= 3)
                    elem.shadow_color = (0xff << 24) | (sb << 16) | (sg << 8) | sr;
            }
            else if (cmd == "#SHADOWTEXTXY") {
                float sx, sy;
                if (sscanf(rest.c_str(), "%f %f", &sx, &sy) >= 1) {
                    elem.shadow_text_xy.x = sx;
                    elem.shadow_text_xy.y = (sy > 0) ? sy : sx;
                }
            }
            else if (cmd == "#TEXTCOLOR") {
                int tr, tg, tb;
                if (sscanf(rest.c_str(), "%d %d %d", &tr, &tg, &tb) >= 3)
                    elem.text_color = (0xff << 24) | (tb << 16) | (tg << 8) | tr;
            }
            else if (cmd == "#TEXTRECT") {
                float tx, ty, tw, th;
                if (sscanf(rest.c_str(), "%f %f %f %f", &tx, &ty, &tw, &th) >= 4)
                    elem.text_rect = {tx, ty, tw, th};
            }
            else if (cmd == "#TEXTALPHA") elem.text_alpha = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#TEXTOFFSET") {
                float l, r, t;
                if (sscanf(rest.c_str(), "%f %f %f", &l, &r, &t) >= 1) {
                    // stored as text tile offsets
                }
            }
            else if (cmd == "#COMMANDNUM") {
                int c1 = 0, c2 = 0, c3 = 0;
                sscanf(rest.c_str(), "%d %d %d", &c1, &c2, &c3);
                elem.command_num = c1;
            }
            else if (cmd == "#EDITSIZE") {
                if (sscanf(rest.c_str(), "%f %f", &elem.edit_w, &elem.edit_h) < 2)
                    sscanf(rest.c_str(), "%f %f", &elem.edit_w, &elem.edit_h);
            }
            else if (cmd == "#SPINSIZE") {
                float sw, sh;
                if (sscanf(rest.c_str(), "%f %f", &sw, &sh) >= 1) {
                    elem.spin_w = sw; elem.spin_h = (sh > 0) ? sh : sw;
                }
            }
            else if (cmd == "#SECRET") elem.secret = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#CARETCOLOR") {
                int cr, cg, cb;
                if (sscanf(rest.c_str(), "%d %d %d", &cr, &cg, &cb) >= 3)
                    elem.caret_color = (0xff << 24) | (cb << 16) | (cg << 8) | cr;
            }
            else if (cmd == "#LIMITLINENUM") elem.limit_line_num = std::atoi(rest.c_str());
            else if (cmd == "#LIMITBYTES") elem.limit_bytes = std::atoi(rest.c_str());
            else if (cmd == "#READONLY") elem.read_only = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#AUTOSCROLL") elem.auto_scroll = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#PASSIVE") elem.passive = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#GUAGEPIECEIMAGE" || cmd == "#GAUGEPIECEIMAGE")
                elem.gauge_piece_img = ParseImage(rest);
            else if (cmd == "#GUAGEWIDTH" || cmd == "#GAUGEWIDTH")
                elem.gauge_width = (float)std::atof(rest.c_str());
            else if (cmd == "#GUAGEPIECEWIDTH" || cmd == "#GAUGEPIECEWIDTH")
                elem.gauge_piece_width = (float)std::atof(rest.c_str());
            else if (cmd == "#GUAGEEFFECTPIECEIMAGE" || cmd == "#GAUGEEFFECTPIECEIMAGE")
                elem.gauge_effect_piece_img = ParseImage(rest);
            else if (cmd == "#GUAGEEFFECTPIECEWIDTH" || cmd == "#GAUGEEFFECTPIECEWIDTH")
                elem.gauge_effect_piece_width = (float)std::atof(rest.c_str());
            else if (cmd == "#GUAGEPIECEHEIGHTSCALE" || cmd == "#GAUGEPIECEHEIGHTSCALE")
                elem.gauge_piece_height_scale = (float)std::atof(rest.c_str());
            else if (cmd == "#GUAGEEFFECTPIECEHEIGHTSCALE" || cmd == "#GAUGEEFFECTPIECEHEIGHTSCALE")
                elem.gauge_effect_piece_height_scale = (float)std::atof(rest.c_str());
            else if (cmd == "#GUAGEMINVALUE" || cmd == "#GAUGEMINVALUE")
                elem.gauge_min_val = (float)std::atof(rest.c_str());
            else if (cmd == "#GUAGEMAXVALUE" || cmd == "#GAUGEMAXVALUE")
                elem.gauge_max_val = (float)std::atof(rest.c_str());
            else if (cmd == "#GUAGECURVALUE" || cmd == "#GAUGECURVALUE")
                elem.gauge_cur_val = (float)std::atof(rest.c_str());
            else if (cmd == "#GUAGENEVALUE" || cmd == "#GAUGENEVALUE")
                elem.gauge_ne_val = (float)std::atof(rest.c_str());
            else if (cmd == "#GUAGEPOS" || cmd == "#GAUGEPOS") {
                float gx, gy;
                if (sscanf(rest.c_str(), "%f %f", &gx, &gy) >= 2) {
                    elem.gauge_pos.x = gx; elem.gauge_pos.y = gy;
                }
            }
            else if (cmd == "#INITGUAGEBAR" || cmd == "#INITGAUGEBAR") {
                int n;
                if (sscanf(rest.c_str(), "%d", &n) >= 1) {
                    // init gauge bar with N pieces
                }
            }
            else if (cmd == "#OPEN_SOUND") elem.open_sound = std::atoi(rest.c_str());
            else if (cmd == "#CLOSE_SOUND") elem.close_sound = std::atoi(rest.c_str());
            else if (cmd == "#BTNCLICKSOUND") elem.btn_click_sound = std::atoi(rest.c_str());
            else if (cmd == "#SCALE") {
                float sx = 1.0f, sy = 1.0f;
                if (sscanf(rest.c_str(), "%f %f", &sx, &sy) >= 1) {
                    elem.scale_x = sx;
                    elem.scale_y = (sy > 0.0f) ? sy : sx;
                }
            }
            else if (cmd == "#LISTSCALE") {
                float sx = 1.0f, sy = 1.0f;
                if (sscanf(rest.c_str(), "%f %f", &sx, &sy) >= 1) {
                    elem.list_scale_x = sx;
                    elem.list_scale_y = (sy > 0.0f) ? sy : sx;
                }
            }
            else if (cmd == "#BASICIMAGE") elem.basic_img = ParseImage(rest);
            else if (cmd == "#OVERIMAGE") elem.over_img = ParseImage(rest);
            else if (cmd == "#LISTOVERIMAGE") elem.list_over_img = ParseImage(rest);
            else if (cmd == "#SELECTIMAGE") elem.select_img = ParseImage(rest);
            else if (cmd == "#PRESSIMAGE") elem.press_img = ParseImage(rest);
            else if (cmd == "#FOCUSIMAGE") elem.focus_img = ParseImage(rest);
            else if (cmd == "#IMAGESRCRECT") {
                float ix, iy, iw, ih;
                if (sscanf(rest.c_str(), "%f %f %f %f", &ix, &iy, &iw, &ih) >= 4)
                    elem.img_src_rect = {ix, iy, iw, ih};
            }
            else if (cmd == "#TOPIMAGE") elem.top_img = ParseImage(rest);
            else if (cmd == "#TOPHEIGHT") elem.top_height = std::atoi(rest.c_str());
            else if (cmd == "#MIDDELIMAGE") elem.middle_img = ParseImage(rest);
            else if (cmd == "#MIDDLEHEIGHT") elem.middle_height = std::atoi(rest.c_str());
            else if (cmd == "#DOWNIMAGE") elem.down_img = ParseImage(rest);
            else if (cmd == "#DOWNHEIGHT") elem.down_height = std::atoi(rest.c_str());
            else if (cmd == "#HEADIMAGE") elem.head_img = ParseImage(rest);
            else if (cmd == "#HEADHEIGHT") elem.head_height = std::atoi(rest.c_str());
            else if (cmd == "#BODYIMAGE") elem.body_img = ParseImage(rest);
            else if (cmd == "#BODYHEIGHT") elem.body_height = std::atoi(rest.c_str());
            else if (cmd == "#LISTBARIMAGE") elem.list_bar_img = ParseImage(rest);
            else if (cmd == "#LISTBARINTERVAL") elem.list_bar_interval = std::atoi(rest.c_str());
            else if (cmd == "#LISTBARPOINT") {
                float lx, ly, lw, lh;
                if (sscanf(rest.c_str(), "%f %f %f %f", &lx, &ly, &lw, &lh) >= 4)
                    elem.list_bar_point = {lx, ly, lw, lh};
            }
            else if (cmd == "#FGCOLOR") elem.fg_color = ParseColor(rest);
            else if (cmd == "#TEXTALIGN") elem.text_align = std::atoi(rest.c_str());
            else if (cmd == "#MOVEABLE") elem.moveable = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#ACTIVE") elem.active = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#ALPHA") elem.alpha = std::atoi(rest.c_str());
            else if (cmd == "#VALIDXY") {
                float vx, vy;
                if (sscanf(rest.c_str(), "%f %f", &vx, &vy) >= 2) {
                    elem.valid_xy.x = vx; elem.valid_xy.y = vy;
                }
            }
            else if (cmd == "#CAPTIONRECT") {
                float cx, cy, cw, ch;
                if (sscanf(rest.c_str(), "%f %f %f %f", &cx, &cy, &cw, &ch) >= 4) {
                    elem.caption_rect = {cx, cy, cw, ch};
                }
            }
            else if (cmd == "#COLS") elem.grid_cols = std::atoi(rest.c_str());
            else if (cmd == "#ROWS") elem.grid_rows = std::atoi(rest.c_str());
            else if (cmd == "#VIEW_COLS") elem.view_cols = std::atoi(rest.c_str());
            else if (cmd == "#VIEW_ROWS") elem.view_rows = std::atoi(rest.c_str());
            else if (cmd == "#LISTMAXLINE") elem.list_max_line = std::atoi(rest.c_str());
            else if (cmd == "#LINEHEIGHT") elem.line_height = std::atoi(rest.c_str());
            else if (cmd == "#MIDDLENUM") {
                int a = 0, b = 0;
                if (sscanf(rest.c_str(), "%d %d", &a, &b) >= 1)
                    elem.middle_num = a;
            }
            else if (cmd == "#MINMAXMIDDLENUM") {
                int mn, mx;
                if (sscanf(rest.c_str(), "%d %d", &mn, &mx) >= 2) {
                    elem.min_middle_num = mn; elem.max_middle_num = mx;
                }
            }
            else if (cmd == "#SHOWSCROLL") elem.show_scroll = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#DELTAPOS") {
                float dx, dy;
                if (sscanf(rest.c_str(), "%f %f", &dx, &dy) >= 2) {
                    // delta position stored in valid_xy
                    elem.valid_xy.x = dx; elem.valid_xy.y = dy;
                }
            }
            else if (cmd == "#INITGRID") {
                float gx, gy, gw, gh, padx = 1.0f, pady = 1.0f;
                int n = sscanf(rest.c_str(), "%f %f %f %f %f %f", &gx, &gy, &gw, &gh, &padx, &pady);
                if (n >= 4) {
                    elem.grid_cell = {gx, gy, gw, gh};
                    if (n >= 6) { elem.grid_pad_x = padx; elem.grid_pad_y = pady; }
                }
            }
            else if (cmd == "#ICONCELL") {
                UiScriptRect cell = ParseRect(rest);
                if (cell.w > 0) elem.icon_cells.push_back(cell);
            }
            else if (cmd == "#ICONCELLNUM") elem.icon_cells.resize(std::max(0, std::atoi(rest.c_str())));
            else if (cmd == "#ICONCELLBGIMAGE") elem.icon_cell_bg = ParseImage(rest);
            else if (cmd == "#DRAGOVERBGIMAGE") elem.dragover_bg = ParseImage(rest);
            else if (cmd == "#CHECKBOXIMAGE") elem.checkbox_img = ParseImage(rest);
            else if (cmd == "#CHECKIMAGE") elem.check_img = ParseImage(rest);
            else if (cmd == "#SETCHECK") elem.checked = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#COMBOTEXTCOLOR") {
                int cr, cg, cb;
                if (sscanf(rest.c_str(), "%d %d %d", &cr, &cg, &cb) >= 3)
                    elem.combo_text_color = (0xff << 24) | (cb << 16) | (cg << 8) | cr;
            }
            else if (cmd == "#SELECTCOMBOINDEX") elem.select_combo_idx = std::atoi(rest.c_str());
            else if (cmd == "#ADDSTRING") elem.add_strings.push_back(rest);
            else if (cmd == "#INITCOMBOLIST") elem.init_combo_list.push_back(rest);
            else if (cmd == "#INSERTCOLUMN") {
                int col_idx;
                float col_w;
                if (sscanf(rest.c_str(), "%d %f", &col_idx, &col_w) >= 2) {
                    // column insertion stored as spin_minmax pairs
                    elem.spin_minmax.push_back({(float)col_idx, col_w});
                }
            }
            else if (cmd == "#SETITEMTEXT") {
                // set item text - stored in add_strings
                elem.add_strings.push_back(rest);
            }
            else if (cmd == "#SPINMINMAX") {
                int smin, smax;
                if (sscanf(rest.c_str(), "%d %d", &smin, &smax) >= 2) {
                    elem.spin_min = smin; elem.spin_max = smax;
                }
            }
            else if (cmd == "#SPINUNIT") elem.spin_unit = std::atoi(rest.c_str());
            else if (cmd == "#LINEUPWARD") elem.line_upward = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#COLUMNCOLOR") {
                int cr, cg, cb;
                if (sscanf(rest.c_str(), "%d %d %d", &cr, &cg, &cb) >= 3)
                    elem.column_color = (0xff << 24) | (cb << 16) | (cg << 8) | cr;
            }
            else if (cmd == "#SELECTCOLUMCOLOR") {
                int cr, cg, cb;
                if (sscanf(rest.c_str(), "%d %d %d", &cr, &cg, &cb) >= 3)
                    elem.select_column_color = (0xff << 24) | (cb << 16) | (cg << 8) | cr;
            }
            else if (cmd == "#CELLRECT") {
                // cell rect stored as grid_cell
                UiScriptRect cr = ParseRect(rest);
                if (cr.w > 0) elem.grid_cell = cr;
            }
            else if (cmd == "#REMAIN_OVERIMAGE")
                elem.remain_over_image = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#SORT_LIST") elem.sort_type = std::atoi(rest.c_str());
            else if (cmd == "#SAVE_POSITION")
                elem.save_position = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#SAVE_ACTIVITY")
                elem.save_activity = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#USE_TOPROW_CHANGE")
                elem.use_toprow_change = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#TABNUM") elem.tab_num = std::atoi(rest.c_str());
            else if (cmd == "#TAB_STYLE") elem.tab_style = std::atoi(rest.c_str());
            else if (cmd == "#ROWS_MSGID") elem.rows_msg_id = std::atoi(rest.c_str());
            else if (cmd == "#MAXSPRITE") elem.max_sprite = std::atoi(rest.c_str());
            else if (cmd == "#SPRITELOOP") elem.sprite_loop = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#TRAVELINGTYPE") elem.traveling_type = std::atoi(rest.c_str());
            else if (cmd == "#ADDSPRITE") {
                // add sprite frame - stored as icon_cell entries
                UiScriptRect spr = ParseRect(rest);
                if (spr.w > 0) elem.icon_cells.push_back(spr);
            }
            else if (cmd == "#MESSAGE") elem.message = rest;
            else if (cmd == "#COORD") {
                float cx, cy, cz, cw;
                if (sscanf(rest.c_str(), "%f %f %f %f", &cx, &cy, &cz, &cw) >= 4) {
                    // coord stored in valid_xy
                    elem.valid_xy.x = cx; elem.valid_xy.y = cy;
                }
            }
            else if (cmd == "#EXTENDCOLOR") {
                int ecr, ecg, ecb;
                if (sscanf(rest.c_str(), "%d %d %d", &ecr, &ecg, &ecb) >= 3) {
                    if (elem.extend_color_count < 5) {
                        elem.extend_colors[elem.extend_color_count++] =
                            (0xff << 24) | (ecb << 16) | (ecg << 8) | ecr;
                    }
                }
            }
            else if (cmd == "#ITEMTOOLTIP") elem.tooltip_msg_id = std::atoi(rest.c_str());
            else if (cmd == "#BALLOONIMAGETOP") elem.balloon_top = ParseImage(rest);
            else if (cmd == "#BALLOONIMAGEMIDDLE") elem.balloon_middle = ParseImage(rest);
            else if (cmd == "#BALLOONIMAGEBOTTOM") elem.balloon_bottom = ParseImage(rest);
            else if (cmd == "#BALLOONIMAGETAIL") elem.balloon_tail = ParseImage(rest);
            else if (cmd == "#BALLOONIMAGEBORDER") elem.balloon_border = (float)std::atof(rest.c_str());
            else if (cmd == "#COMPONENT_ID") elem.component_id = std::atoi(rest.c_str());
            else if (cmd == "#IMAGE_RGBA") {
                unsigned int rgba;
                if (sscanf(rest.c_str(), "%x", &rgba) >= 1)
                    elem.image_rgba = rgba;
            }
            else if (cmd == "#BTN_MOUSE_EVT_USE_IMG_ALPHA") {
                // stored as text_alpha
                elem.text_alpha = (std::atoi(rest.c_str()) != 0);
            }
            else if (cmd == "#VALIDXY_BTN_ANIIMG" || cmd == "#VALIDXY_BTN_OVER_ANIIMG" ||
                     cmd == "#VALIDXY_BTN_PRESS_ANIIMG") {
                float vx, vy;
                if (sscanf(rest.c_str(), "%f %f", &vx, &vy) >= 2) {
                    elem.valid_xy.x = vx; elem.valid_xy.y = vy;
                }
            }
            else if (cmd == "#SELECTOPTION") {
                // select option stored in spin_min
                elem.spin_min = std::atoi(rest.c_str());
            }
            else {
                spdlog::debug("UiScriptParser: unknown directive '{}'", cmd);
            }
        }
    }

    return elem;
}

UiScriptRect UiScriptParser::ParseRect(const std::string& line) {
    UiScriptRect r{0,0,0,0};
    sscanf(line.c_str(), "%f %f %f %f", &r.x, &r.y, &r.w, &r.h);
    return r;
}

UiScriptUV UiScriptParser::ParseImage(const std::string& line) {
    UiScriptUV uv{-1, 0,0,0,0};
    int a; float x1, y1, x2, y2;
    if (sscanf(line.c_str(), "( %d %f %f %f %f )", &a, &x1, &y1, &x2, &y2) == 5) {
        float aw = UiAtlasRegistry::AtlasWidth(a);
        float ah = UiAtlasRegistry::AtlasHeight(a);
        uv.atlas = a;
        uv.u1 = x1 / aw;
        uv.v1 = y1 / ah;
        uv.u2 = x2 / aw;
        uv.v2 = y2 / ah;
    }
    return uv;
}

uint32_t UiScriptParser::ParseColor(const std::string& line) {
    int r, g, b, a = 255;
    if (sscanf(line.c_str(), "%d %d %d %d", &r, &g, &b, &a) >= 3) {
        return (a << 24) | (b << 16) | (g << 8) | r;
    }
    return 0xffffffff;
}

std::string UiScriptParser::Trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}
