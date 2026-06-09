#include "PhysicsWorld.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayerPairFilterTable.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <spdlog/spdlog.h>
#include <memory>

// Provide AssertFailed stub required by Jolt when asserts are enabled
#ifdef JPH_ENABLE_ASSERTS
namespace JPH {
    AssertFailedFunction AssertFailed = [](const char*, const char*, const char*, uint) -> bool { return true; };
}
#endif

JPH_SUPPRESS_WARNINGS

namespace {

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
public:
    BPLayerInterfaceImpl() { }
    uint GetNumBroadPhaseLayers() const override { return 1; }
    JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer) const override {
        return JPH::BroadPhaseLayer(0);
    }
};

class ObjectVsBPLayerFilterImpl final : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
    bool ShouldCollide(JPH::ObjectLayer, JPH::BroadPhaseLayer) const override {
        return true;
    }
};

class ObjectLayerPairFilterImpl final : public JPH::ObjectLayerPairFilter {
public:
    bool ShouldCollide(JPH::ObjectLayer, JPH::ObjectLayer) const override {
        return true;
    }
};

static BPLayerInterfaceImpl          s_bp_layer_interface;
static ObjectVsBPLayerFilterImpl     s_object_vs_bp;
static ObjectLayerPairFilterImpl     s_object_pair_filter;

} // anonymous namespace

static JPH::TempAllocator*          g_temp_allocator = nullptr;
static JPH::JobSystemThreadPool*    g_job_system = nullptr;
static JPH::PhysicsSystem*          g_physics = nullptr;
static JPH::BodyInterface*          g_body_interface = nullptr;
static const JPH::NarrowPhaseQuery* g_narrow_phase = nullptr;

static constexpr float c_character_height_stand = 1.8f;
static constexpr float c_character_radius = 0.3f;

bool PhysicsWorld::Initialize() {
    if (initialized_) return true;

    JPH::RegisterDefaultAllocator();

    if (!g_temp_allocator)
        g_temp_allocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
    if (!g_job_system)
        g_job_system = new JPH::JobSystemThreadPool(
            JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers,
            std::thread::hardware_concurrency() - 1);

    if (!g_physics) {
        g_physics = new JPH::PhysicsSystem();
        g_physics->Init(1024, 0, 1024, 1024,
                        s_bp_layer_interface,
                        s_object_vs_bp,
                        s_object_pair_filter);
        g_physics->SetGravity(JPH::Vec3(0, -9.81f, 0));

        JPH::BodyCreationSettings ground_settings(
            new JPH::BoxShape(JPH::Vec3(5000.0f, 1.0f, 5000.0f)),
            JPH::Vec3(0, -1.0f, 0),
            JPH::Quat::sIdentity(),
            JPH::EMotionType::Static,
            JPH::ObjectLayer(0)
        );
        JPH::BodyID ground = g_physics->GetBodyInterface().CreateAndAddBody(
            ground_settings, JPH::EActivation::DontActivate);
        (void)ground;
    }

    g_body_interface = &g_physics->GetBodyInterface();
    g_narrow_phase = &g_physics->GetNarrowPhaseQuery();
    initialized_ = true;

    spdlog::info("PhysicsWorld: initialized (Jolt Physics)");
    return true;
}

void PhysicsWorld::Shutdown() {
    characters_.clear();
    if (g_physics) {
        delete g_physics;
        g_physics = nullptr;
    }
    if (g_job_system) {
        delete g_job_system;
        g_job_system = nullptr;
    }
    if (g_temp_allocator) {
        delete g_temp_allocator;
        g_temp_allocator = nullptr;
    }
    initialized_ = false;
    spdlog::info("PhysicsWorld: shutdown");
}

void PhysicsWorld::Update(float delta_time) {
    if (!g_physics) return;
    const int c_collision_steps = 1;
    g_physics->Update(delta_time, c_collision_steps,
                      g_temp_allocator, g_job_system);
}

