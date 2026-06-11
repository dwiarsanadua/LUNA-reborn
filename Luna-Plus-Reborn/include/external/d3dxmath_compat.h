#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <cstdint>
#include <cmath>

// D3DXMath → glm compatibility header
// Provides backward-compatible types and functions for code ported from Direct3D 9

// ─── Type Aliases ───────────────────────────────────────────
using D3DXVECTOR2 = glm::vec2;
using D3DXVECTOR3 = glm::vec3;
using D3DXVECTOR4 = glm::vec4;
using D3DXMATRIX = glm::mat4;
using D3DXQUATERNION = glm::quat;

// ─── Color ──────────────────────────────────────────────────
inline D3DXVECTOR4 D3DXCOLORtoFLOAT(uint32_t color) {
    return D3DXVECTOR4(
        ((color >> 16) & 0xFF) / 255.0f,
        ((color >> 8) & 0xFF) / 255.0f,
        (color & 0xFF) / 255.0f,
        ((color >> 24) & 0xFF) / 255.0f
    );
}

// ─── Matrix Functions ──────────────────────────────────────
inline D3DXMATRIX D3DXMatrixIdentity() {
    return glm::mat4(1.0f);
}

inline D3DXMATRIX D3DXMatrixTranslation(float x, float y, float z) {
    return glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
}

inline D3DXMATRIX D3DXMatrixScaling(float sx, float sy, float sz) {
    return glm::scale(glm::mat4(1.0f), glm::vec3(sx, sy, sz));
}

inline D3DXMATRIX D3DXMatrixRotationX(float angle) {
    return glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1, 0, 0));
}

inline D3DXMATRIX D3DXMatrixRotationY(float angle) {
    return glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
}

inline D3DXMATRIX D3DXMatrixRotationZ(float angle) {
    return glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 0, 1));
}

inline D3DXMATRIX D3DXMatrixRotationYawPitchRoll(float yaw, float pitch, float roll) {
    return glm::yawPitchRoll(yaw, pitch, roll);
}

inline D3DXMATRIX D3DXMatrixLookAtLH(const D3DXVECTOR3& eye, const D3DXVECTOR3& at, const D3DXVECTOR3& up) {
    return glm::lookAtLH(eye, at, up);
}

inline D3DXMATRIX D3DXMatrixPerspectiveFovLH(float fovY, float aspect, float zn, float zf) {
    return glm::perspectiveLH(fovY, aspect, zn, zf);
}

inline D3DXMATRIX D3DXMatrixOrthoLH(float w, float h, float zn, float zf) {
    return glm::orthoLH(0.0f, w, h, 0.0f, zn, zf);
}

inline D3DXMATRIX D3DXMatrixMultiply(const D3DXMATRIX& a, const D3DXMATRIX& b) {
    return a * b;
}

inline D3DXMATRIX D3DXMatrixInverse(const D3DXMATRIX& m) {
    return glm::inverse(m);
}

inline D3DXMATRIX D3DXMatrixTranspose(const D3DXMATRIX& m) {
    return glm::transpose(m);
}

// ─── Vector Functions ──────────────────────────────────────
inline D3DXVECTOR3 D3DXVec3Normalize(const D3DXVECTOR3& v) {
    return glm::normalize(v);
}

inline float D3DXVec3Dot(const D3DXVECTOR3& a, const D3DXVECTOR3& b) {
    return glm::dot(a, b);
}

inline D3DXVECTOR3 D3DXVec3Cross(const D3DXVECTOR3& a, const D3DXVECTOR3& b) {
    return glm::cross(a, b);
}

inline float D3DXVec3Length(const D3DXVECTOR3& v) {
    return glm::length(v);
}

inline float D3DXVec3LengthSq(const D3DXVECTOR3& v) {
    return glm::length2(v);
}

inline D3DXVECTOR3 D3DXVec3Lerp(const D3DXVECTOR3& a, const D3DXVECTOR3& b, float t) {
    return glm::lerp(a, b, t);
}

// ─── Quaternion Functions ───────────────────────────────────
inline D3DXQUATERNION D3DXQuaternionRotationYawPitchRoll(float yaw, float pitch, float roll) {
    return glm::quat(glm::vec3(pitch, yaw, roll));
}

inline D3DXMATRIX D3DXMatrixRotationQuaternion(const D3DXQUATERNION& q) {
    return glm::mat4_cast(q);
}

// ─── Plane ──────────────────────────────────────────────────
struct D3DXPLANE {
    float a, b, c, d;
};

inline float D3DXPlaneDot(const D3DXPLANE& p, const D3DXVECTOR4& v) {
    return p.a * v.x + p.b * v.y + p.c * v.z + p.d * v.w;
}

// ─── Viewport ───────────────────────────────────────────────
struct D3DVIEWPORT9 {
    uint32_t X, Y;
    uint32_t Width, Height;
    float MinZ, MaxZ;
};
