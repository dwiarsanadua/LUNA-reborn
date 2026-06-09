#include "NoticeView.hpp"
#include <rendering/UIRenderer.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <fstream>

NoticeView::NoticeView() = default;
NoticeView::~NoticeView() = default;

std::string NoticeView::StripHtmlTags(const std::string& html) const {
    std::string result;
    bool in_tag = false;
    bool in_entity = false;
    std::string entity_buf;

    for (size_t i = 0; i < html.size(); i++) {
        char c = html[i];

        if (in_entity) {
            if (c == ';') {
                in_entity = false;
                result += ' ';
            } else if (!std::isalpha(static_cast<unsigned char>(c))) {
                in_entity = false;
                result += entity_buf;
                entity_buf.clear();
            } else {
                entity_buf += c;
            }
            continue;
        }

        if (c == '<') {
            in_tag = true;
            continue;
        }
        if (c == '>') {
            in_tag = false;
            continue;
        }
        if (c == '&') {
            in_entity = true;
            entity_buf.clear();
            continue;
        }

        if (!in_tag) {
            if (c == '\r') continue;
            result += c;
        }
    }

    std::string collapsed;
    bool last_was_space = false;
    for (char c : result) {
        if (c == '\n') {
            collapsed += '\n';
            last_was_space = false;
            continue;
        }
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!last_was_space) {
                collapsed += ' ';
                last_was_space = true;
            }
        } else {
            collapsed += c;
            last_was_space = false;
        }
    }
    return collapsed;
}

std::vector<std::string> NoticeView::WrapText(const std::string& text, int max_chars_per_line) const {
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.empty()) {
            lines.emplace_back();
            continue;
        }

        while (line.size() > static_cast<size_t>(max_chars_per_line)) {
            size_t break_pos = line.rfind(' ', max_chars_per_line);
            if (break_pos == std::string::npos || break_pos == 0)
                break_pos = max_chars_per_line;

            lines.push_back(line.substr(0, break_pos));
            line = line.substr(break_pos + 1);
        }
        lines.push_back(line);
    }

    return lines;
}

void NoticeView::LoadFromUrl(const std::string& url, const std::string& fallback_content) {
    pending_url_ = url;
    fetch_pending_ = true;
    content_html_ = fallback_content;
    content_text_ = StripHtmlTags(content_html_);
    rendered_lines_ = WrapText(content_text_, 80);
    loaded_ = true;
    spdlog::info("NoticeView: will fetch URL '{}' (using fallback for now)", url);
}

void NoticeView::LoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        spdlog::warn("NoticeView: cannot open file '{}'", filepath);
        return;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    content_html_ = ss.str();
    content_text_ = StripHtmlTags(content_html_);
    rendered_lines_ = WrapText(content_text_, 80);
    loaded_ = true;
    spdlog::info("NoticeView: loaded content from '{}' ({} chars)", filepath, content_html_.size());
}

void NoticeView::SetContent(const std::string& html_content, const std::string& title) {
    content_html_ = html_content;
    title_ = title;
    content_text_ = StripHtmlTags(content_html_);
    rendered_lines_ = WrapText(content_text_, 80);
    loaded_ = true;
}

void NoticeView::Show() { open_ = true; }

void NoticeView::Hide() {
    open_ = false;
    if (on_close_) on_close_();
}

void NoticeView::Toggle() {
    open_ = !open_;
    if (!open_ && on_close_) on_close_();
}

void NoticeView::SetBounds(float x, float y, float w, float h) {
    x_ = x; y_ = y; w_ = w; h_ = h;
}

void NoticeView::SetOnCloseCallback(std::function<void()> cb) {
    on_close_ = std::move(cb);
}

void NoticeView::Update(float dt) {
    (void)dt;
    if (fetch_pending_) {
        fetch_pending_ = false;
    }
}

void NoticeView::Render(UIRenderer& ui, GameState* state) {
    (void)state;
    if (!open_ || !loaded_) return;

    UIColor bg = {17, 17, 17, 204};
    UIColor border = {68, 68, 68, 255};
    UIColor title_bg = {34, 34, 34, 255};
    UIColor title_color = {255, 255, 255, 255};
    UIColor close_bg = {68, 0, 0, 255};
    UIColor close_color = {255, 68, 68, 255};
    UIColor text_color = {204, 204, 204, 255};
    UIColor scroll_color = {136, 136, 136, 255};
    UIColor title_bar_border = {68, 68, 68, 255};

    float title_h = 28;
    float close_size = 20;
    float padding = 8;
    float line_h = 18;

    ui.DrawRect(x_, y_, w_, h_, bg);
    ui.DrawBorder(x_, y_, w_, h_, border, 1);

    ui.DrawRect(x_, y_, w_, title_h, title_bg);
    ui.DrawBorder(x_, y_, w_, title_h, title_bar_border, 1);
    ui.DrawText(x_ + padding, y_ + 4, 0xFFFFFFFF, "%s", title_.c_str());

    float close_x = x_ + w_ - close_size - 4;
    float close_y = y_ + 4;
    ui.DrawRect(close_x, close_y, close_size, close_size, close_bg);
    ui.DrawText(close_x + 4, close_y + 2, 0xFFFF4444, "X");

    float content_x = x_ + padding;
    float content_y = y_ + title_h + padding;
    float content_w = w_ - padding * 2;
    float content_h = h_ - title_h - padding * 2;

    ui.PushScissor(content_x, content_y, content_w, content_h);

    float text_y = content_y - scroll_offset_;
    for (const auto& line : rendered_lines_) {
        if (text_y + line_h > content_y + content_h) break;
        if (text_y + line_h >= content_y) {
            ui.DrawText(content_x, text_y, 0xFFCCCCCC, "%s", line.c_str());
        }
        text_y += line_h;
    }

    ui.PopScissor();

    float total_text_h = static_cast<float>(rendered_lines_.size()) * line_h;
    if (total_text_h > content_h) {
        float max_scroll = total_text_h - content_h;
        float scroll_ratio = (max_scroll > 0) ? scroll_offset_ / max_scroll : 0;
        float scroll_bar_h = content_h * (content_h / total_text_h);
        float scroll_bar_y = content_y + (content_h - scroll_bar_h) * scroll_ratio;
        ui.DrawRect(x_ + w_ - 6, scroll_bar_y, 4, scroll_bar_h, scroll_color);
    }
}
