#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>

#pragma pack(push, 4)

struct FILE_MOTION_HEADER {
    uint32_t dwVersion;
    uint32_t dwTicksPerFrame;
    uint32_t dwFirstFrame;
    uint32_t dwLastFrame;
    uint32_t dwFrameSpeed;
    uint32_t dwMotionObjectNum;
    uint32_t dwKeyFrameStep;
    char     szMotionName[128];
};

struct FILE_MOTION_OBJECT_HEADER {
    uint32_t dwIndex;
    uint32_t dwRotKeyNum;
    uint32_t dwPosKeyNum;
    uint32_t dwScaleKeyNum;
    uint32_t dwAnimatedMeshKeyNum;
    char     szObjectName[128];
    uint32_t motionFlag;
};

struct ROT_KEY {
    uint32_t dwTicks;
    uint32_t dwFrame;
    float    qx, qy, qz, qw;
};

struct POS_KEY {
    uint32_t dwTicks;
    uint32_t dwFrame;
    float    x, y, z;
};

struct SCALE_KEY {
    uint32_t dwTicks;
    uint32_t dwFrame;
    float    sx, sy, sz;
    float    ax, ay, az;
    float    angle;
};

struct ANIMATED_MESH_KEY {
    uint32_t dwTicks;
    uint32_t dwFrame;
    uint32_t dwVertexNum;
    uint32_t dwTexVertexNum;
};

#pragma pack(pop)

#define MOTION_OBJECT_TYPE_MASK 0xff000000u
#define MOTION_KEY_TYPE_POS   0x000000ffu
#define MOTION_KEY_TYPE_ROT   0x0000ff00u
#define MOTION_KEY_TYPE_SCALE 0x00ff0000u

static inline uint32_t read_u32(FILE* f) {
    uint32_t v;
    fread(&v, 4, 1, f);
    return v;
}

static inline uint16_t read_u16(FILE* f) {
    uint16_t v;
    fread(&v, 2, 1, f);
    return v;
}

static inline void read_buf(FILE* f, void* buf, size_t sz) {
    fread(buf, 1, sz, f);
}

