#include "PatchSystem.hpp"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <dirent.h>
    typedef int SOCKET;
    const int INVALID_SOCKET = -1;
    const int SOCKET_ERROR = -1;
    static int closesocket(int fd) { return close(fd); }
#endif

static bool SocketInit() {
#ifdef _WIN32
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
#else
    return true;
#endif
}

static void SocketCleanup() {
#ifdef _WIN32
    WSACleanup();
#endif
}

static SOCKET TcpConnect(const std::string& host, int port) {
    struct addrinfo hints{}, *res = nullptr;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    int err = getaddrinfo(host.c_str(), port_str, &hints, &res);
    if (err != 0 || !res) return INVALID_SOCKET;
    SOCKET fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd == INVALID_SOCKET) { freeaddrinfo(res); return INVALID_SOCKET; }
    struct timeval tv{10, 0};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));
    if (connect(fd, res->ai_addr, (int)res->ai_addrlen) == SOCKET_ERROR) {
        closesocket(fd);
        freeaddrinfo(res);
        return INVALID_SOCKET;
    }
    freeaddrinfo(res);
    return fd;
}

static bool HttpSendRequest(SOCKET fd, const std::string& host, const std::string& path) {
    std::string req = "GET " + path + " HTTP/1.1\r\n"
                      "Host: " + host + "\r\n"
                      "User-Agent: LunaPlusReborn/1.0\r\n"
                      "Connection: close\r\n"
                      "\r\n";
    return send(fd, req.data(), (int)req.size(), 0) != SOCKET_ERROR;
}

static int HttpReadStatus(SOCKET fd) {
    char buf[256];
    int pos = 0;
    while (pos < (int)sizeof(buf) - 1) {
        int n = (int)recv(fd, buf + pos, 1, 0);
        if (n <= 0) return -1;
        pos += n;
        if (pos >= 2 && buf[pos-2] == '\r' && buf[pos-1] == '\n') break;
    }
    buf[pos] = 0;
    int http_ver = 0, status = 0;
    sscanf(buf, "HTTP/%d.%d %d", &http_ver, &http_ver, &status);
    return status;
}

static int64_t HttpReadContentLength(SOCKET fd) {
    char line[256];
    while (true) {
        int pos = 0;
        while (pos < (int)sizeof(line) - 1) {
            int n = (int)recv(fd, line + pos, 1, 0);
            if (n <= 0) return -1;
            pos += n;
            if (pos >= 2 && line[pos-2] == '\r' && line[pos-1] == '\n') break;
        }
        line[pos] = 0;
        if (pos == 2 && line[0] == '\r') break;
        std::string l(line);
        auto cl = l.find("Content-Length:");
        if (cl != std::string::npos) {
            return std::atoll(l.c_str() + cl + 15);
        }
    }
    return -1;
}

static bool HttpReadBody(SOCKET fd, std::vector<uint8_t>& body, int64_t content_len,
                          std::function<void(float)> progress_cb) {
    body.clear();
    int64_t total = 0;
    char buf[65536];
    if (content_len > 0) body.reserve((size_t)content_len);
    while (true) {
        int n = (int)recv(fd, buf, sizeof(buf), 0);
        if (n < 0) return false;
        if (n == 0) break;
        body.insert(body.end(), buf, buf + n);
        total += n;
        if (progress_cb && content_len > 0)
            progress_cb((float)total / (float)content_len);
    }
    return true;
}

// ── Parse URL components ────────────────────────────────────────────────────

static bool ParseUrl(const std::string& url, std::string& host, int& port, std::string& path) {
    std::string s = url;
    port = 80;
    size_t p = s.find("://");
    if (p != std::string::npos) s = s.substr(p + 3);
    p = s.find('/');
    if (p == std::string::npos) {
        host = s;
        path = "/";
    } else {
        host = s.substr(0, p);
        path = s.substr(p);
    }
    p = host.find(':');
    if (p != std::string::npos) {
        port = std::atoi(host.c_str() + p + 1);
        host = host.substr(0, p);
    }
    return true;
}

