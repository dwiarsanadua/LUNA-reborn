#include "Launcher.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <spdlog/spdlog.h>

#ifdef _WIN32
    #include <direct.h>
    #include <io.h>
    #define mkdir(a, b) _mkdir(a)
#else
    #include <unistd.h>
#endif

// ── SHA-256 implementation (compact, no OpenSSL dependency) ────────────────

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

struct SHA256_CTX {
    uint32_t state[8];
    uint64_t count;
    uint8_t buffer[64];
};

static void sha256_transform(SHA256_CTX* ctx, const uint8_t* data) {
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

static void sha256_init(SHA256_CTX* ctx) {
    ctx->count = 0;
    ctx->state[0] = 0x6a09e667; ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372; ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f; ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab; ctx->state[7] = 0x5be0cd19;
}

static void sha256_update(SHA256_CTX* ctx, const uint8_t* data, size_t len) {
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

static void sha256_final(SHA256_CTX* ctx, uint8_t* digest) {
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

// ── Launcher implementation ────────────────────────────────────────────────

bool Launcher::Init(const std::string& base_url, const std::string& install_path) {
    base_url_ = base_url;
    install_path_ = install_path;
    if (install_path_.back() != '/') install_path_ += '/';
    spdlog::info("Launcher: init (url={}, path={})", base_url_, install_path_);

    // Ensure backup directory exists
    std::string backup = GetBackupPath();
    struct stat st{};
    if (stat(backup.c_str(), &st) != 0) {
#ifdef _WIN32
        _mkdir(backup.c_str());
#else
        mkdir(backup.c_str(), 0755);
#endif
    }

    return true;
}

void Launcher::Shutdown() {
    spdlog::info("Launcher: shutdown");
}

bool Launcher::FetchRemoteManifest() {
    remote_manifest_.version = "1.1.0";
    remote_manifest_.files = {
        {"LunaPlusClient", "abc123", 6291456, base_url_ + "/LunaPlusClient",
         base_url_ + "/LunaPlusClient.patch", 12453, "patchabc"},
        {"shaders/vs_default.bin", "def456", 875, base_url_ + "/shaders/vs_default.bin",
         "", 0, ""},
        {"shaders/fs_ui.bin", "ghi789", 644, base_url_ + "/shaders/fs_ui.bin",
         "", 0, ""},
    };
    spdlog::info("Launcher: remote manifest v{} ({} files)", remote_manifest_.version, remote_manifest_.files.size());

    if (status_cb_) status_cb_("Remote manifest v" + remote_manifest_.version + " loaded");
    return true;
}

bool Launcher::LoadLocalManifest() {
    std::string path = install_path_ + "manifest.json";
    std::ifstream f(path);
    if (!f) {
        spdlog::info("Launcher: no local manifest, fresh install");
        local_manifest_.version = "0.0.0";
        return false;
    }
    std::string line;
    while (std::getline(f, line)) {
        if (line.find("\"version\"") != std::string::npos) {
            auto q1 = line.find('"', line.find("version") + 8);
            auto q2 = line.find('"', q1 + 1);
            local_manifest_.version = line.substr(q1 + 1, q2 - q1 - 1);
        }
    }
    local_version_ = local_manifest_.version;
    spdlog::info("Launcher: local manifest v{}", local_manifest_.version);
    return true;
}

bool Launcher::SaveLocalManifest(const std::string& path) {
    std::string out_path = path.empty() ? install_path_ + "manifest.json" : path;
    std::ofstream f(out_path);
    if (!f) return false;
    f << "{\n  \"version\": \"" << remote_manifest_.version << "\",\n  \"files\": [\n";
    for (size_t i = 0; i < remote_manifest_.files.size(); i++) {
        auto& e = remote_manifest_.files[i];
        f << "    {\"path\":\"" << e.relative_path << "\",\"sha256\":\"" << e.sha256
          << "\",\"size\":" << e.file_size;
        if (!e.patch_sha256.empty())
            f << ",\"patch_sha256\":\"" << e.patch_sha256 << "\"";
        f << "}";
        if (i < remote_manifest_.files.size() - 1) f << ",";
        f << "\n";
    }
    f << "  ]\n}\n";
    local_version_ = remote_manifest_.version;
    spdlog::info("Launcher: manifest saved v{}", local_version_);
    return true;
}

std::string Launcher::ComputeSHA256(const uint8_t* data, size_t len) const {
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, data, len);
    uint8_t digest[32];
    sha256_final(&ctx, digest);
    return sha256_hex(digest);
}

std::string Launcher::SHA256File(const std::string& path) const {
    std::ifstream f(path, std::ios::binary);
    if (!f) return "";
    SHA256_CTX ctx;
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

bool Launcher::SHA256FileVerify(const std::string& path, const std::string& expected_sha256) const {
    if (expected_sha256.empty()) return true;
    std::string actual = SHA256File(path);
    bool ok = (actual == expected_sha256);
    if (!ok) {
        spdlog::error("Launcher: SHA-256 mismatch for {} (expected={}, actual={})",
                      path, expected_sha256, actual);
    }
    return ok;
}

bool Launcher::CreateBackup(const std::string& path) {
    std::string backup_path = GetBackupPath() + path;
    std::string src = install_path_ + path;
    std::string dst = backup_path;

    // Create parent directories
    size_t slash = dst.rfind('/');
    if (slash != std::string::npos) {
        std::string dir = dst.substr(0, slash);
#ifdef _WIN32
        _mkdir(dir.c_str());
#else
        mkdir(dir.c_str(), 0755);
#endif
    }

    // Copy file
    std::ifstream src_f(src, std::ios::binary);
    if (!src_f) return false; // File doesn't exist locally, nothing to backup
    std::ofstream dst_f(dst, std::ios::binary);
    if (!dst_f) return false;
    dst_f << src_f.rdbuf();

    spdlog::debug("Launcher: backed up {} -> {}", src, dst);
    return true;
}

bool Launcher::RestoreBackup(const std::string& path) {
    std::string backup_path = GetBackupPath() + path;
    std::string dest = install_path_ + path;
    std::ifstream src_f(backup_path, std::ios::binary);
    if (!src_f) {
        spdlog::warn("Launcher: no backup to restore for {}", path);
        return false;
    }
    std::ofstream dst_f(dest, std::ios::binary);
    if (!dst_f) return false;
    dst_f << src_f.rdbuf();
    spdlog::info("Launcher: restored {} from backup", path);
    return true;
}

bool Launcher::ApplyBsdiffPatch(const std::string& old_file, const std::string& patch_file, const std::string& new_file) {
    // bsdiff/bspatch algorithm implementation — simplified
    // In production, link against libbspatch or use a proper bsdiff library
    std::ifstream old_f(old_file, std::ios::binary | std::ios::ate);
    std::ifstream patch_f(patch_file, std::ios::binary | std::ios::ate);
    if (!old_f || !patch_f) return false;

    size_t old_size = (size_t)old_f.tellg();
    size_t patch_size = (size_t)patch_f.tellg();
    old_f.seekg(0);
    patch_f.seekg(0);

    std::vector<uint8_t> old_data(old_size);
    std::vector<uint8_t> patch_data(patch_size);
    old_f.read((char*)old_data.data(), old_size);
    patch_f.read((char*)patch_data.data(), patch_size);

    // Minimal bsdiff control data reader — applies patch to produce new file
    // This is a placeholder for actual bsdiff integration
    std::vector<uint8_t> new_data;
    if (patch_size < 32) return false;

    // Parse bsdiff header
    // Magic: "BSDIFF40"
    if (std::memcmp(patch_data.data(), "BSDIFF40", 8) != 0) return false;

    // Read new file size from header (offset 8, 8 bytes, little-endian)
    int64_t new_size = 0;
    for (int i = 0; i < 8; i++)
        new_size |= (int64_t)patch_data[8 + i] << (i * 8);

    new_data.resize((size_t)new_size);

    // Simple patch: copy old data, apply control sequences
    // For real bsdiff, use the bspatch algorithm from bsdiff.c
    size_t old_pos = 0, new_pos = 0;
    size_t ctrl_pos = 32; // after header

    while (new_pos < (size_t)new_size && ctrl_pos + 24 <= patch_size) {
        // Read control triplet (each 8 bytes, little-endian)
        auto read64 = [&](size_t off) -> int64_t {
            int64_t v = 0;
            for (int i = 0; i < 8; i++)
                v |= (int64_t)patch_data[off + i] << (i * 8);
            return v;
        };

        int64_t diff_len = read64(ctrl_pos);
        int64_t extra_len = read64(ctrl_pos + 8);
        int64_t seek_offset = read64(ctrl_pos + 16);
        ctrl_pos += 24;

        // Apply diff data
        for (int64_t i = 0; i < diff_len && new_pos < (size_t)new_size; i++) {
            uint8_t diff_byte = (ctrl_pos < patch_size) ? patch_data[ctrl_pos++] : 0;
            uint8_t old_byte = (old_pos < old_size) ? old_data[old_pos++] : 0;
            new_data[new_pos++] = old_byte + diff_byte;
        }

        // Copy extra data
        for (int64_t i = 0; i < extra_len && new_pos < (size_t)new_size; i++) {
            new_data[new_pos++] = (ctrl_pos < patch_size) ? patch_data[ctrl_pos++] : 0;
        }

        // Seek in old data
        old_pos = (size_t)((int64_t)old_pos + seek_offset);
    }

    // Write new file
    std::ofstream new_f(new_file, std::ios::binary);
    if (!new_f) return false;
    new_f.write((char*)new_data.data(), new_data.size());

    spdlog::info("Launcher: bsdiff patch applied ({} -> {} bytes)", old_size, new_size);
    return true;
}

bool Launcher::ApplyDifferentialPatch(const PatchEntry& entry, const std::string& local_path) {
    if (entry.patch_url.empty() || entry.patch_sha256.empty()) {
        spdlog::debug("Launcher: no patch for {}, full download required", entry.relative_path);
        return false;
    }

    // Download patch
    std::string patch_path = install_path_ + entry.relative_path + ".patch";
    if (!DownloadFile(entry.patch_url, patch_path)) {
        spdlog::error("Launcher: failed to download patch for {}", entry.relative_path);
        return false;
    }

    // Verify patch integrity
    if (!SHA256FileVerify(patch_path, entry.patch_sha256)) {
        spdlog::error("Launcher: patch integrity check failed for {}", entry.relative_path);
        std::remove(patch_path.c_str());
        return false;
    }

    // Apply patch
    std::string new_path = local_path + ".new";
    if (!ApplyBsdiffPatch(local_path, patch_path, new_path)) {
        spdlog::error("Launcher: failed to apply patch for {}", entry.relative_path);
        std::remove(patch_path.c_str());
        std::remove(new_path.c_str());
        return false;
    }

    // Verify result
    if (!SHA256FileVerify(new_path, entry.sha256)) {
        spdlog::error("Launcher: patched file SHA-256 mismatch for {}", entry.relative_path);
        std::remove(patch_path.c_str());
        std::remove(new_path.c_str());
        return false;
    }

    // Replace original with patched file
    std::rename(new_path.c_str(), local_path.c_str());
    std::remove(patch_path.c_str());

    spdlog::info("Launcher: differential patch applied for {}", entry.relative_path);
    return true;
}

std::vector<PatchEntry> Launcher::ComputeDelta() const {
    std::vector<PatchEntry> delta;
    for (auto& remote : remote_manifest_.files) {
        std::string local_path = install_path_ + remote.relative_path;
        std::ifstream test(local_path);
        if (!test) {
            delta.push_back(remote);
            continue;
        }
        test.close();
        std::string local_hash = SHA256File(local_path);
        if (local_hash != remote.sha256) {
            delta.push_back(remote);
        }
    }
    return delta;
}

std::vector<PatchEntry> Launcher::ComputeDifferentialDelta() const {
    std::vector<PatchEntry> delta;
    for (auto& remote : remote_manifest_.files) {
        std::string local_path = install_path_ + remote.relative_path;
        std::ifstream test(local_path);
        if (!test) {
            delta.push_back(remote);
            continue;
        }
        test.close();
        std::string local_hash = SHA256File(local_path);
        if (local_hash != remote.sha256) {
            delta.push_back(remote);
        }
    }
    return delta;
}

bool Launcher::DownloadUpdates(const std::vector<PatchEntry>& updates) {
    if (updates.empty()) {
        spdlog::info("Launcher: no updates needed");
        if (status_cb_) status_cb_("Up to date!");
        return true;
    }

    updating_ = true;
    rolling_back_ = false;
    updated_files_.clear();
    backup_created_ = false;
    last_error_.clear();

    int total = (int)updates.size();
    int64_t total_bytes = 0;
    for (auto& u : updates) total_bytes += u.file_size;

    spdlog::info("Launcher: downloading {} files ({} bytes)...", total, total_bytes);
    if (status_cb_) status_cb_("Downloading " + std::to_string(total) + " files...");

    for (size_t i = 0; i < updates.size(); i++) {
        const auto& entry = updates[i];
        progress_ = (float)(i + 1) / (float)total;
        std::string dest = install_path_ + entry.relative_path;

        if (progress_cb_) progress_cb_(progress_, entry.relative_path);
        if (status_cb_) status_cb_("[" + std::to_string(i + 1) + "/" + std::to_string(total) + "] " + entry.relative_path);

        // Create parent directories
        size_t slash = dest.rfind('/');
        if (slash != std::string::npos) {
            std::string dir = dest.substr(0, slash);
#ifdef _WIN32
            _mkdir(dir.c_str());
#else
            mkdir(dir.c_str(), 0755);
#endif
        }

        // Create backup before overwriting
        if (!backup_created_) {
            backup_created_ = true;
            CreateBackup(entry.relative_path);
        }

        bool ok = false;

        // Try differential patch first
        if (ApplyDifferentialPatch(entry, dest)) {
            ok = true;
        } else {
            // Full download fallback
            if (DownloadFile(entry.download_url, dest)) {
                // Verify full download
                ok = SHA256FileVerify(dest, entry.sha256);
                if (!ok) {
                    last_error_ = "SHA-256 verification failed for " + entry.relative_path;
                    spdlog::error("Launcher: {}", last_error_);
                    if (error_cb_) error_cb_(last_error_);
                }
            } else {
                last_error_ = "Download failed for " + entry.relative_path;
                spdlog::error("Launcher: {}", last_error_);
                if (error_cb_) error_cb_(last_error_);
            }
        }

        if (!ok) {
            spdlog::error("Launcher: update failed for {}, initiating rollback", entry.relative_path);
            if (status_cb_) status_cb_("Update failed! Rolling back...");
            Rollback();
            updating_ = false;
            return false;
        }

        updated_files_.push_back(entry.relative_path);
        spdlog::info("Launcher:   [{}/{}] {} (OK)", i + 1, total, entry.relative_path);
    }

    SaveLocalManifest();
    updating_ = false;
    progress_ = 1.0f;
    spdlog::info("Launcher: update complete");
    if (status_cb_) status_cb_("Update complete!");
    return true;
}

bool Launcher::Rollback() {
    if (updated_files_.empty()) {
        spdlog::info("Launcher: no files to rollback");
        return true;
    }

    rolling_back_ = true;
    spdlog::info("Launcher: rolling back {} files...", updated_files_.size());
    if (status_cb_) status_cb_("Rolling back...");

    bool all_ok = true;
    for (auto it = updated_files_.rbegin(); it != updated_files_.rend(); ++it) {
        if (!RestoreBackup(*it)) {
            spdlog::error("Launcher: rollback failed for {}", *it);
            all_ok = false;
        }
    }

    if (all_ok) {
        spdlog::info("Launcher: rollback complete");
        if (status_cb_) status_cb_("Rollback complete");
    } else {
        spdlog::error("Launcher: rollback completed with errors");
        last_error_ = "Rollback completed with errors — some files may be corrupted";
        if (error_cb_) error_cb_(last_error_);
    }

    updated_files_.clear();
    rolling_back_ = false;
    return all_ok;
}

bool Launcher::VerifyInstallation() {
    spdlog::info("Launcher: verifying installation...");
    if (status_cb_) status_cb_("Verifying installation...");

    bool all_ok = true;
    for (auto& entry : remote_manifest_.files) {
        std::string local_path = install_path_ + entry.relative_path;
        if (!SHA256FileVerify(local_path, entry.sha256)) {
            spdlog::warn("Launcher: verification failed for {}", entry.relative_path);
            all_ok = false;
        }
    }

    if (all_ok) {
        spdlog::info("Launcher: all files verified");
        if (status_cb_) status_cb_("All files verified");
    } else {
        last_error_ = "Some files failed verification";
        if (error_cb_) error_cb_(last_error_);
    }

    return all_ok;
}

int64_t Launcher::GetDownloadSize() const {
    int64_t total = 0;
    for (auto& e : remote_manifest_.files) total += e.file_size;
    return total;
}

bool Launcher::DownloadFile(const std::string& url, const std::string& dest) {
    (void)url;
    (void)dest;
    // In production: use libcurl or WinHTTP
    spdlog::debug("Launcher: DownloadFile({}, {}) [stub]", url, dest);
    return true;
}
