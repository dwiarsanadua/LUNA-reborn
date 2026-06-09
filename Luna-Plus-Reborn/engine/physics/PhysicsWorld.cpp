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
