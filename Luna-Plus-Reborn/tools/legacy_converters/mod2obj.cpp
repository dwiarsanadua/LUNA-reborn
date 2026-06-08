#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>

#pragma pack(push, 4)

struct FILE_SCENE_HEADER {
    uint32_t dwVersion;
    uint32_t dwObjectNum;
    uint32_t dwMaterialNum;
    uint32_t dwMaxMeshObjectNum;
    uint32_t dwMaxLightObjectNum;
    uint32_t dwMaxCameraObjectNum;
    uint32_t dwMaxBoneObjectNum;
};

struct FILE_MATERIAL_HEADER {
    uint32_t dwTextureNum;
    uint32_t dwDiffuse;
    uint32_t dwAmbient;
    uint32_t dwSpecular;
    float    fTransparency;
    float    fShine;
    float    fShineStrength;
    char     szTexmapFileName[10][128];
    char     szMtlName[128];
    uint32_t dwIndex;
    uint32_t dwFlag;
};

struct FILE_BASE_OBJECT_HEADER {
    uint32_t dwIndex;
    float    fRotAng;
    float    fPosX, fPosY, fPosZ;
    float    fRotAxisX, fRotAxisY, fRotAxisZ;
    float    fScaleX, fScaleY, fScaleZ;
    float    fScaleAxisX, fScaleAxisY, fScaleAxisZ;
    float    fScaleAngle;
    float    mat4[16];
    float    mat4Inverse[16];
    uint32_t dwChildObjectNum;
    uint32_t dwParentObjectIndex;
    char     szObjName[128];
};

struct FILE_MESH_HEADER {
    uint32_t dwMaxVertexNum;
    uint32_t dwVertexNum;
    uint32_t dwOriginalVertexNum;
    uint32_t dwExtVertexNum;
    uint32_t dwTexVertexNum;
    uint32_t dwMtlIndex;
    uint32_t dwFaceGroupNum;
    uint32_t meshFlag[3];
    uint32_t dwGridIndex;
    float    v3Dir[3];
};

struct FILE_FACE_GROUP_HEADER {
    uint32_t dwMtlIndex;
    uint32_t dwIndex;
    uint32_t dwFacesNum;
    uint32_t dwMaxFacesNum;
    uint32_t dwVertexIndexNum;
    uint32_t dwLightUVNum1;
    uint32_t dwLightUVNum2;
};

#pragma pack(pop)

#define OBJECT_TYPE_MESH   0xf4000000u
#define OBJECT_TYPE_BONE   0xf5000000u
#define OBJECT_TYPE_MATERIAL 0x00f00000u

static inline bool is_degenerate(uint16_t a, uint16_t b, uint16_t c) {
    return a == b || b == c || a == c;
}

static std::string mtl_dir;

static uint32_t read_u32(FILE* f) {
    uint32_t v;
    fread(&v, 4, 1, f);
    return v;
}

