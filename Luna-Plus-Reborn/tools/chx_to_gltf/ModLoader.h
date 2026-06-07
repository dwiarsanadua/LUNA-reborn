#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <glm/glm.hpp>

struct ModMesh {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<uint16_t> indices;
    std::string name;
};

class ModLoader {
public:
    static bool Load(const std::string& path, ModMesh& out) {
        std::ifstream f(path, std::ios::binary);
        if (!f) return false;
        f.seekg(0, std::ios::end);
        size_t fileSize = f.tellg();
        f.seekg(0);
        if (fileSize < 28) return false;
        std::vector<uint8_t> buf(fileSize);
        f.read(reinterpret_cast<char*>(buf.data()), fileSize);

        const auto r = [&](size_t off) { return buf.data() + off; };
        auto rd32 = [&](size_t off) -> uint32_t {
            uint32_t v; std::memcpy(&v, buf.data() + off, 4); return v;
        };

        uint32_t ver = rd32(0), objNum = rd32(4), mtlNum = rd32(8);
        (void)ver;

        size_t off = 28;
        for (uint32_t i = 0; i < mtlNum; i++) {
            if (off + 8 > fileSize) return false;
            uint32_t mtype = rd32(off), msize = rd32(off + 4);
            (void)mtype;
            off += 8 + msize;
        }

        std::vector<glm::vec3> allPos;
        std::vector<glm::vec2> allUv;
        std::vector<uint16_t> allIdx;
        std::vector<glm::vec3> allNrm;
        size_t baseV = 0;

        for (uint32_t oi = 0; oi < objNum; oi++) {
            if (off + 8 > fileSize) break;
            uint32_t otype = rd32(off), osize = rd32(off + 4);
            off += 8;
            size_t objEnd = off + osize;
            if (objEnd > fileSize) break;

            if (otype != 0xF4000000) { off = objEnd; continue; }

            uint32_t childNum = rd32(off + 188);
            off += 324 + childNum * 4;
            if (off + 7 * 4 + 24 > fileSize) { off = objEnd; continue; }

            uint32_t maxVn = rd32(off), vn = rd32(off + 4);
            uint32_t origVn = rd32(off + 8), extVn = rd32(off + 12);
            uint32_t texVn = rd32(off + 16), mtlIdx = rd32(off + 20);
            uint32_t fgNum = rd32(off + 24);
            (void)maxVn; (void)origVn; (void)mtlIdx;
            if (vn == 0 || vn > 50000 || fgNum == 0) { off = objEnd; continue; }
            off += 28;

            off += 4 + 4 + 12 + 4;

            baseV = allPos.size();
            for (uint32_t i = 0; i < vn; i++) {
                if (off + 12 > fileSize) break;
                float x, y, z;
                std::memcpy(&x, r(off), 4); std::memcpy(&y, r(off + 4), 4); std::memcpy(&z, r(off + 8), 4);
                allPos.emplace_back(x, y, z);
                off += 12;
            }
            for (uint32_t i = 0; i < texVn; i++) {
                if (off + 8 > fileSize) break;
                float u, v;
                std::memcpy(&u, r(off), 4); std::memcpy(&v, r(off + 4), 4);
                allUv.emplace_back(u, 1.0f - v);
                off += 8;
            }
            while (allUv.size() < allPos.size()) allUv.emplace_back(0, 0);

            off += extVn * 4;

            for (uint32_t fg = 0; fg < fgNum; fg++) {
                if (off + 24 > fileSize) break;
                uint32_t fgMtl = rd32(off), fgIdx = rd32(off + 4);
                uint32_t fgFaces = rd32(off + 8), fgMax = rd32(off + 12);
                uint32_t fgVi = rd32(off + 16), fgLuv = rd32(off + 20);
                (void)fgMtl; (void)fgIdx; (void)fgMax; (void)fgVi;
                if (fgFaces == 0 || fgFaces > 50000) break;
                size_t idxOff = off + 24;
                for (uint32_t f = 0; f < fgFaces; f++) {
                    if (idxOff + f * 6 + 6 > fileSize) break;
                    uint16_t a, b, c;
                    std::memcpy(&a, r(idxOff + f * 6), 2);
                    std::memcpy(&b, r(idxOff + f * 6 + 2), 2);
                    std::memcpy(&c, r(idxOff + f * 6 + 4), 2);
                    if (a == b || b == c || a == c) continue;
                    if (a < vn && b < vn && c < vn) {
                        allIdx.push_back(static_cast<uint16_t>(baseV + a));
                        allIdx.push_back(static_cast<uint16_t>(baseV + b));
                        allIdx.push_back(static_cast<uint16_t>(baseV + c));
                    }
                }
                size_t fgTotal = 24 + fgFaces * 6;
                if (fgLuv > 0) fgTotal += fgLuv * 8;
                off += fgTotal;
            }
            off = std::max(off, objEnd);
        }

        if (allPos.empty() || allIdx.empty()) return false;

        // Append to existing output (support multi-part .chx)
        size_t vertBase = out.positions.size();

        // Normal computation on the local allPos, then remap normals to output indices
        std::vector<glm::vec3> localNrm(allPos.size(), glm::vec3(0));
        for (size_t i = 0; i < allIdx.size(); i += 3) {
            glm::vec3& p0 = allPos[allIdx[i] - baseV];
            glm::vec3& p1 = allPos[allIdx[i + 1] - baseV];
            glm::vec3& p2 = allPos[allIdx[i + 2] - baseV];
            glm::vec3 n = glm::normalize(glm::cross(p1 - p0, p2 - p0));
            if (!std::isfinite(n.x)) n = glm::vec3(0, 1, 0);
            localNrm[allIdx[i] - baseV] += n;
            localNrm[allIdx[i + 1] - baseV] += n;
            localNrm[allIdx[i + 2] - baseV] += n;
        }
        for (auto& n : localNrm) {
            float len = glm::length(n);
            if (len > 0.0001f) n /= len; else n = glm::vec3(0, 1, 0);
        }

        // Append vertices, uvs, normals to output
        out.positions.insert(out.positions.end(), allPos.begin(), allPos.end());
        out.uvs.insert(out.uvs.end(), allUv.begin(), allUv.end());
        out.normals.insert(out.normals.end(), localNrm.begin(), localNrm.end());

        // Append indices with offset
        for (auto idx : allIdx) {
            out.indices.push_back(static_cast<uint16_t>(idx - baseV + vertBase));
        }

        auto lastSlash = path.find_last_of("/\\");
        auto lastDot = path.find_last_of('.');
        out.name = (lastSlash != std::string::npos) ? path.substr(lastSlash + 1, lastDot - lastSlash - 1) : path;
        return true;
    }
};
