#include "PacketCrypto.hpp"
#include <cstring>
#include <algorithm>
#include <spdlog/spdlog.h>

uint32_t PacketCrypto::sequence_ = 0;
std::vector<uint8_t> PacketCrypto::key_;
aes_ctx PacketCrypto::encrypt_ctx_;
aes_ctx PacketCrypto::decrypt_ctx_;

// AES-256 S-box
static const uint8_t SBOX[256] = {
    0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
    0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
    0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
    0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
    0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
    0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
    0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
    0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
    0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
    0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
    0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
    0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
    0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
    0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
    0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
    0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

static const uint8_t RCON[11] = {0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36};

static uint8_t GMul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        uint8_t hi = a & 0x80;
        a <<= 1;
        if (hi) a ^= 0x1b;
        b >>= 1;
    }
    return p;
}

static void KeyExpansion(const uint8_t* key, uint32_t* rk, int nk, int nr) {
    int i = 0;
    while (i < nk) {
        rk[i] = ((uint32_t)key[4*i] << 24) | ((uint32_t)key[4*i+1] << 16) |
                ((uint32_t)key[4*i+2] << 8) | (uint32_t)key[4*i+3];
        i++;
    }
    i = nk;
    while (i < 4 * (nr + 1)) {
        uint32_t temp = rk[i-1];
        if (i % nk == 0) {
            temp = (temp << 8) | (temp >> 24);
            uint8_t a[4] = {(uint8_t)(temp >> 24), (uint8_t)(temp >> 16),
                            (uint8_t)(temp >> 8), (uint8_t)temp};
            for (int j = 0; j < 4; j++) a[j] = SBOX[a[j]];
            a[0] ^= RCON[i/nk];
            temp = ((uint32_t)a[0] << 24) | ((uint32_t)a[1] << 16) |
                   ((uint32_t)a[2] << 8) | a[3];
        } else if (nk > 6 && i % nk == 4) {
            uint8_t a[4] = {(uint8_t)(temp >> 24), (uint8_t)(temp >> 16),
                            (uint8_t)(temp >> 8), (uint8_t)temp};
            for (int j = 0; j < 4; j++) a[j] = SBOX[a[j]];
            temp = ((uint32_t)a[0] << 24) | ((uint32_t)a[1] << 16) |
                   ((uint32_t)a[2] << 8) | a[3];
        }
        rk[i] = rk[i - nk] ^ temp;
        i++;
    }
}

static void AddRoundKey(uint8_t state[16], const uint32_t* rk, int round) {
    for (int i = 0; i < 4; i++) {
        uint32_t k = rk[round * 4 + i];
        state[4*i]   ^= (uint8_t)(k >> 24);
        state[4*i+1] ^= (uint8_t)(k >> 16);
        state[4*i+2] ^= (uint8_t)(k >> 8);
        state[4*i+3] ^= (uint8_t)k;
    }
}

static void SubBytes(uint8_t state[16]) {
    for (int i = 0; i < 16; i++) state[i] = SBOX[state[i]];
}

static void InvSubBytes(uint8_t state[16]) {
    static const uint8_t ISBOX[256] = {
        0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
        0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
        0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
        0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
        0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
        0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
        0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
        0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
        0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
        0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
        0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
        0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
        0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
        0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
        0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
        0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
    };
    for (int i = 0; i < 16; i++) state[i] = ISBOX[state[i]];
}

static void ShiftRows(uint8_t state[16]) {
    uint8_t t;
    t = state[4]; state[4] = state[5]; state[5] = state[6]; state[6] = state[7]; state[7] = t;
    t = state[8]; state[8] = state[10]; state[10] = t;
    t = state[9]; state[9] = state[11]; state[11] = t;
    t = state[15]; state[15] = state[14]; state[14] = state[13]; state[13] = state[12]; state[12] = t;
}

static void InvShiftRows(uint8_t state[16]) {
    uint8_t t;
    t = state[7]; state[7] = state[6]; state[6] = state[5]; state[5] = state[4]; state[4] = t;
    t = state[8]; state[8] = state[10]; state[10] = t;
    t = state[9]; state[9] = state[11]; state[11] = t;
    t = state[12]; state[12] = state[13]; state[13] = state[14]; state[14] = state[15]; state[15] = t;
}

static void MixColumns(uint8_t state[16]) {
    for (int i = 0; i < 4; i++) {
        int idx = i * 4;
        uint8_t a0 = state[idx], a1 = state[idx+1], a2 = state[idx+2], a3 = state[idx+3];
        state[idx]   = GMul(a0,2) ^ GMul(a1,3) ^ a2 ^ a3;
        state[idx+1] = a0 ^ GMul(a1,2) ^ GMul(a2,3) ^ a3;
        state[idx+2] = a0 ^ a1 ^ GMul(a2,2) ^ GMul(a3,3);
        state[idx+3] = GMul(a0,3) ^ a1 ^ a2 ^ GMul(a3,2);
    }
}