static void read_buf(FILE* f, void* buf, size_t sz) {
    fread(buf, 1, sz, f);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.mod> [output.obj] [output.mtl]\n", argv[0]);
        return 1;
    }

    const char* in_path = argv[1];
    const char* obj_path = (argc > 2) ? argv[2] : nullptr;
    const char* mtl_path = (argc > 3) ? argv[3] : nullptr;

    FILE* f = fopen(in_path, "rb");
    if (!f) { fprintf(stderr, "Cannot open %s\n", in_path); return 1; }

    FILE_SCENE_HEADER hdr;
    read_buf(f, &hdr, sizeof(hdr));

    std::vector<std::string> mtl_names;
    mtl_names.reserve(hdr.dwMaterialNum);

    for (uint32_t i = 0; i < hdr.dwMaterialNum; i++) {
        uint32_t mtype = read_u32(f);
        uint32_t msize = read_u32(f);
        (void)mtype;

        FILE_MATERIAL_HEADER mhdr;
        read_buf(f, &mhdr, msize);

        std::string texname;
        if (mhdr.szTexmapFileName[0][0]) {
            texname = mhdr.szTexmapFileName[0];
            size_t pos = texname.find_last_of("\\/");
            if (pos != std::string::npos) texname = texname.substr(pos + 1);
            pos = texname.find_last_of(".");
            if (pos != std::string::npos) texname = texname.substr(0, pos);
        }
        if (texname.empty()) {
            char idx[32];
            snprintf(idx, sizeof(idx), "material_%u", mhdr.dwIndex);
            texname = idx;
        }
        mtl_names.push_back(texname);
    }

    std::vector<float> all_verts;
    std::vector<float> all_uvs;
    std::vector<int> all_faces; // 4 ints per face: a,b,c,mtl_idx

    for (uint32_t i = 0; i < hdr.dwObjectNum; i++) {
        uint32_t otype = read_u32(f);
        uint32_t osize = read_u32(f);
        long obj_end = ftell(f) + osize;

        if (otype == OBJECT_TYPE_MESH) {
            FILE_BASE_OBJECT_HEADER base;
            read_buf(f, &base, sizeof(base));

            // Read FILE_MESH_HEADER: 7 DWORDs + meshFlag(4) + gridIdx(4) + v3Dir(12) + reserved(4)
            uint32_t mhdr[7];
            read_buf(f, mhdr, 28);
            uint32_t vn = mhdr[1];
            uint32_t tex_vn = mhdr[4];
            uint32_t ext_vn = mhdr[3];
            uint32_t fg_num = mhdr[6];
            // Skip the rest of mesh header: meshFlag(4) + gridIdx(4) + v3Dir(12) + reserved(4)
            fseek(f, 24, SEEK_CUR);

            if (vn == 0 || vn > 100000) { fseek(f, obj_end, SEEK_SET); continue; }

            uint32_t base_v = (uint32_t)all_verts.size() / 3;

            for (uint32_t j = 0; j < vn; j++) {
                float xyz[3];
                read_buf(f, xyz, 12);
                all_verts.push_back(xyz[0]);
                all_verts.push_back(xyz[1]);
                all_verts.push_back(xyz[2]);
            }

            for (uint32_t j = 0; j < tex_vn; j++) {
                float uv[2];
                read_buf(f, uv, 8);
                all_uvs.push_back(uv[0]);
                all_uvs.push_back(1.0f - uv[1]);
            }
            while ((uint32_t)all_uvs.size() / 2 < base_v + vn) {
                all_uvs.push_back(0.0f);
                all_uvs.push_back(0.0f);
            }

            if (ext_vn) fseek(f, ext_vn * 4, SEEK_CUR);

            for (uint32_t fg = 0; fg < fg_num; fg++) {
                FILE_FACE_GROUP_HEADER fghdr;
                read_buf(f, &fghdr, sizeof(fghdr));

                uint32_t faces = fghdr.dwFacesNum;
                uint32_t fg_mtl = fghdr.dwMtlIndex;
                if (faces == 0 || faces > 100000) continue;

                for (uint32_t r = 0; r < faces; r++) {
                    uint16_t tri[3];
                    read_buf(f, tri, 6);
                    if (!is_degenerate(tri[0], tri[1], tri[2])) {
                        if (tri[0] < vn && tri[1] < vn && tri[2] < vn) {
                            all_faces.push_back(base_v + tri[0]);
                            all_faces.push_back(base_v + tri[1]);
                            all_faces.push_back(base_v + tri[2]);
                            all_faces.push_back(fg_mtl);
                        }
                    }
                }

                if (fghdr.dwLightUVNum1) fseek(f, fghdr.dwLightUVNum1 * 8, SEEK_CUR);
                if (fghdr.dwLightUVNum2) fseek(f, fghdr.dwLightUVNum2 * 8, SEEK_CUR);
            }
        }

        fseek(f, obj_end, SEEK_SET);
    }

    fclose(f);

    if (all_verts.empty() || all_faces.empty()) {
        fprintf(stderr, "No mesh data found\n");
        return 1;
    }

    char obj_fn[1024], mtl_fn[1024];
    if (!obj_path) {
        snprintf(obj_fn, sizeof(obj_fn), "%s.obj", in_path);
        obj_path = obj_fn;
    }
    if (!mtl_path) {
        snprintf(mtl_fn, sizeof(mtl_fn), "%s.obj.mtl", obj_path);
        mtl_path = mtl_fn;
    }

    FILE* f_obj = fopen(obj_path, "w");
    if (!f_obj) { fprintf(stderr, "Cannot write %s\n", obj_path); return 1; }

    FILE* f_mtl = fopen(mtl_path, "w");
    if (!f_mtl) { fprintf(stderr, "Cannot write %s\n", mtl_path); return 1; }

    const char* name = strrchr(in_path, '/');
    name = name ? name + 1 : in_path;
    char* dot = strrchr((char*)name, '.');
    if (dot) *dot = 0;

    fprintf(f_mtl, "# MTL generated from %s\n\n", in_path);
    fprintf(f_mtl, "newmtl %s_mtl\n", name);
    fprintf(f_mtl, "Ka 0.2 0.2 0.2\n");
    fprintf(f_mtl, "Kd 0.8 0.8 0.8\n");
    fprintf(f_mtl, "Ks 0.0 0.0 0.0\n");
    fprintf(f_mtl, "d 1.0\n");
    fprintf(f_mtl, "illum 2\n");

    for (size_t i = 0; i < mtl_names.size(); i++) {
        fprintf(f_mtl, "newmtl %s\n", mtl_names[i].c_str());
        fprintf(f_mtl, "Ka 0.2 0.2 0.2\n");
        fprintf(f_mtl, "Kd 0.8 0.8 0.8\n");
        fprintf(f_mtl, "Ks 0.0 0.0 0.0\n");
        fprintf(f_mtl, "d 1.0\n");
        fprintf(f_mtl, "illum 2\n");
        fprintf(f_mtl, "map_Kd %s.png\n\n", mtl_names[i].c_str());
    }

    fprintf(f_obj, "mtllib %s\n", strrchr(mtl_path, '/') ? strrchr(mtl_path, '/') + 1 : mtl_path);
    fprintf(f_obj, "o %s\n", name);

    uint32_t vert_count = (uint32_t)all_verts.size() / 3;
    uint32_t uv_count = (uint32_t)all_uvs.size() / 2;

    for (uint32_t i = 0; i < vert_count; i++) {
        fprintf(f_obj, "v %.6f %.6f %.6f\n", all_verts[i*3], all_verts[i*3+1], all_verts[i*3+2]);
    }
    for (uint32_t i = 0; i < uv_count; i++) {
        fprintf(f_obj, "vt %.6f %.6f\n", all_uvs[i*2], all_uvs[i*2+1]);
    }

    int cur_mtl = -1;
    int face_count = (int)all_faces.size() / 4;
    for (int i = 0; i < face_count; i++) {
        int a = all_faces[i*4];
        int b = all_faces[i*4+1];
        int c = all_faces[i*4+2];
        int mtl_idx = all_faces[i*4+3];
        if (mtl_idx != cur_mtl) {
            cur_mtl = mtl_idx;
            if (mtl_idx >= 0 && (size_t)mtl_idx < mtl_names.size()) {
                fprintf(f_obj, "usemtl %s\n", mtl_names[mtl_idx].c_str());
            } else {
                fprintf(f_obj, "usemtl %s_mtl\n", name);
            }
        }
        int a1 = a + 1, b1 = b + 1, c1 = c + 1;
        if (a < uv_count && b < uv_count && c < uv_count) {
            fprintf(f_obj, "f %d/%d %d/%d %d/%d\n", a1, a1, b1, b1, c1, c1);
        } else {
            fprintf(f_obj, "f %d %d %d\n", a1, b1, c1);
        }
    }

    fclose(f_obj);
    fclose(f_mtl);

    fprintf(stdout, "%s: %u verts, %d faces -> OK\n", name, vert_count, face_count);
    return 0;
}
