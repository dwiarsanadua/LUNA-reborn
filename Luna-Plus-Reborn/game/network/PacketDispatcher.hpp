#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <spdlog/spdlog.h>

namespace Luna {
    using PacketHandlerFunc = std::function<void(uint32_t conn_id, uint16_t type, const std::vector<uint8_t>& payload)>;

    class PacketDispatcher {
    public:
        void Register(uint16_t type, PacketHandlerFunc handler) {
            handlers_[type] = handler;
        }

        void Dispatch(uint32_t conn_id, uint16_t type, const std::vector<uint8_t>& payload) {
            auto it = handlers_.find(type);
            if (it != handlers_.end()) {
                it->second(conn_id, type, payload);
            } else {
                spdlog::warn("Unhandled packet: type=0x{:04X} ({} bytes)", type, payload.size());
            }
        }

    private:
        std::unordered_map<uint16_t, PacketHandlerFunc> handlers_;
    };
}