// ── SHA-256 ─────────────────────────────────────────────────────────────────

#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define SIG0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ ((x) >> 3))
#define SIG1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ ((x) >> 10))
#define EP0(x)  (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define EP1(x)  (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

static const uint32_t K256[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

struct Sha256Ctx {
    uint32_t state[8];
    uint64_t count;
    uint8_t buffer[64];
};

static void sha256_transform(Sha256Ctx* ctx, const uint8_t* data) {
    uint32_t W[64], a, b, c, d, e, f, g, h, t1, t2;
    for (int i = 0; i < 16; i++)
        W[i] = ((uint32_t)data[4*i]) << 24 | ((uint32_t)data[4*i+1]) << 16 |
               ((uint32_t)data[4*i+2]) << 8 | data[4*i+3];
    for (int i = 16; i < 64; i++)
        W[i] = SIG1(W[i-2]) + W[i-7] + SIG0(W[i-15]) + W[i-16];
    a = ctx->state[0]; b = ctx->state[1]; c = ctx->state[2]; d = ctx->state[3];
    e = ctx->state[4]; f = ctx->state[5]; g = ctx->state[6]; h = ctx->state[7];
    for (int i = 0; i < 64; i++) {
        t1 = h + EP1(e) + CH(e,f,g) + K256[i] + W[i];
        t2 = EP0(a) + MAJ(a,b,c);
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }
    ctx->state[0] += a; ctx->state[1] += b; ctx->state[2] += c; ctx->state[3] += d;
    ctx->state[4] += e; ctx->state[5] += f; ctx->state[6] += g; ctx->state[7] += h;
}

static void sha256_init(Sha256Ctx* ctx) {
    ctx->count = 0;
    ctx->state[0] = 0x6a09e667; ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372; ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f; ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab; ctx->state[7] = 0x5be0cd19;
}

static void sha256_update(Sha256Ctx* ctx, const uint8_t* data, size_t len) {
    size_t idx = (size_t)(ctx->count & 0x3f);
    ctx->count += (uint64_t)len;
    size_t part = 64 - idx;
    if (len >= part) {
        std::memcpy(ctx->buffer + idx, data, part);
        sha256_transform(ctx, ctx->buffer);
        for (size_t i = part; i + 63 < len; i += 64)
            sha256_transform(ctx, data + i);
        idx = 0;
    } else {
        part = len;
    }
    std::memcpy(ctx->buffer + idx, data + len - part, part);
}

static void sha256_final(Sha256Ctx* ctx, uint8_t* digest) {
    uint64_t bits = ctx->count << 3;
    size_t idx = (size_t)(ctx->count & 0x3f);
    size_t pad = (idx < 56) ? (56 - idx) : (120 - idx);
    static const uint8_t padding[64] = {0x80};
    sha256_update(ctx, padding, pad);
    uint8_t len_buf[8];
    for (int i = 0; i < 8; i++)
        len_buf[i] = (uint8_t)(bits >> (56 - i * 8));
    sha256_update(ctx, len_buf, 8);
    for (int i = 0; i < 8; i++) {
        digest[4*i]   = (uint8_t)(ctx->state[i] >> 24);
        digest[4*i+1] = (uint8_t)(ctx->state[i] >> 16);
        digest[4*i+2] = (uint8_t)(ctx->state[i] >> 8);
        digest[4*i+3] = (uint8_t)(ctx->state[i]);
    }
}

static std::string sha256_hex(const uint8_t* digest) {
    const char hex[] = "0123456789abcdef";
    std::string out(64, 0);
    for (int i = 0; i < 32; i++) {
        out[2*i]   = hex[digest[i] >> 4];
        out[2*i+1] = hex[digest[i] & 0xf];
    }
    return out;
}

// ── PatchSystem implementation ──────────────────────────────────────────────

void PatchSystem::Init() {
    current_ = {1, 1, 0};
    latest_ = {1, 1, 0};
    update_available_ = false;
    patch_notes_ = "v1.1.0 — Luna Plus Reborn";
    patch_server_url_ = "";
    spdlog::info("PatchSystem: v{} (latest: v{})",
                 current_.ToString(), latest_.ToString());
}

void PatchSystem::SetCurrentVersion(int major, int minor, int patch) {
    current_ = {major, minor, patch};
    update_available_ = current_ < latest_;
}

bool PatchSystem::CheckForUpdate(const VersionInfo& latest) {
    latest_ = latest;
    update_available_ = current_ < latest_;
    return update_available_;
}

bool PatchSystem::FetchManifest() {
    if (patch_server_url_.empty()) {
        spdlog::warn("PatchSystem: no patch server URL configured");
        return false;
    }
    std::string manifest_url = patch_server_url_;
    if (manifest_url.back() != '/') manifest_url += '/';
    manifest_url += "manifest.json";

    std::vector<uint8_t> response;
    if (!HttpGet(manifest_url, response, 1048576)) {
        last_error_ = "Failed to fetch manifest";
        spdlog::error("PatchSystem: {}", last_error_);
        if (update_cb_) update_cb_("Failed to fetch manifest", 0);
        return false;
    }

    std::string json((const char*)response.data(), response.size());
    try {
        manifest_.files.clear();
        size_t ver_pos = json.find("\"version\"");
        if (ver_pos != std::string::npos) {
            auto q1 = json.find('"', ver_pos + 9);
            auto q2 = json.find('"', q1 + 1);
            if (q1 != std::string::npos && q2 != std::string::npos) {
                manifest_.version = json.substr(q1 + 1, q2 - q1 - 1);
                int mj = 0, mn = 0, pt = 0;
                sscanf(manifest_.version.c_str(), "%d.%d.%d", &mj, &mn, &pt);
                latest_ = {mj, mn, pt};
                update_available_ = current_ < latest_;
            }
        }

        // Parse files array
        auto files_pos = json.find("\"files\"");
        if (files_pos != std::string::npos) {
            auto arr_start = json.find('[', files_pos);
            auto arr_end = json.find(']', arr_start);
            if (arr_start != std::string::npos && arr_end != std::string::npos) {
                std::string arr = json.substr(arr_start + 1, arr_end - arr_start - 1);
                size_t pos = 0;
                while (true) {
                    auto ob = arr.find('{', pos);
                    if (ob == std::string::npos) break;
                    auto cb = arr.find('}', ob);
                    if (cb == std::string::npos) break;
                    std::string entry = arr.substr(ob + 1, cb - ob - 1);
                    PatchFileEntry pfe;
                    auto extract = [&](const std::string& key) -> std::string {
                        auto kp = entry.find("\"" + key + "\"");
                        if (kp == std::string::npos) return "";
                        auto v1 = entry.find('"', kp + key.size() + 2);
                        if (v1 == std::string::npos) return "";
                        auto v2 = entry.find('"', v1 + 1);
                        if (v2 == std::string::npos) return "";
                        return entry.substr(v1 + 1, v2 - v1 - 1);
                    };
                    pfe.path = extract("path");
                    pfe.sha256 = extract("sha256");
                    std::string sz = extract("size");
                    if (!sz.empty()) pfe.size = std::atoll(sz.c_str());
                    if (pfe.path.empty()) { pos = cb + 1; continue; }
                    pfe.url = patch_server_url_;
                    if (pfe.url.back() != '/') pfe.url += '/';
                    pfe.url += pfe.path;
                    manifest_.files.push_back(pfe);
                    pos = cb + 1;
                }
            }
        }
    } catch (...) {
        last_error_ = "Failed to parse manifest";
        spdlog::error("PatchSystem: {}", last_error_);
        return false;
    }

    update_available_ = current_ < latest_;
    spdlog::info("PatchSystem: manifest v{} ({} files, update: {})",
                 manifest_.version, manifest_.files.size(), update_available_);
    if (update_cb_) update_cb_("Manifest v" + manifest_.version, 0);
    return true;
}

void PatchSystem::StartDownload() {
    if (manifest_.files.empty()) {
        spdlog::warn("PatchSystem: no files to download");
        return;
    }
    downloading_ = true;
    cancelled_ = false;
    download_progress_ = 0;
    downloaded_bytes_ = 0;
    total_bytes_ = 0;
    current_file_.clear();
    pending_files_.clear();

    for (const auto& f : manifest_.files) {
        std::string local_path = f.path;
        if (!SHA256Verify(local_path, f.sha256)) {
            pending_files_.push_back(f);
            total_bytes_ += f.size;
        }
    }
    total_files_ = static_cast<int>(pending_files_.size());
    download_speed_bytes_ = 0;

    spdlog::info("PatchSystem: downloading {} files ({} bytes)...",
                 total_files_, total_bytes_);
    if (update_cb_) update_cb_("Downloading " + std::to_string(total_files_) + " files...", 0);
}

void PatchSystem::UpdateDownload(float dt) {
    if (!downloading_ || cancelled_) return;
    if (pending_files_.empty()) {
        downloading_ = false;
        ApplyUpdate();
        return;
    }

    auto& entry = pending_files_.front();
    current_file_ = entry.path;
    std::string dest = entry.path;

    // Create parent directories
    size_t slash = dest.rfind('/');
    if (slash != std::string::npos) {
        std::string dir = dest.substr(0, slash);
        struct stat st{};
        if (stat(dir.c_str(), &st) != 0) {
#ifdef _WIN32
            _mkdir(dir.c_str());
#else
            mkdir(dir.c_str(), 0755);
#endif
        }
    }

    int64_t prev_downloaded = downloaded_bytes_;
    auto file_progress = [&](float pct) {
        if (file_cb_) file_cb_(current_file_, pct);
    };

    bool ok = HttpDownloadFile(entry.url, dest, entry.size, file_progress);
    if (!ok) {
        last_error_ = "Download failed: " + entry.path;
        spdlog::error("PatchSystem: {}", last_error_);
        if (update_cb_) update_cb_(last_error_, download_progress_);
        downloading_ = false;
        return;
    }

    if (!entry.sha256.empty() && !SHA256Verify(dest, entry.sha256)) {
        last_error_ = "SHA-256 mismatch: " + entry.path;
        spdlog::error("PatchSystem: {}", last_error_);
        std::remove(dest.c_str());
        if (update_cb_) update_cb_(last_error_, download_progress_);
        downloading_ = false;
        return;
    }

    downloaded_bytes_ += entry.size;
    pending_files_.erase(pending_files_.begin());

    int done = total_files_ - static_cast<int>(pending_files_.size());
    download_progress_ = total_bytes_ > 0 ? (float)downloaded_bytes_ / (float)total_bytes_ : 0;
    download_speed_bytes_ = dt > 0 ? (float)(downloaded_bytes_ - prev_downloaded) / dt : 0;

    if (update_cb_)
        update_cb_("[" + std::to_string(done) + "/" + std::to_string(total_files_) + "] " + current_file_,
                   download_progress_);
}

void PatchSystem::ApplyUpdate() {
    current_ = latest_;
    update_available_ = false;
    downloading_ = false;
    download_progress_ = 1.0f;
    spdlog::info("PatchSystem: updated to v{}", current_.ToString());
    if (update_cb_) update_cb_("Update applied!", 1.0f);
}

void PatchSystem::CancelDownload() {
    cancelled_ = true;
    downloading_ = false;
    pending_files_.clear();
    spdlog::info("PatchSystem: download cancelled");
}

bool PatchSystem::VerifyFiles() {
    if (manifest_.files.empty()) {
        spdlog::warn("PatchSystem: no manifest loaded for verification");
        return false;
    }
    bool all_ok = true;
    for (const auto& f : manifest_.files) {
        if (!SHA256Verify(f.path, f.sha256)) {
            spdlog::warn("PatchSystem: verification failed: {}", f.path);
            all_ok = false;
        }
    }
    if (all_ok && update_cb_) update_cb_("All files verified", 1.0f);
    return all_ok;
}

// ── HTTP GET implementation ─────────────────────────────────────────────────

bool PatchSystem::HttpGet(const std::string& url, std::vector<uint8_t>& response, int64_t max_size) {
    SocketInit();
    std::string host; int port = 80; std::string path;
    if (!ParseUrl(url, host, port, path)) return false;

    SOCKET fd = TcpConnect(host, port);
    if (fd == INVALID_SOCKET) { SocketCleanup(); return false; }
    bool ok = false;
    if (HttpSendRequest(fd, host, path)) {
        int status = HttpReadStatus(fd);
        if (status == 200) {
            int64_t content_len = HttpReadContentLength(fd);
            if (max_size > 0 && content_len > max_size) content_len = max_size;
            ok = HttpReadBody(fd, response, content_len, nullptr);
        }
    }
    closesocket(fd);
    SocketCleanup();
    return ok;
}

bool PatchSystem::HttpDownloadFile(const std::string& url, const std::string& dest_path,
                                    int64_t expected_size,
                                    std::function<void(float)> progress_cb) {
    SocketInit();
    std::string host; int port = 80; std::string path;
    if (!ParseUrl(url, host, port, path)) { SocketCleanup(); return false; }

    SOCKET fd = TcpConnect(host, port);
    if (fd == INVALID_SOCKET) { SocketCleanup(); return false; }

    std::ofstream out(dest_path, std::ios::binary);
    if (!out) { closesocket(fd); SocketCleanup(); return false; }

    bool ok = false;
    if (HttpSendRequest(fd, host, path)) {
        int status = HttpReadStatus(fd);
        if (status == 200) {
            int64_t content_len = HttpReadContentLength(fd);
            if (expected_size > 0 && content_len != expected_size) {
                spdlog::warn("PatchSystem: size mismatch (expected {}, got {})",
                             expected_size, content_len);
            }
            int64_t total = 0;
            char buf[65536];
            while (true) {
                int n = (int)recv(fd, buf, sizeof(buf), 0);
                if (n < 0) break;
                if (n == 0) { ok = true; break; }
                out.write(buf, n);
                total += n;
                if (progress_cb && content_len > 0)
                    progress_cb((float)total / (float)content_len);
            }
        }
    }
    out.close();
    closesocket(fd);
    SocketCleanup();
    return ok;
}

// ── SHA-256 helpers ─────────────────────────────────────────────────────────

std::string PatchSystem::ComputeSHA256(const std::string& path) const {
    std::ifstream f(path, std::ios::binary);
    if (!f) return "";
    Sha256Ctx ctx;
    sha256_init(&ctx);
    uint8_t buf[65536];
    while (f) {
        f.read((char*)buf, sizeof(buf));
        sha256_update(&ctx, buf, (size_t)f.gcount());
    }
    uint8_t digest[32];
    sha256_final(&ctx, digest);
    return sha256_hex(digest);
}

bool PatchSystem::SHA256Verify(const std::string& path, const std::string& expected) const {
    if (expected.empty()) return true;
    std::string actual = ComputeSHA256(path);
    bool ok = (actual == expected);
    if (!ok)
        spdlog::debug("PatchSystem: SHA-256 mismatch for {} (expected={}, actual={})",
                      path, expected, actual);
    return ok;
}

void PatchSystem::ScanInstallDirectory() {
    // Simple stub: scans current directory for files matching manifest entries
    for (auto& f : manifest_.files) {
        struct stat st{};
        if (stat(f.path.c_str(), &st) == 0) {
            f.size = (int64_t)st.st_size;
        }
    }
}

std::string PatchSystem::GetChangelog() const {
    return patch_notes_;
}