int PhysicsWorld::CreateCharacter(glm::vec3 position, float radius, float height) {
    Character ch;
    ch.position = position;
    ch.radius = radius;
    ch.height = height;
    ch.active = true;

    int id = (int)characters_.size();
    characters_.push_back(ch);
    return id;
}

void PhysicsWorld::RemoveCharacter(int id) {
    if (id >= 0 && id < (int)characters_.size()) {
        characters_[id].active = false;
    }
}

void PhysicsWorld::SetCharacterPosition(int id, glm::vec3 pos) {
    if (id >= 0 && id < (int)characters_.size()) {
        characters_[id].position = pos;
    }
}

glm::vec3 PhysicsWorld::GetCharacterPosition(int id) const {
    if (id >= 0 && id < (int)characters_.size()) {
        return characters_[id].position;
    }
    return glm::vec3(0.0f);
}

bool PhysicsWorld::RayCast(glm::vec3 from, glm::vec3 to, glm::vec3& out_hit) const {
    if (!g_narrow_phase) return false;

    JPH::RRayCast ray;
    ray.mOrigin = JPH::Vec3(from.x, from.y, from.z);
    ray.mDirection = JPH::Vec3(to.x - from.x, to.y - from.y, to.z - from.z);

    JPH::ClosestHitCollisionCollector<JPH::CastRayCollector> collector;
    g_narrow_phase->CastRay(ray, JPH::RayCastSettings(), collector);

    if (collector.HadHit()) {
        float fraction = collector.mHit.mFraction;
        out_hit.x = from.x + (to.x - from.x) * fraction;
        out_hit.y = from.y + (to.y - from.y) * fraction;
        out_hit.z = from.z + (to.z - from.z) * fraction;
        return true;
    }

    out_hit = to;
    return false;
}

float PhysicsWorld::GetTerrainHeight(float x, float z) const {
    if (terrain_height_func_) {
        return terrain_height_func_(x, z);
    }
    return 0.0f;
}

// ============================================================
// Collision Detection (ported from Luna Old engine)
// ============================================================

static float Dot(const glm::vec3& a, const glm::vec3& b) {
    return glm::dot(a, b);
}

static glm::vec3 Cross(const glm::vec3& a, const glm::vec3& b) {
    return glm::cross(a, b);
}

static float Length(const glm::vec3& v) {
    return glm::length(v);
}

static glm::vec3 Normalized(const glm::vec3& v) {
    float len = Length(v);
    if (len < 1e-8f) return glm::vec3(0.0f);
    return v / len;
}

static void CalcPlaneEquation(glm::vec3& out_normal, float& out_d, const glm::vec3 tri[3]) {
    glm::vec3 e1 = tri[1] - tri[0];
    glm::vec3 e2 = tri[2] - tri[0];
    out_normal = Normalized(Cross(e1, e2));
    out_d = -Dot(out_normal, tri[0]);
}

static bool IsInsideTriangle(const glm::vec3 tri[3], const glm::vec3& point) {
    glm::vec3 e0 = tri[1] - tri[0];
    glm::vec3 e1 = tri[2] - tri[1];
    glm::vec3 e2 = tri[0] - tri[2];
    glm::vec3 v0 = point - tri[0];
    glm::vec3 v1 = point - tri[1];
    glm::vec3 v2 = point - tri[2];

    glm::vec3 c0 = Cross(e0, v0);
    glm::vec3 c1 = Cross(e1, v1);
    glm::vec3 c2 = Cross(e2, v2);

    float d0 = Dot(c0, c1);
    float d1 = Dot(c1, c2);
    return (d0 >= 0.0f && d1 >= 0.0f) || (d0 <= 0.0f && d1 <= 0.0f);
}

