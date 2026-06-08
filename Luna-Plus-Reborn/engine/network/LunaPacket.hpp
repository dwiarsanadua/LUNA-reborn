#pragma once
#include <cstddef>
#include <cstdint>

#pragma pack(push, 1)
struct LunaPacketHeader {
    uint32_t magic = 0x4C4E50; // "LNP"
    uint16_t length = 0;
    uint16_t type = 0;
    uint16_t sequence = 0;
    uint32_t checksum = 0;
};
#pragma pack(pop)

static constexpr uint32_t LUNA_PACKET_MAGIC = 0x4C4E50;
static constexpr size_t LUNA_PACKET_HEADER_SIZE = sizeof(LunaPacketHeader);
