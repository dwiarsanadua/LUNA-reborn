#pragma once
#include <cstdint>

#pragma pack(push, 4)

enum OBJECT_TYPE : uint32_t {
    OBJECT_TYPE_UNKNOWN       = 0x0f00000,
    OBJECT_TYPE_LIGHT         = 0xf1000000,
    OBJECT_TYPE_CAMERA        = 0xf2000000,
    OBJECT_TYPE_CAMERA_TARGET = 0xf3000000,
    OBJECT_TYPE_MESH          = 0xf4000000,
    OBJECT_TYPE_BONE          = 0xf5000000,
    OBJECT_TYPE_ILLUSION_MESH = 0xf6000000,
    OBJECT_TYPE_COLLISION_MESH= 0xf7000000,
    OBJECT_TYPE_MATERIAL      = 0x00f00000,
    OBJECT_TYPE_MOTION        = 0x0000f000,
};

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

#define FILE_BASE_OBJECT_HEADER_SIZE 324

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
    uint32_t meshFlag[3]; // CMeshFlag + padding
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

struct PAK_ENTRY {
    uint32_t total_size;
    uint32_t real_size;
    uint32_t name_len;
    uint32_t abs_offset;
    uint32_t flag;
    uint32_t time;
    uint32_t reserved[5];
};

#pragma pack(pop)