static void json_escape(FILE* f, const char* s) {
    fputc('"', f);
    while (*s) {
        if (*s == '"' || *s == '\\') fputc('\\', f);
        fputc(*s, f);
        s++;
    }
    fputc('"', f);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.anm> [output.json]\n", argv[0]);
        return 1;
    }

    const char* in_path = argv[1];
    const char* out_path = (argc > 2) ? argv[2] : nullptr;

    FILE* f = fopen(in_path, "rb");
    if (!f) { fprintf(stderr, "Cannot open %s\n", in_path); return 1; }

    FILE_MOTION_HEADER hdr;
    read_buf(f, &hdr, sizeof(hdr));

    char out_fn[1024];
    if (!out_path) {
        snprintf(out_fn, sizeof(out_fn), "%s.json", in_path);
        out_path = out_fn;
    }

    FILE* fo = fopen(out_path, "w");
    if (!fo) { fprintf(stderr, "Cannot write %s\n", out_path); fclose(f); return 1; }

    fprintf(fo, "{\n");
    fprintf(fo, "  \"version\": %u,\n", hdr.dwVersion);
    fprintf(fo, "  \"ticksPerFrame\": %u,\n", hdr.dwTicksPerFrame);
    fprintf(fo, "  \"firstFrame\": %u,\n", hdr.dwFirstFrame);
    fprintf(fo, "  \"lastFrame\": %u,\n", hdr.dwLastFrame);
    fprintf(fo, "  \"frameSpeed\": %u,\n", hdr.dwFrameSpeed);
    fprintf(fo, "  \"keyFrameStep\": %u,\n", hdr.dwKeyFrameStep);
    fprintf(fo, "  \"motionName\": ");
    json_escape(fo, hdr.szMotionName);
    fprintf(fo, ",\n");
    fprintf(fo, "  \"objects\": [\n");

    for (uint32_t i = 0; i < hdr.dwMotionObjectNum; i++) {
        uint32_t type = read_u32(f);
        uint32_t size = read_u32(f);
        (void)type; (void)size;
        long obj_end = ftell(f) + size;

        FILE_MOTION_OBJECT_HEADER obj_hdr;
        read_buf(f, &obj_hdr, sizeof(obj_hdr));

        if (i > 0) fprintf(fo, ",\n");

        fprintf(fo, "    {\n");
        fprintf(fo, "      \"name\": ");
        json_escape(fo, obj_hdr.szObjectName);
        fprintf(fo, ",\n");
        fprintf(fo, "      \"index\": %u,\n", obj_hdr.dwIndex);
        fprintf(fo, "      \"flag\": %u,\n", obj_hdr.motionFlag);

        // POS keys
        fprintf(fo, "      \"posKeys\": [\n");
        for (uint32_t j = 0; j < obj_hdr.dwPosKeyNum; j++) {
            POS_KEY k;
            read_buf(f, &k, sizeof(k));
            if (j > 0) fprintf(fo, ",\n");
            fprintf(fo, "        { \"ticks\": %u, \"frame\": %u, \"x\": %.6f, \"y\": %.6f, \"z\": %.6f }",
                    k.dwTicks, k.dwFrame, k.x, k.y, k.z);
        }
        fprintf(fo, "\n      ],\n");

        // ROT keys
        fprintf(fo, "      \"rotKeys\": [\n");
        for (uint32_t j = 0; j < obj_hdr.dwRotKeyNum; j++) {
            ROT_KEY k;
            read_buf(f, &k, sizeof(k));
            if (j > 0) fprintf(fo, ",\n");
            fprintf(fo, "        { \"ticks\": %u, \"frame\": %u, \"qx\": %.6f, \"qy\": %.6f, \"qz\": %.6f, \"qw\": %.6f }",
                    k.dwTicks, k.dwFrame, k.qx, k.qy, k.qz, k.qw);
        }
        fprintf(fo, "\n      ],\n");

        // SCALE keys
        fprintf(fo, "      \"scaleKeys\": [\n");
        for (uint32_t j = 0; j < obj_hdr.dwScaleKeyNum; j++) {
            SCALE_KEY k;
            read_buf(f, &k, sizeof(k));
            if (j > 0) fprintf(fo, ",\n");
            fprintf(fo, "        { \"ticks\": %u, \"frame\": %u, \"sx\": %.6f, \"sy\": %.6f, \"sz\": %.6f, \"ax\": %.6f, \"ay\": %.6f, \"az\": %.6f, \"angle\": %.6f }",
                    k.dwTicks, k.dwFrame, k.sx, k.sy, k.sz, k.ax, k.ay, k.az, k.angle);
        }
        fprintf(fo, "\n      ],\n");

        // Animated mesh keys
        fprintf(fo, "      \"meshKeys\": [\n");
        for (uint32_t j = 0; j < obj_hdr.dwAnimatedMeshKeyNum; j++) {
            ANIMATED_MESH_KEY mk;
            read_buf(f, &mk, sizeof(mk));
            if (j > 0) fprintf(fo, ",\n");
            fprintf(fo, "        { \"ticks\": %u, \"frame\": %u, \"vertexNum\": %u, \"texVertexNum\": %u }",
                    mk.dwTicks, mk.dwFrame, mk.dwVertexNum, mk.dwTexVertexNum);
            if (mk.dwVertexNum) fseek(f, mk.dwVertexNum * 12, SEEK_CUR);
            if (mk.dwTexVertexNum) fseek(f, mk.dwTexVertexNum * 8, SEEK_CUR);
        }
        fprintf(fo, "\n      ]\n");

        fprintf(fo, "    }");
        long actual = ftell(f);
        if (actual < obj_end) fseek(f, obj_end, SEEK_SET);
    }

    fprintf(fo, "\n  ]\n}\n");

    fclose(f);
    fclose(fo);

    const char* name = strrchr(in_path, '/');
    name = name ? name + 1 : in_path;
    fprintf(stdout, "%s: %u objects -> OK\n", name, hdr.dwMotionObjectNum);
    return 0;
}