bool PhysicsWorld::TestMovingSphereMeetPlane(
    glm::vec3& out_where_meet, float& out_t,
    const MovingSphere& sphere, const glm::vec3 triangle[3])
{
    glm::vec3 normal;
    float d;
    CalcPlaneEquation(normal, d, triangle);

    float plane_dot_vel = Dot(normal, sphere.velocity);
    if (plane_dot_vel >= -0.0001f) return false;

    float det = Dot(normal, sphere.from) + d;
    if (det < 0.0f) return false;

    float t = Dot(normal, sphere.from);
    t = sphere.radius - (t + d);
    t = t / plane_dot_vel;

    if (t > 1.0f || t < -1.0f) return false;

    glm::vec3 x = sphere.from + sphere.velocity * t;
    glm::vec3 meet_pos = x + normal * (-sphere.radius);

    if (!IsInsideTriangle(triangle, meet_pos)) return false;

    if (t < 0.0f) {
        float temp = Dot(sphere.from, normal) + d;
        if (temp <= 0.0f) return false;
    }

    out_t = t;
    out_where_meet = meet_pos;
    return true;
}

bool PhysicsWorld::TestMovingSphereMeetLine(
    glm::vec3& out_where_meet, float& out_t,
    const MovingSphere& sphere,
    const glm::vec3& line_from, const glm::vec3& line_to)
{
    glm::vec3 a = line_from;
    glm::vec3 b = line_to - line_from;
    if (Length(b) == 0.0f) return false;

    glm::vec3 c = sphere.from;
    glm::vec3 d = sphere.velocity;

    if (Length(b) == 0.0f || Length(d) == 0.0f) return false;

    glm::vec3 nb = Normalized(b);
    glm::vec3 nd = Normalized(d);
    float fv = Length(nb - nd);
    if (fv <= 0.0001f || fv >= 1.9999f) return false;

    float dot_bb = Dot(b, b);
    float temp = (Dot(b, c) - Dot(a, b)) / dot_bb;

    glm::vec3 e = b * temp - c + a;
    glm::vec3 f = b * (Dot(b, d) / dot_bb) - d;

    float aa = Dot(f, f);
    float bb_val = 2.0f * Dot(e, f);
    float cc = Dot(e, e) - sphere.radius * sphere.radius;

    float det = bb_val * bb_val - 4.0f * aa * cc;
    if (det < -0.001f) return false;
    if (det <= 0.0f) return false;

    float t = (-bb_val - sqrtf(det)) / (2.0f * aa);
    if (t > 1.0f) return false;

    glm::vec3 p = c + d * t;
    float s = (Dot(b, c) - Dot(a, b) + Dot(b, d) * t) / dot_bb;
    if (s < 0.0f || s > 1.0f) return false;

    glm::vec3 m = a + b * s;

    if (t < 0.0f) {
        glm::vec3 temp1 = m - sphere.from;
        float temp3 = Dot(temp1, sphere.velocity);
        if (temp3 < 0.00001f) return false;
    }

    glm::vec3 when_meet = m - p;
    float f_dot_when = Dot(when_meet, sphere.velocity);
    if (f_dot_when <= 0.0f) return false;

    out_where_meet = m;
    out_t = t;
    return true;
}

bool PhysicsWorld::TestMovingSphereMeetTriEdge(
    glm::vec3& out_where_meet, float& out_t,
    const MovingSphere& sphere, const glm::vec3 triangle[3])
{
    bool found = false;
    float last_meet_t = 1.0f;
    glm::vec3 last_where_meet{0.0f};

    for (int i = 0; i < 3; i++) {
        glm::vec3 meet;
        float t;
        bool hit = TestMovingSphereMeetLine(meet, t, sphere,
            triangle[i], triangle[(i + 1) % 3]);
        if (hit && t < last_meet_t) {
            found = true;
            last_meet_t = t;
            last_where_meet = meet;
        }
    }

    if (!found) return false;

    out_t = last_meet_t;
    out_where_meet = last_where_meet;
    return true;
}