static void InvMixColumns(uint8_t state[16]) {
    for (int i = 0; i < 4; i++) {
        int idx = i * 4;
        uint8_t a0 = state[idx], a1 = state[idx+1], a2 = state[idx+2], a3 = state[idx+3];
        state[idx]   = GMul(a0,14) ^ GMul(a1,11) ^ GMul(a2,13) ^ GMul(a3,9);
        state[idx+1] = GMul(a0,9) ^ GMul(a1,14) ^ GMul(a2,11) ^ GMul(a3,13);
        state[idx+2] = GMul(a0,13) ^ GMul(a1,9) ^ GMul(a2,14) ^ GMul(a3,11);
        state[idx+3] = GMul(a0,11) ^ GMul(a1,13) ^ GMul(a2,9) ^ GMul(a3,14);
    }
}

void PacketCrypto::Init() {
    sequence_ = 0;
    key_.resize(32);
    for (size_t i = 0; i < 32; i++) key_[i] = 0x4C + (uint8_t)(i * 0x55); // Legacy key derivation
    
    int nk = 8, nr = 14; // AES-256
    KeyExpansion(key_.data(), encrypt_ctx_.rk, nk, nr);
    encrypt_ctx_.nr = nr;
    std::memcpy(&decrypt_ctx_, &encrypt_ctx_, sizeof(aes_ctx));
    
    spdlog::info("PacketCrypto: AES-256 initialized with legacy key schedule");
}

void PacketCrypto::GenerateKey() {
    // Key is generated in Init()
}

void PacketCrypto::SetKey(const uint8_t* key, size_t len) {
    key_.assign(key, key + std::min(len, (size_t)32));
    int nk = 8, nr = 14;
    KeyExpansion(key_.data(), encrypt_ctx_.rk, nk, nr);
    encrypt_ctx_.nr = nr;
    std::memcpy(&decrypt_ctx_, &encrypt_ctx_, sizeof(aes_ctx));
}

void PacketCrypto::AesEncryptBlock(const uint8_t* in, uint8_t* out) {
    uint8_t state[16];
    std::memcpy(state, in, 16);
    
    AddRoundKey(state, encrypt_ctx_.rk, 0);
    for (int r = 1; r < encrypt_ctx_.nr; r++) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(state, encrypt_ctx_.rk, r);
    }
    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state, encrypt_ctx_.rk, encrypt_ctx_.nr);
    
    std::memcpy(out, state, 16);
}

void PacketCrypto::AesDecryptBlock(const uint8_t* in, uint8_t* out) {
    uint8_t state[16];
    std::memcpy(state, in, 16);
    
    AddRoundKey(state, decrypt_ctx_.rk, decrypt_ctx_.nr);
    for (int r = decrypt_ctx_.nr - 1; r >= 1; r--) {
        InvShiftRows(state);
        InvSubBytes(state);
        AddRoundKey(state, decrypt_ctx_.rk, r);
        InvMixColumns(state);
    }
    InvShiftRows(state);
    InvSubBytes(state);
    AddRoundKey(state, decrypt_ctx_.rk, 0);
    
    std::memcpy(out, state, 16);
}

std::vector<uint8_t> PacketCrypto::Encrypt(uint16_t type, const uint8_t* data, size_t size) {
    PacketHeader header;
    header.type = type;
    header.size = (uint32_t)size;
    header.sequence = ++sequence_;
    
    size_t padded = ((size + 15) & ~15) + 16; // +16 for IV
    std::vector<uint8_t> result(sizeof(PacketHeader) + padded);
    std::memcpy(result.data(), &header, sizeof(PacketHeader));
    std::memset(result.data() + sizeof(PacketHeader), 0, padded);
    
    if (size > 0) {
        std::memcpy(result.data() + sizeof(PacketHeader), data, size);
        for (size_t i = 0; i < padded; i += 16) {
            AesEncryptBlock(result.data() + sizeof(PacketHeader) + i,
                            result.data() + sizeof(PacketHeader) + i);
        }
    }
    
    header.crc32 = CalculateCRC(result.data() + 4, result.size() - 4);
    std::memcpy(result.data(), &header, sizeof(PacketHeader));
    return result;
}

std::vector<uint8_t> PacketCrypto::Decrypt(const uint8_t* data, size_t size) {
    if (size < sizeof(PacketHeader)) return {};
    
    PacketHeader header;
    std::memcpy(&header, data, sizeof(PacketHeader));
    
    if (header.magic != MAGIC) return {};
    
    uint32_t expected_crc = header.crc32;
    header.crc32 = 0;
    
    size_t payload_size = size - sizeof(PacketHeader);
    if (payload_size % 16 != 0) return {};
    
    std::vector<uint8_t> result(payload_size);
    if (payload_size > 0) {
        for (size_t i = 0; i < payload_size; i += 16) {
            AesDecryptBlock(data + sizeof(PacketHeader) + i, result.data() + i);
        }
    }
    
    return result;
}

uint32_t PacketCrypto::CalculateCRC(const uint8_t* data, size_t size) {
    uint32_t crc = 0;
    for (size_t i = 0; i < size; i++) {
        crc += data[i];
        crc += (crc << 10);
        crc ^= (crc >> 6);
    }
    crc += (crc << 3);
    crc ^= (crc >> 11);
    crc += (crc << 15);
    return crc;
}

bool PacketCrypto::CheckSequence(uint32_t seq) {
    if (seq <= sequence_) return false;
    sequence_ = seq;
    return true;
}
