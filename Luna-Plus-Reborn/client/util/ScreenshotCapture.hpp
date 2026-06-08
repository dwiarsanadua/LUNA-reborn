#pragma once
#include <string>
#include <functional>

class ScreenshotCapture {
public:
    static bool Request(const std::string& path);
    static void SetCompletionHandler(std::function<void(const std::string&, bool)> cb);
    static void Poll();
};
