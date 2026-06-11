#pragma once
#include <vector>
#include <cstdint>
#include <cstring>

class PacketBuffer {
public:
    PacketBuffer();
    ~PacketBuffer();

    void Write(const char* data, size_t length);
    size_t Read(char* buffer, size_t length);
    void Clear();

    const char* GetData() const { return buffer_.data(); }
    size_t GetSize() const { return buffer_.size(); }
    size_t GetReadableSize() const { return buffer_.size() - read_pos_; }

    bool CanReadPacket(uint32_t header_size) const;

    template<typename T>
    void WriteValue(const T& value) {
        Write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    template<typename T>
    T ReadValue() {
        T value;
        if (Read(reinterpret_cast<char*>(&value), sizeof(T)) == sizeof(T))
            return value;
        return T{};
    }

private:
    std::vector<char> buffer_;
    size_t read_pos_ = 0;
};
