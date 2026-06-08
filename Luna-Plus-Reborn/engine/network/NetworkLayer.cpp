#include "NetworkLayer.h"
#include <asio.hpp>
#include <asio/steady_timer.hpp>
#include <spdlog/spdlog.h>
#include <cstring>
#include <deque>
#include <chrono>

// CRC32 lookup table
static uint32_t crc32_table[256];
static bool crc32_initialized = false;

static void InitCRC32() {
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
        crc32_table[i] = crc;
    }
    crc32_initialized = true;
}

static uint32_t CalculateCRC32(const uint8_t* data, size_t size) {
    if (!crc32_initialized) InitCRC32();
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < size; i++) {
        crc = crc32_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

// AES-256 CTR wrapper (reuses algorithm from PacketCrypto)
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

static void AesEncryptBlock(const uint8_t in[16], uint8_t out[16], const uint32_t rk[68], int nr) {
    uint8_t state[16];
    std::memcpy(state, in, 16);
    auto add = [&](int r) {
        for (int i = 0; i < 4; i++) {
            uint32_t k = rk[r * 4 + i];
            state[4*i]   ^= (uint8_t)(k >> 24);
            state[4*i+1] ^= (uint8_t)(k >> 16);
            state[4*i+2] ^= (uint8_t)(k >> 8);
            state[4*i+3] ^= (uint8_t)k;
        }
    };
    auto sub = [&]() { for (int i = 0; i < 16; i++) state[i] = SBOX[state[i]]; };
    auto shift = [&]() {
        uint8_t t;
        t=state[4];state[4]=state[5];state[5]=state[6];state[6]=state[7];state[7]=t;
        t=state[8];state[8]=state[10];state[10]=t;
        t=state[9];state[9]=state[11];state[11]=t;
        t=state[15];state[15]=state[14];state[14]=state[13];state[13]=state[12];state[12]=t;
    };
    auto mix = [&]() {
        for (int i = 0; i < 4; i++) {
            int idx = i*4;
            auto g = [](uint8_t a, uint8_t b) -> uint8_t {
                uint8_t p=0; for(int j=0;j<8;j++){if(b&1)p^=a;uint8_t hi=a&0x80;a<<=1;if(hi)a^=0x1b;b>>=1;} return p;
            };
            uint8_t a0=state[idx],a1=state[idx+1],a2=state[idx+2],a3=state[idx+3];
            state[idx]=g(a0,2)^g(a1,3)^a2^a3;
            state[idx+1]=a0^g(a1,2)^g(a2,3)^a3;
            state[idx+2]=a0^a1^g(a2,2)^g(a3,3);
            state[idx+3]=g(a0,3)^a1^a2^g(a3,2);
        }
    };
    add(0);
    for (int r = 1; r < nr; r++) { sub(); shift(); mix(); add(r); }
    sub(); shift(); add(nr);
    std::memcpy(out, state, 16);
}

struct AesCtx {
    int nr;
    uint32_t rk[68];
};

static void KeyExpansion(const uint8_t* key, uint32_t* rk) {
    const int nk = 8, nr = 14;
    static const uint8_t RCON[11] = {0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36};
    int i = 0;
    while (i < nk) {
        rk[i] = ((uint32_t)key[4*i]<<24)|((uint32_t)key[4*i+1]<<16)|((uint32_t)key[4*i+2]<<8)|key[4*i+3];
        i++;
    }
    i = nk;
    while (i < 4 * (nr + 1)) {
        uint32_t temp = rk[i-1];
        if (i % nk == 0) {
            temp = (temp << 8) | (temp >> 24);
            uint8_t a[4] = {(uint8_t)(temp>>24),(uint8_t)(temp>>16),(uint8_t)(temp>>8),(uint8_t)temp};
            for (int j=0;j<4;j++) a[j]=SBOX[a[j]];
            a[0] ^= RCON[i/nk];
            temp = ((uint32_t)a[0]<<24)|((uint32_t)a[1]<<16)|((uint32_t)a[2]<<8)|a[3];
        } else if (nk > 6 && i % nk == 4) {
            uint8_t a[4] = {(uint8_t)(temp>>24),(uint8_t)(temp>>16),(uint8_t)(temp>>8),(uint8_t)temp};
            for (int j=0;j<4;j++) a[j]=SBOX[a[j]];
            temp = ((uint32_t)a[0]<<24)|((uint32_t)a[1]<<16)|((uint32_t)a[2]<<8)|a[3];
        }
        rk[i] = rk[i - nk] ^ temp;
        i++;
    }
}

static AesCtx g_aes_ctx;

static void InitCipher() {
    uint8_t key[32];
    for (size_t i = 0; i < 32; i++) key[i] = 0x4C + (uint8_t)(i * 0x55);
    KeyExpansion(key, g_aes_ctx.rk);
    g_aes_ctx.nr = 14;
}

// GHASH for GCM authentication tag
static void GHASH(uint8_t out[16], const uint8_t* aad, size_t aad_len,
                  const uint8_t* ct, size_t ct_len, const uint8_t h[16]) {
    uint8_t y[16] = {0};
    auto inc = [](uint8_t* v, const uint8_t* x, int n) {
        uint16_t carry = 0;
        for (int i = n-1; i >= 0; i--) {
            uint16_t s = v[i] + x[i] + carry;
            v[i] = s & 0xFF;
            carry = s >> 8;
        }
    };
    auto mul_h = [&](uint8_t* x) {
        uint8_t z[16] = {0}, v[16];
        std::memcpy(v, h, 16);
        for (int i = 0; i < 128; i++) {
            if (x[i/8] & (0x80 >> (i%8)))
                inc(z, v, 16);
            uint8_t lsb = v[15] & 1;
            for (int j = 15; j >= 0; j--) {
                v[j] >>= 1;
                if (j > 0 && (v[j-1] & 1)) v[j] |= 0x80;
            }
            if (lsb) v[0] ^= 0xE1;
        }
        std::memcpy(x, z, 16);
    };

    // Process AAD
    uint8_t block[16];
    size_t pos = 0;
    while (pos < aad_len) {
        std::memset(block, 0, 16);
        size_t copy = std::min((size_t)16, aad_len - pos);
        std::memcpy(block, aad + pos, copy);
        inc(y, block, 16);
        mul_h(y);
        pos += 16;
    }
    // Process ciphertext
    pos = 0;
    while (pos < ct_len) {
        std::memset(block, 0, 16);
        size_t copy = std::min((size_t)16, ct_len - pos);
        std::memcpy(block, ct + pos, copy);
        inc(y, block, 16);
        mul_h(y);
        pos += 16;
    }
    // Length block
    uint64_t aad_bits = (uint64_t)aad_len * 8;
    uint64_t ct_bits = (uint64_t)ct_len * 8;
    for (int i = 0; i < 8; i++) {
        block[i] = (aad_bits >> (56 - i*8)) & 0xFF;
        block[8+i] = (ct_bits >> (56 - i*8)) & 0xFF;
    }
    inc(y, block, 16);
    mul_h(y);
    std::memcpy(out, y, 16);
}

// GCM encrypt: returns ciphertext + 16-byte tag
static std::vector<uint8_t> GcmEncrypt(const uint8_t* plain, size_t len) {
    uint8_t iv[12];
    static uint32_t counter = 0;
    for (int i = 0; i < 8; i++) iv[i] = (counter >> (56 - i*8)) & 0xFF;
    counter++;
    std::memcpy(iv + 8, "\x00\x00\x00\x01", 4);

    uint8_t j0[16], h[16] = {0};
    AesEncryptBlock(h, h, g_aes_ctx.rk, g_aes_ctx.nr);
    std::memcpy(j0, iv, 12);
    j0[12] = j0[13] = j0[14] = 0; j0[15] = 1;

    // CTR mode encryption
    std::vector<uint8_t> ct(len);
    uint8_t ctr[16];
    std::memcpy(ctr, j0, 16);
    for (size_t i = 0; i < len; i += 16) {
        ctr[15]++; // increment counter
        uint8_t ks[16];
        AesEncryptBlock(ctr, ks, g_aes_ctx.rk, g_aes_ctx.nr);
        for (size_t j = 0; j < 16 && i+j < len; j++)
            ct[i+j] = plain[i+j] ^ ks[j];
    }

    // Compute GHASH tag
    uint8_t tag[16];
    GHASH(tag, iv, 12, ct.data(), len, h);

    // XOR with E(K, J0)
    uint8_t ek_j0[16];
    AesEncryptBlock(j0, ek_j0, g_aes_ctx.rk, g_aes_ctx.nr);
    for (int i = 0; i < 16; i++) tag[i] ^= ek_j0[i];

    // Output: IV(8) + ciphertext + tag(16)
    std::vector<uint8_t> result(8 + len + 16);
    std::memcpy(result.data(), iv, 8);
    std::memcpy(result.data() + 8, ct.data(), len);
    std::memcpy(result.data() + 8 + len, tag, 16);
    return result;
}

// GCM decrypt: returns plaintext on success, empty on auth failure
static std::vector<uint8_t> GcmDecrypt(const uint8_t* data, size_t size) {
    if (size < 24) return {}; // 8 IV + 16 tag min
    size_t ct_len = size - 24;

    uint8_t h[16] = {0};
    AesEncryptBlock(h, h, g_aes_ctx.rk, g_aes_ctx.nr);

    uint8_t j0[16];
    std::memcpy(j0, data, 8); // IV
    j0[8] = j0[9] = j0[10] = 0; j0[11] = 1;
    j0[12] = j0[13] = j0[14] = 0; j0[15] = 1;

    // Verify tag
    uint8_t expected_tag[16], computed_tag[16];
    std::memcpy(expected_tag, data + 8 + ct_len, 16);
    GHASH(computed_tag, data, 8, data + 8, ct_len, h);
    uint8_t ek_j0[16];
    AesEncryptBlock(j0, ek_j0, g_aes_ctx.rk, g_aes_ctx.nr);
    for (int i = 0; i < 16; i++) computed_tag[i] ^= ek_j0[i];

    if (std::memcmp(expected_tag, computed_tag, 16) != 0) return {};

    // CTR decryption
    std::vector<uint8_t> plain(ct_len);
    uint8_t ctr[16];
    std::memcpy(ctr, j0, 16);
    for (size_t i = 0; i < ct_len; i += 16) {
        ctr[15]++;
        uint8_t ks[16];
        AesEncryptBlock(ctr, ks, g_aes_ctx.rk, g_aes_ctx.nr);
        for (size_t j = 0; j < 16 && i+j < ct_len; j++)
            plain[i+j] = data[8+i+j] ^ ks[j];
    }
    return plain;
}

static bool g_cipher_initialized = false;

class NetworkLayer::Impl {
public:
    asio::io_context io_context_;
    asio::ip::tcp::socket socket_;
    asio::ip::tcp::acceptor acceptor_{io_context_};
    asio::steady_timer reconnect_timer_{io_context_};
    ReceiveCallback recv_cb_;
    std::vector<uint8_t> read_buf_;
    std::deque<std::vector<uint8_t>> write_queue_;
    bool connected_ = false;
    bool is_server_ = false;
    std::string remote_host_;
    uint16_t remote_port_ = 0;
    uint16_t local_port_ = 0;
    uint32_t seq_send_ = 0;
    uint32_t seq_recv_ = 0;
    std::chrono::steady_clock::time_point connect_time_;
    static constexpr uint32_t MAGIC = 0x4C4E50;
    static constexpr int CONNECT_TIMEOUT_SEC = 30;

    Impl() : socket_(io_context_) {
        read_buf_.reserve(65536);
    }

    bool StartServer(uint16_t port) {
        if (!g_cipher_initialized) { InitCipher(); g_cipher_initialized = true; }
        try {
            asio::ip::tcp::endpoint ep(asio::ip::tcp::v4(), port);
            acceptor_.open(ep.protocol());
            acceptor_.set_option(asio::socket_base::reuse_address(true));
            acceptor_.bind(ep);
            acceptor_.listen(128);
            local_port_ = port;
            is_server_ = true;
            DoAccept();
            spdlog::info("NetworkLayer: server listening on port {}", port);
            return true;
        } catch (std::exception& e) {
            spdlog::error("NetworkLayer: server start failed - {}", e.what());
            return false;
        }
    }

    void DoAccept() {
        acceptor_.async_accept(socket_, [this](std::error_code ec) {
            if (!ec) {
                connected_ = true;
                connect_time_ = std::chrono::steady_clock::now();
                spdlog::info("NetworkLayer: client accepted");
                DoRead();
            } else {
                spdlog::error("NetworkLayer: accept error - {}", ec.message());
            }
            if (is_server_) DoAccept();
        });
    }

    bool StartClient(const std::string& host, uint16_t port) {
        remote_host_ = host;
        remote_port_ = port;
        try {
            asio::ip::tcp::resolver resolver(io_context_);
            auto endpoints = resolver.resolve(host, std::to_string(port));
            asio::async_connect(socket_, endpoints, [this](std::error_code ec, asio::ip::tcp::endpoint) {
                if (!ec) {
                    connected_ = true;
                    connect_time_ = std::chrono::steady_clock::now();
                    spdlog::info("NetworkLayer: connected to {}:{}", remote_host_, remote_port_);
                    DoRead();
                } else {
                    spdlog::error("NetworkLayer: connect failed - {}", ec.message());
                    ScheduleReconnect();
                }
            });
            return true;
        } catch (std::exception& e) {
            spdlog::error("NetworkLayer: connect error - {}", e.what());
            ScheduleReconnect();
            return false;
        }
    }

    void ScheduleReconnect() {
        reconnect_timer_.expires_after(std::chrono::seconds(5));
        reconnect_timer_.async_wait([this](std::error_code ec) {
            if (!ec && !remote_host_.empty()) {
                spdlog::info("NetworkLayer: reconnecting to {}:{}", remote_host_, remote_port_);
                StartClient(remote_host_, remote_port_);
            }
        });
    }

    void DoRead() {
        auto buf = std::make_shared<std::vector<uint8_t>>(4096);
        socket_.async_read_some(asio::buffer(*buf), [this, buf](std::error_code ec, size_t len) {
            if (ec) {
                if (ec != asio::error::eof) {
                    spdlog::error("NetworkLayer: read error - {}", ec.message());
                }
                Close();
                if (!is_server_) ScheduleReconnect();
                return;
            }
            buf->resize(len);
            read_buf_.insert(read_buf_.end(), buf->begin(), buf->end());

            // Process framed packets
            while (read_buf_.size() >= sizeof(PacketHeader)) {
                PacketHeader hdr;
                std::memcpy(&hdr, read_buf_.data(), sizeof(PacketHeader));
                if (hdr.magic != MAGIC) {
                    spdlog::error("NetworkLayer: bad magic 0x{:08X}", hdr.magic);
                    read_buf_.clear();
                    return;
                }
                if (hdr.length > 65535) {
                    spdlog::error("NetworkLayer: oversized packet len={}", hdr.length);
                    read_buf_.clear();
                    return;
                }
                size_t total = sizeof(PacketHeader) + hdr.length;
                if (read_buf_.size() < total) break;

                // Verify CRC32
                uint32_t actual_crc = CalculateCRC32(read_buf_.data() + sizeof(PacketHeader), hdr.length);
                if (actual_crc != hdr.crc32) {
                    spdlog::warn("NetworkLayer: CRC mismatch, dropping packet");
                    read_buf_.erase(read_buf_.begin(), read_buf_.begin() + total);
                    continue;
                }

                // Decrypt payload (AES-256-GCM)
                std::vector<uint8_t> raw(read_buf_.begin() + sizeof(PacketHeader),
                                         read_buf_.begin() + total);
                auto payload = GcmDecrypt(raw.data(), raw.size());
                if (payload.empty()) {
                    spdlog::warn("NetworkLayer: GCM auth tag mismatch, dropping packet");
                    read_buf_.erase(read_buf_.begin(), read_buf_.begin() + total);
                    continue;
                }

                if (recv_cb_) {
                    recv_cb_(payload.data(), payload.size());
                }

                read_buf_.erase(read_buf_.begin(), read_buf_.begin() + total);
            }

            DoRead();
        });
    }

    void DoWrite() {
        if (write_queue_.empty()) return;
        auto& data = write_queue_.front();
        asio::async_write(socket_, asio::buffer(data.data(), data.size()), [this](std::error_code ec, size_t) {
            if (ec) {
                spdlog::error("NetworkLayer: write error - {}", ec.message());
                Close();
                return;
            }
            write_queue_.pop_front();
            if (!write_queue_.empty()) DoWrite();
        });
    }

    void SendData(const uint8_t* data, size_t size) {
        if (!connected_) return;

        // Build packet
        PacketHeader hdr;
        hdr.magic = MAGIC;
        hdr.length = static_cast<uint32_t>(size);
        hdr.type = 0;
        hdr.sequence = ++seq_send_;
        // Encrypt payload with AES-256-GCM (size becomes payload + 24: 8 IV + ciphertext + 16 tag)
        auto encrypted = GcmEncrypt(data, size);
        hdr.crc32 = CalculateCRC32(encrypted.data(), encrypted.size());
        hdr.length = static_cast<uint32_t>(encrypted.size());

        auto packet = std::make_shared<std::vector<uint8_t>>(sizeof(PacketHeader) + encrypted.size());
        std::memcpy(packet->data(), &hdr, sizeof(PacketHeader));
        std::memcpy(packet->data() + sizeof(PacketHeader), encrypted.data(), encrypted.size());

        asio::post(io_context_, [this, packet]() {
            write_queue_.push_back(*packet);
            if (write_queue_.size() == 1) DoWrite();
        });
    }

    void Close() {
        std::error_code ec;
        socket_.close(ec);
        connected_ = false;
        seq_send_ = 0;
        seq_recv_ = 0;
        read_buf_.clear();
        write_queue_.clear();
    }
};

NetworkLayer::NetworkLayer() : impl_(std::make_unique<Impl>()) {}
NetworkLayer::~NetworkLayer() { Shutdown(); }

bool NetworkLayer::Initialize(uint16_t port) {
    return impl_->StartServer(port);
}

void NetworkLayer::Shutdown() {
    impl_->Close();
    impl_->acceptor_.close();
}

bool NetworkLayer::Connect(const std::string& host, uint16_t port) {
    return impl_->StartClient(host, port);
}

void NetworkLayer::Disconnect() {
    impl_->Close();
}

void NetworkLayer::Send(const uint8_t* data, size_t size) {
    impl_->SendData(data, size);
}

void NetworkLayer::SetReceiveCallback(ReceiveCallback cb) {
    impl_->recv_cb_ = std::move(cb);
}

void NetworkLayer::Update() {
    impl_->io_context_.poll();
}

bool NetworkLayer::IsConnected() const {
    return impl_->connected_;
}
