#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

struct UIRenderer;
struct GameState;
class WindowManager;

struct NoticeViewEntry {
    std::string title;
    std::string content_html;
    std::string url;
    bool is_loaded = false;
    bool open = false;
    float x = 50, y = 50, w = 500, h = 400;
};

class NoticeView {
public:
    NoticeView();
    ~NoticeView();

    void LoadFromUrl(const std::string& url, const std::string& fallback_content);
    void LoadFromFile(const std::string& filepath);
    void SetContent(const std::string& html_content, const std::string& title = "Notice");
    void Show();
    void Hide();
    void Toggle();
    void Render(UIRenderer& ui, GameState* state);
    void Update(float dt);

    bool IsOpen() const { return open_; }
    const std::string& GetTitle() const { return title_; }
    const std::string& GetContent() const { return content_text_; }

    void SetBounds(float x, float y, float w, float h);
    void SetOnCloseCallback(std::function<void()> cb);

private:
    std::string StripHtmlTags(const std::string& html) const;
    std::vector<std::string> WrapText(const std::string& text, int max_chars_per_line) const;

    bool open_ = false;
    bool loaded_ = false;
    std::string title_;
    std::string content_html_;
    std::string content_text_;
    std::vector<std::string> rendered_lines_;
    float x_ = 50, y_ = 50, w_ = 500, h_ = 400;
    float scroll_offset_ = 0;
    std::function<void()> on_close_;
    bool fetch_pending_ = false;
    std::string pending_url_;
};
