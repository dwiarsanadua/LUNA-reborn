#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <vector>
#include <memory>
#include <string>

namespace Luna {
    inline void InitLog(const std::string& name, const std::string& filename) {
        try {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(spdlog::level::info);
            console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");

            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/" + filename, true);
            file_sink->set_level(spdlog::level::debug);
            file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");

            std::vector<spdlog::sink_ptr> sinks { console_sink, file_sink };
            auto logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
            
            logger->set_level(spdlog::level::debug);
            spdlog::set_default_logger(logger);
            spdlog::flush_on(spdlog::level::info);
            
            spdlog::info("Logging initialized: {} -> logs/{}", name, filename);
        } catch (const spdlog::spdlog_ex& ex) {
            printf("Log initialization failed: %s\n", ex.what());
        }
    }
}
