#include "UiScriptParser.hpp"
#include <spdlog/spdlog.h>
#include <iostream>

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
            spdlog::info("UiScriptParser: parsed '{}' id='{}' at {:.0f},{:.0f} size {:.0f}x{:.0f} ({} children)",
                root.type, root.id, root.rect.x, root.rect.y, root.rect.w, root.rect.h, root.children.size());
            return root;
        }
    }
    return {};
}

UiElement UiScriptParser::ParseBlock(ParseContext& ctx, const std::string& name) {
    UiElement elem;
    elem.type = name;
    
    // Find open brace
    std::string line;
    bool found_brace = false;
    while (std::getline(ctx.file, line)) {
        line = Trim(line);
        if (line == "{") { found_brace = true; break; }
    }
    if (!found_brace) return elem;

    while (std::getline(ctx.file, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '@' || line[0] == ';') continue;
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
                // Format: x y [over_x over_y] [down_x down_y]  — take first 2 values as position
                float x, y; char comma;
                if (sscanf(rest.c_str(), "%f %f", &x, &y) >= 2) { elem.rect.x = x; elem.rect.y = y; }
            }
            else if (cmd == "#SETWH") {
                float w, h;
                if (sscanf(rest.c_str(), "%f %f", &w, &h) == 2) { elem.rect.w = w; elem.rect.h = h; }
            }
            else if (cmd == "#FUNC") elem.func_name = rest;
            else if (cmd == "#ID") elem.id = rest;
            else if (cmd == "#BASICIMAGE") elem.basic_img = ParseImage(rest);
            else if (cmd == "#OVERIMAGE") elem.over_img = ParseImage(rest);
            else if (cmd == "#PRESSIMAGE") elem.press_img = ParseImage(rest);
            else if (cmd == "#FGCOLOR") elem.fg_color = ParseColor(rest);
            else if (cmd == "#TEXTALIGN") elem.text_align = std::atoi(rest.c_str());
            else if (cmd == "#MOVEABLE") elem.moveable = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#ACTIVE") elem.active = (std::atoi(rest.c_str()) != 0);
            else if (cmd == "#ALPHA") elem.alpha = std::atoi(rest.c_str());
            else if (cmd == "#CAPTIONRECT") {
                float cx, cy, cw, ch;
                if (sscanf(rest.c_str(), "%f %f %f %f", &cx, &cy, &cw, &ch) >= 4) {
                    elem.caption_rect = {cx, cy, cw, ch};
                }
            }
            else if (cmd == "#COLS") elem.grid_cols = std::atoi(rest.c_str());
            else if (cmd == "#ROWS") elem.grid_rows = std::atoi(rest.c_str());
            else if (cmd == "#INITGRID") {
                float gx, gy, gw, gh;
                if (sscanf(rest.c_str(), "%f %f %f %f", &gx, &gy, &gw, &gh) >= 4) {
                    elem.grid_cell = {gx, gy, gw, gh};
                }
            }
            else if (cmd == "#ICONCELL") {
                // Equipment slot: x y w h
                UiScriptRect cell = ParseRect(rest);
                if (cell.w > 0) elem.icon_cells.push_back(cell);
            }
            else if (cmd == "#ICONCELLBGIMAGE") elem.icon_cell_bg = ParseImage(rest);
            else if (cmd == "#DRAGOVERBGIMAGE") elem.dragover_bg = ParseImage(rest);
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
    // Format: ( atlas_id  x1 y1 x2 y2 )
    int a; float x1, y1, x2, y2;
    if (sscanf(line.c_str(), "( %d %f %f %f %f )", &a, &x1, &y1, &x2, &y2) == 5) {
        uv.atlas = a;
        uv.u1 = x1 / 1024.0f;
        uv.v1 = y1 / 1024.0f;
        uv.u2 = x2 / 1024.0f;
        uv.v2 = y2 / 1024.0f;
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
