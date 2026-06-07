#pragma once
#include <cstdint>
#include <vector>
#include <string>

// Simple XOR-based packet encryption for basic anti-tamper.
// Production would use AES-256-GCM with key exchange.
// This provides obfuscation and basic integrity checking.

struct PacketHeader {
    uint16_t magic = 0x4C4E; // "LN"
    uint16_t type = 0;
    uint32_t size = 0;
    uint32_t sequence = 0;
    uint32_t crc32 = 0;
    uint32_t padding = 0;
};

#define AES_ENCRYPT     1
#define AES_DECRYPT     0

typedef struct
{
    int nr;
    uint32_t rk[68];
} aes_ctx;

class PacketCrypto {
public:
    static void Init();
    
    // Encrypt/decrypt packet payload
    static std::vector<uint8_t> Encrypt(uint16_t type, const uint8_t* data, size_t size);
    static std::vector<uint8_t> Decrypt(const uint8_t* data, size_t size);
    
    // Verify packet integrity
    static bool VerifyIntegrity(const uint8_t* data, size_t size);
    static uint32_t CalculateCRC(const uint8_t* data, size_t size);
    
    // Sequence number management
    static uint32_t NextSequence() { return ++sequence_; }
    static bool CheckSequence(uint32_t seq);
    static void ResetSequence() { sequence_ = 0; }
    
    // Key management
    static void SetKey(const uint8_t* key, size_t len);
    static void GenerateKey();
    
    static constexpr size_t HEADER_SIZE = sizeof(PacketHeader);
    static constexpr uint16_t MAGIC = 0x4C4E;

private:
    static uint32_t sequence_;
    static aes_ctx encrypt_ctx_;
    static aes_ctx decrypt_ctx_;
    static std::vector<uint8_t> key_;
    
    static void AesEncryptBlock(const uint8_t* in, uint8_t* out);
    static void AesDecryptBlock(const uint8_t* in, uint8_t* out);
    static uint32_t Crc32Table(size_t index);
};