bool PhysicsWorld::TestMovingSphereMeetVertex(
    glm::vec3& out_where_meet, float& out_t,
    const MovingSphere& sphere, const glm::vec3& vertex)
{
    glm::vec3 a = sphere.from;
    glm::vec3 b = sphere.velocity;
    glm::vec3 p = vertex;

    glm::vec3 pa = a - p;

    float aa = Dot(b, b);
    float bb_val = 2.0f * Dot(b, pa);
    float cc = Dot(pa, pa) - sphere.radius * sphere.radius;

    float det = bb_val * bb_val - 4.0f * aa * cc;
    if (det <= 0.00001f) return false;

    float t = (-bb_val - sqrtf(det)) / (2.0f * aa);
    if (t > 1.0f) return false;

    if (t < 0.0f) {
        glm::vec3 vtemp = sphere.from - p;
        float ftemp = Length(vtemp);
        if (ftemp >= sphere.radius) return false;

        glm::vec3 vel_normal = Normalized(sphere.velocity);
        float fd = -Dot(vel_normal, sphere.from);
        float fcheck = Dot(vel_normal, vertex) + fd;
        if (fcheck <= 0.0f) return false;
    }

    out_t = t;
    out_where_meet = p;
    return true;
}

bool PhysicsWorld::CollisionTestMovingSphereMeetTriangle(
    uint32_t& out_component_type,
    glm::vec3& out_where_meet,
    float& out_t,
    const MovingSphere& in_sphere,
    const glm::vec3 in_triangle[3])
{
    glm::vec3 normal;
    float d;
    CalcPlaneEquation(normal, d, in_triangle);

    float f_dot = Dot(normal, in_sphere.velocity);
    if (f_dot >= 0.0f) return false;

    float original_radius = in_sphere.radius;
    const float kUnitFactor = 100.0f;
    float scale_factor = kUnitFactor / original_radius;

    MovingSphere sphere = in_sphere;
    glm::vec3 tri[3] = { in_triangle[0], in_triangle[1], in_triangle[2] };
    bool scaled = false;

    if (original_radius != kUnitFactor) {
        scaled = true;
        sphere.radius *= scale_factor;
        sphere.from *= scale_factor;
        sphere.velocity *= scale_factor;
        tri[0] *= scale_factor;
        tri[1] *= scale_factor;
        tri[2] *= scale_factor;
    }

    bool found = false;
    out_t = 1.1f;
    uint32_t last_component = 0xffffffff;

    sphere.radius -= 10.0f;
    float edge_t;
    glm::vec3 edge_meet;
    bool edge_hit = TestMovingSphereMeetTriEdge(edge_meet, edge_t, sphere, tri);
    if (edge_hit && out_t > edge_t) {
        found = true;
        out_t = edge_t;
        out_where_meet = edge_meet;
        last_component = 10;
    }
    sphere.radius += 10.0f;

    float plane_t;
    glm::vec3 plane_meet;
    bool plane_hit = TestMovingSphereMeetPlane(plane_meet, plane_t, sphere, tri);
    if (plane_hit && out_t > plane_t) {
        found = true;
        out_t = plane_t;
        out_where_meet = plane_meet;
        last_component = 0;
    }

    for (int i = 0; i < 3; i++) {
        float vertex_t;
        glm::vec3 vertex_meet;
        bool vertex_hit = TestMovingSphereMeetVertex(vertex_meet, vertex_t, sphere, tri[i]);
        if (vertex_hit && out_t > vertex_t) {
            found = true;
            out_t = vertex_t;
            out_where_meet = vertex_meet;
            last_component = 20 + static_cast<uint32_t>(i);
        }
    }

    if (!found) return false;

    glm::vec3 meet_pivot = sphere.from + sphere.velocity * out_t;
    glm::vec3 plane_normal = Normalized(meet_pivot - out_where_meet);
    float f_test = Dot(plane_normal, sphere.velocity);
    if (f_test > -0.001f) return false;

    if (scaled) {
        float rescale = original_radius / kUnitFactor;
        out_where_meet *= rescale;
    }

    glm::vec3 meet_pivot2 = in_sphere.from + in_sphere.velocity * out_t;
    glm::vec3 plane_normal2 = Normalized(meet_pivot2 - out_where_meet);
    float f_test2 = Dot(plane_normal2, in_sphere.velocity);
    (void)f_test2;

    out_component_type = last_component;
    return true;
}

