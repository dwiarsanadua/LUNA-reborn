#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <cmath>

struct HeightField {
    int width = 0;
    int height = 0;
    std::vector<float> data;
    float hmin = 0, hmax = 0;
};

class HflParser {
public:
    static bool Load(const std::string& path, HeightField& out, int preferred_w = 0) {
        std::ifstream f(path, std::ios::binary);
        if (!f) return false;
        f.seekg(0, std::ios::end);
        size_t fileSize = f.tellg();
        f.seekg(0);
        if (fileSize < 128) return false;
        std::vector<uint8_t> buf(fileSize);
        f.read(reinterpret_cast<char*>(buf.data()), fileSize);

        auto rd32 = [&](size_t off) -> float { float v; std::memcpy(&v, buf.data() + off, 4); return v; };

        // Grid dimensions to try
        std::vector<int> dims = {1024, 512, 256, 129, 128, 65, 64, 33, 32};

        // Try float32 with all offsets in 4-byte steps
        for (int hdr = 0; hdr < 2048 && hdr < (int)fileSize; hdr += 4) {
            size_t rawSize = fileSize - hdr;
            int total_floats = static_cast<int>(rawSize / 4);

            for (int w : dims) {
                if (preferred_w > 0 && w != preferred_w) continue;
                int h = total_floats / w;
                if (h < 8 || h > 2048) continue;
                if (w * h != total_floats) continue;

                std::vector<float> heights;
                heights.reserve(total_floats);
                float hmin = 1e9, hmax = -1e9;
                int bad_count = 0;
                int skip_header_floats = 28;
                for (int i = 0; i < total_floats; i++) {
                    float v = rd32(hdr + i * 4);
                    if (i < skip_header_floats) { v = 0; }
                    else if (std::isnan(v) || std::isinf(v)) { v = 0; bad_count++; }
                    else if (std::abs(v) > 100000.0f) { v = (v > 0) ? 100000.0f : -100000.0f; bad_count++; }
                    heights.push_back(v);
                    hmin = std::min(hmin, v);
                    hmax = std::max(hmax, v);
                }
                if (bad_count > total_floats / 10 || hmax - hmin < 1.0f) continue;

                out.width = w;
                out.height = h;
                out.data = std::move(heights);
                out.hmin = hmin;
                out.hmax = hmax;
                return true;
            }
        }

        // Float32 failed; try as uint16/int16 (some HFL files store heights as 16-bit integers)
        {
            auto rd16 = [&](size_t off) -> uint16_t { uint16_t v; std::memcpy(&v, buf.data() + off, 2); return v; };
            auto rd16s = [&](size_t off) -> int16_t { int16_t v; std::memcpy(&v, buf.data() + off, 2); return v; };

            for (int hdr = 0; hdr < 2048 && hdr < (int)fileSize; hdr += 2) {
                size_t rawSize = fileSize - hdr;
                int total_shorts = static_cast<int>(rawSize / 2);

                for (int w : dims) {
                    if (preferred_w > 0 && w != preferred_w) continue;
                    int h = total_shorts / w;
                    if (h < 8 || h > 4096) continue;
                    if (w * h != total_shorts) continue;

                    // Try unsigned 16-bit
                    std::vector<float> heights;
                    heights.reserve(total_shorts);
                    float hmin = 1e9, hmax = -1e9;
                    int bad = 0;
                    for (int i = 28; i < total_shorts; i++) {
                        float v = static_cast<float>(rd16(hdr + i * 2));
                        if (v > 60000) { bad++; continue; }
                        heights.push_back(v);
                        hmin = std::min(hmin, v);
                        hmax = std::max(hmax, v);
                    }
                    if (bad > total_shorts / 20 || hmax - hmin < 1.0f || heights.empty()) {
                        // Try signed 16-bit
                        heights.clear();
                        hmin = 1e9; hmax = -1e9; bad = 0;
                        for (int i = 28; i < total_shorts; i++) {
                            float v = static_cast<float>(rd16s(hdr + i * 2));
                            if (std::abs(v) > 30000) { bad++; continue; }
                            heights.push_back(v);
                            hmin = std::min(hmin, v);
                            hmax = std::max(hmax, v);
                        }
                        if (bad > total_shorts / 20 || hmax - hmin < 1.0f || heights.empty()) continue;
                    }

                    out.width = w;
                    out.height = h;
                    out.data = std::move(heights);
                    out.hmin = hmin;
                    out.hmax = hmax;
                    return true;
                }
            }
        }

        return false;
    }

    static bool WriteHGT(const std::string& path, const HeightField& hf) {
        std::ofstream f(path);
        if (!f) return false;
        f << hf.width << " " << hf.height << "\n";
        for (int y = 0; y < hf.height; y++) {
            for (int x = 0; x < hf.width; x++) {
                f << hf.data[y * hf.width + x];
                if (x < hf.width - 1) f << " ";
            }
            f << "\n";
        }
        return true;
    }

    static bool WriteOBJ(const std::string& path, const HeightField& hf, float scale = 0.01f) {
        std::ofstream f(path);
        if (!f) return false;
        int w = hf.width, h = hf.height;
        float cx = w * 0.5f, cz = h * 0.5f;

        f << "# HFL terrain: " << w << "x" << h << "\n";
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                float fx = (x - cx) * scale;
                float fz = (y - cz) * scale;
                float fy = hf.data[y * w + x] * scale;
                f << "v " << fx << " " << fy << " " << fz << "\n";
            }
        }
        f << "\n";
        for (int y = 0; y < h - 1; y++) {
            for (int x = 0; x < w - 1; x++) {
                int a = y * w + x;
                int b = a + 1;
                int c = (y + 1) * w + x;
                int d = c + 1;
                f << "f " << a+1 << " " << c+1 << " " << b+1 << "\n";
                f << "f " << b+1 << " " << c+1 << " " << d+1 << "\n";
            }
        }
        return true;
    }
};
