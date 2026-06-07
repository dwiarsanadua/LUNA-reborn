#pragma once
#include "ModLoader.h"
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

class GltfWriter {
public:
    static bool WriteGLB(const std::string& path, const ModMesh& mesh, float scale = 1.0f) {
        size_t count = mesh.positions.size();
        if (count == 0 || mesh.indices.empty()) return false;
        size_t idxCount = mesh.indices.size();

        // Transform
        float cx = 0, cy = 0, cz = 0;
        for (auto& p : mesh.positions) { cx += p.x; cy += p.y; cz += p.z; }
        cx /= count; cy /= count; cz /= count;

        std::vector<glm::vec3> pos(count);
        std::vector<glm::vec2> uv(count);
        std::vector<glm::vec3> nrm(count);
        for (size_t i = 0; i < count; i++) {
            pos[i] = (mesh.positions[i] - glm::vec3(cx, cy, cz)) * scale;
            uv[i] = mesh.uvs[i];
            nrm[i] = mesh.normals[i];
        }

        // Compute min/max for accessors
        glm::vec3 pMin = pos[0], pMax = pos[0];
        for (auto& p : pos) {
            pMin = glm::min(pMin, p); pMax = glm::max(pMax, p);
        }

        // Binary buffer layout
        size_t posSize = count * 12;
        size_t uvSize = count * 8;
        size_t nrmSize = count * 12;
        size_t idxSize = idxCount * 2;
        size_t binSize = posSize + uvSize + nrmSize + idxSize;

        // Build buffer views & accessors
        std::stringstream json;
        json << std::setprecision(6) << std::fixed;

        json << R"({"asset":{"version":"2.0","generator":"luna_chx_to_gltf"})";
        json << R"(,"scene":0,"scenes":[{"nodes":[0]}])";
        json << R"(,"nodes":[{"mesh":0,"name":")" << mesh.name << "\"}]";

        json << R"(,"meshes":[{"primitives":[{)";
        json << R"("attributes":{"POSITION":0,"TEXCOORD_0":1,"NORMAL":2},"indices":3)";
        json << "}]}]";

        json << R"(,"accessors":[{)";
        json << R"("bufferView":0,"byteOffset":0,"componentType":5126,"count":)";
        json << count << R"(,"type":"VEC3","min":[)";
        json << pMin.x << "," << pMin.y << "," << pMin.z << R"(],"max":[)";
        json << pMax.x << "," << pMax.y << "," << pMax.z << "]},";
        json << R"({"bufferView":1,"byteOffset":0,"componentType":5126,"count":)";
        json << count << R"(,"type":"VEC2"})" << ",";
        json << R"({"bufferView":2,"byteOffset":0,"componentType":5126,"count":)";
        json << count << R"(,"type":"VEC3"})" << ",";
        json << R"({"bufferView":3,"byteOffset":0,"componentType":5123,"count":)";
        json << idxCount << R"(,"type":"SCALAR"})";
        json << "]";

        json << R"(,"bufferViews":[{)";
        json << R"("buffer":0,"byteOffset":0,"byteLength":)" << posSize << "},";
        json << R"({"buffer":0,"byteOffset":)" << posSize << R"(,"byteLength":)" << uvSize << "},";
        json << R"({"buffer":0,"byteOffset":)" << (posSize + uvSize) << R"(,"byteLength":)" << nrmSize << "},";
        json << R"({"buffer":0,"byteOffset":)" << (posSize + uvSize + nrmSize) << R"(,"byteLength":)" << idxSize << "}";
        json << "]";

        json << R"(,"buffers":[{"byteLength":)" << binSize << "}]";
        json << "}";

        std::string jsonStr = json.str();

        // GLB header + chunks
        std::ofstream out(path, std::ios::binary);
        if (!out) return false;

        // Pad JSON to 4-byte alignment
        size_t jsonLen = jsonStr.size();
        size_t jsonPad = (4 - jsonLen % 4) % 4;
        jsonStr.append(jsonPad, ' ');

        size_t binPad = (4 - binSize % 4) % 4;

        uint32_t headerLen = 12;
        uint32_t jsonChunkLen = 8 + static_cast<uint32_t>(jsonStr.size());
        uint32_t binChunkLen = 8 + static_cast<uint32_t>(binSize + binPad);
        uint32_t totalLen = headerLen + jsonChunkLen + binChunkLen;

        auto write32 = [&](uint32_t v) { out.write(reinterpret_cast<const char*>(&v), 4); };

        write32(0x46546C67); write32(2); write32(totalLen);                          // header
        write32(static_cast<uint32_t>(jsonStr.size())); write32(0x4E4F534A);          // JSON chunk
        out.write(jsonStr.data(), static_cast<std::streamsize>(jsonStr.size()));
        write32(static_cast<uint32_t>(binSize + binPad)); write32(0x004E4942);        // BIN chunk

        // Write binary data
        for (auto& p : pos) { out.write(reinterpret_cast<const char*>(&p), 12); }
        for (auto& u : uv) { out.write(reinterpret_cast<const char*>(&u), 8); }
        for (auto& n : nrm) { out.write(reinterpret_cast<const char*>(&n), 12); }
        out.write(reinterpret_cast<const char*>(mesh.indices.data()),
                  static_cast<std::streamsize>(idxSize));
        for (size_t i = 0; i < binPad; i++) out.put(0);

        return true;
    }
};