bool PhysicsWorld::CalculatePlaneOnEllipsoidWithVertex(
    glm::vec3& out_normal, float& out_d,
    const glm::vec3& ellipsoid_center, float width, float height,
    const glm::vec3& vertex)
{
    float x1xc = vertex.x - ellipsoid_center.x;
    float y1yc = vertex.y - ellipsoid_center.y;
    float z1zc = vertex.z - ellipsoid_center.z;

    float aa = width * width;
    float bb = height * height;
    float cc = aa;

    glm::vec3 n;
    n.x = x1xc / aa;
    n.y = y1yc / bb;
    n.z = z1zc / cc;
    float dd = -1.0f * (x1xc * ellipsoid_center.x / aa)
               - (y1yc * ellipsoid_center.y / bb)
               - (z1zc * ellipsoid_center.z / cc) - 1.0f;

    float plane_len = sqrtf(n.x * n.x + n.y * n.y + n.z * n.z);
    if (plane_len < 1e-8f) return false;

    n = (n / plane_len) * -1.0f;
    dd = (dd / plane_len) * -1.0f;

    out_normal = n;
    out_d = dd;
    return true;
}

bool PhysicsWorld::CollisionTestMovingEllipsoidMeetTriangle(
    CollisionResult& out_result,
    const MovingEllipsoid& ellipsoid,
    const glm::vec3 triangle[3])
{
    float velocity_len = Length(ellipsoid.velocity);
    if (velocity_len == 0.0f) return false;

    const float kFactorUnit = 100.0f;
    float fx_factor = ellipsoid.width / kFactorUnit;
    float fy_factor = ellipsoid.height / kFactorUnit;
    float fz_factor = fx_factor;

    glm::vec3 st[3];
    for (int i = 0; i < 3; i++) {
        st[i].x = triangle[i].x / fx_factor;
        st[i].y = triangle[i].y / fy_factor;
        st[i].z = triangle[i].z / fz_factor;
    }

    MovingSphere sphere;
    sphere.radius = kFactorUnit;
    sphere.from.x = ellipsoid.from.x / fx_factor;
    sphere.from.y = ellipsoid.from.y / fy_factor;
    sphere.from.z = ellipsoid.from.z / fz_factor;
    sphere.velocity.x = ellipsoid.velocity.x / fx_factor;
    sphere.velocity.y = ellipsoid.velocity.y / fy_factor;
    sphere.velocity.z = ellipsoid.velocity.z / fz_factor;

    uint32_t last_component;
    glm::vec3 where_meet;
    float meet_t;
    bool found = CollisionTestMovingSphereMeetTriangle(
        last_component, where_meet, meet_t, sphere, st);

    if (!found) return false;

    out_result.meet_time = meet_t;
    out_result.meet_pivot = ellipsoid.from + ellipsoid.velocity * meet_t;
    out_result.where_meet.x = where_meet.x * fx_factor;
    out_result.where_meet.y = where_meet.y * fy_factor;
    out_result.where_meet.z = where_meet.z * fz_factor;
    out_result.component_type = last_component;

    glm::vec3 ellipsoid_center_at_meet = out_result.meet_pivot;
    glm::vec3 plane_normal;
    float plane_d;
    bool plane_ok = CalculatePlaneOnEllipsoidWithVertex(
        plane_normal, plane_d,
        ellipsoid_center_at_meet,
        ellipsoid.width, ellipsoid.height,
        out_result.where_meet);
    if (!plane_ok) return false;

    out_result.meet_plane_normal = plane_normal;
    out_result.meet_plane_d = plane_d;

    float dot_plane_vel = Dot(plane_normal, ellipsoid.velocity);
    if (dot_plane_vel >= 0.0f) return false;

    float fp = Length(out_result.meet_pivot - ellipsoid.from);
    if (fp > velocity_len) return false;

    return true;
}

bool PhysicsWorld::AAEllipsoidCollisionTest(
    CollisionResult& out_result,
    const MovingEllipsoid& ellipsoid,
    const glm::vec3 triangle[3])
{
    return CollisionTestMovingEllipsoidMeetTriangle(out_result, ellipsoid, triangle);
}
