#include "PacketBuffer.h"
#include <cstring>
#include <algorithm>

PacketBuffer::PacketBuffer() = default;

PacketBuffer::~PacketBuffer() = default;

void PacketBuffer::Write(const char* data, size_t length) {
    buffer_.insert(buffer_.end(), data, data + length);
}

size_t PacketBuffer::Read(char* buffer, size_t length) {
    size_t readable = GetReadableSize();
    size_t to_read = std::min(length, readable);
    if (to_read > 0) {
        memcpy(buffer, buffer_.data() + read_pos_, to_read);
        read_pos_ += to_read;
    }
    if (read_pos_ >= buffer_.size()) {
        Clear();
    }
    return to_read;
}

void PacketBuffer::Clear() {
    buffer_.clear();
    read_pos_ = 0;
}

bool PacketBuffer::CanReadPacket(uint32_t header_size) const {
    return GetReadableSize() >= header_size;
}
