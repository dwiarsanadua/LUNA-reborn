#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct AnmRotKey {
    glm::quat rotation{1,0,0,0};
};

struct AnmPosKey {
    glm::vec3 position{0};
};

struct AnmBoneTrack {
    std::string name;
    std::vector<AnmRotKey> rot_keys;
    std::vector<AnmPosKey> pos_keys;
    std::vector<glm::vec3> scale_keys; // simplified — no axis/angle
};

struct AnmClip {
    std::string name;
    float duration = 1.0f;
    float fps = 30.0f;
    int num_frames = 0;
    std::vector<AnmBoneTrack> tracks;
};

class AnmParser {
public:
    static bool Load(const std::string& path, AnmClip& out) {
        std::ifstream f(path, std::ios::binary);
        if (!f) return false;
        f.seekg(0, std::ios::end);
        size_t fileSize = f.tellg();
        f.seekg(0);
        if (fileSize < 160) return false;
        std::vector<uint8_t> buf(fileSize);
        f.read(reinterpret_cast<char*>(buf.data()), fileSize);

        auto rdu32 = [&](size_t off) -> uint32_t { uint32_t v; std::memcpy(&v, buf.data() + off, 4); return v; };
        auto rdf32 = [&](size_t off) -> float { float v; std::memcpy(&v, buf.data() + off, 4); return v; };

        // FILE_MOTION_HEADER
        uint32_t version = rdu32(0);
        (void)version;
        uint32_t lastFrame = rdu32(12);
        uint32_t frameSpeed = rdu32(16);
        uint32_t objNum = rdu32(20);
        char nameBuf[129] = {};
        std::memcpy(nameBuf, buf.data() + 32, 128);

        out.name = nameBuf;
        out.num_frames = lastFrame;
        out.fps = frameSpeed > 0 ? static_cast<float>(frameSpeed) : 30.0f;
        out.duration = out.fps > 0 ? static_cast<float>(lastFrame) / out.fps : 1.0f;

        size_t off = 160;
        for (uint32_t i = 0; i < objNum; i++) {
            if (off + 8 > fileSize) break;
            uint32_t objType = rdu32(off);
            uint32_t objSize = rdu32(off + 4);
            (void)objType;
            off += 8;
            if (off + 152 > fileSize) break;

            // FILE_MOTION_OBJECT_HEADER
            uint32_t idx = rdu32(off);
            uint32_t rotNum = rdu32(off + 4);
            uint32_t posNum = rdu32(off + 8);
            uint32_t scaleNum = rdu32(off + 12);
            uint32_t meshNum = rdu32(off + 16);
            (void)idx; (void)meshNum;

            char objName[129] = {};
            std::memcpy(objName, buf.data() + off + 20, 128);
            off += 152;

            if (rotNum == 0 && posNum == 0 && scaleNum == 0) {
                // Skip empty bone
                continue;
            }

            AnmBoneTrack track;
            track.name = objName;
            track.rot_keys.resize(rotNum);
            track.pos_keys.resize(posNum);
            track.scale_keys.resize(scaleNum);

            // Rotation keys:  DWORD ticks + DWORD frame + float[4] quat = 24 bytes each
            for (uint32_t k = 0; k < rotNum && off + 24 <= fileSize; k++) {
                float qx = rdf32(off + 8);
                float qy = rdf32(off + 12);
                float qz = rdf32(off + 16);
                float qw = rdf32(off + 20);
                track.rot_keys[k].rotation = glm::quat(qw, qx, qy, qz);
                off += 24;
            }

            // Position keys: DWORD ticks + DWORD frame + float[3] = 20 bytes each
            for (uint32_t k = 0; k < posNum && off + 20 <= fileSize; k++) {
                float px = rdf32(off + 8);
                float py = rdf32(off + 12);
                float pz = rdf32(off + 16);
                track.pos_keys[k].position = glm::vec3(px, py, pz);
                off += 20;
            }

            // Scale keys: DWORD ticks + DWORD frame + float[3] scale + float[3] axis + float angle = 36 bytes
            for (uint32_t k = 0; k < scaleNum && off + 36 <= fileSize; k++) {
                float sx = rdf32(off + 8);
                float sy = rdf32(off + 12);
                float sz = rdf32(off + 16);
                track.scale_keys[k] = glm::vec3(sx, sy, sz);
                off += 36;
            }

            // Skip mesh keys
            for (uint32_t k = 0; k < meshNum && off + 16 <= fileSize; k++) {
                uint32_t vn = rdu32(off + 8);
                uint32_t tvn = rdu32(off + 12);
                off += 16 + vn * 12 + tvn * 8;
            }

            out.tracks.push_back(track);
        }

        return !out.tracks.empty();
    }
};
