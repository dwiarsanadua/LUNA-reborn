/// Map Editor Tool — ImGui-based terrain & object editor with bgfx preview

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#else
#define GLFW_EXPOSE_NATIVE_X11
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD  // not actually used
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_bgfx.h>

// ── Simple JSON writer (no dependency) ─────────────────────────────────────

static std::string JsonEscape(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

// ── Scene data structures ──────────────────────────────────────────────────

struct TerrainVertex {
    float x, y, z;
    float nx, ny, nz;
    uint32_t color;
    float u, v;
};

struct PlacedObject {
    std::string type;       // "tree", "rock", "building", "npc", "monster"
    std::string mesh_path;
    glm::vec3 position{0};
    glm::vec3 rotation{0};
    float scale = 1.0f;
    int spawn_count = 1;
    float spawn_radius = 10.0f;
    int hp = 100;
    int level = 1;
    bool is_respawn = true;
    float respawn_time = 30.0f;
    std::string ai_type = "passive";
    std::string dialogue;
};

struct SceneData {
    std::string name = "Untitled";
    std::string description;
    glm::vec3 terrain_size{200, 20, 200};
    int terrain_segments = 64;

    // Terrain heightmap (flat by default)
    std::vector<float> heights;
    float height_scale = 8.0f;

    // Objects
    std::vector<PlacedObject> objects;

    // Terrain GPU resources (for preview)
    bgfx::VertexBufferHandle terrain_vb = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle terrain_ib = BGFX_INVALID_HANDLE;
    int terrain_indices = 0;
};

// ── Global state ───────────────────────────────────────────────────────────

static GLFWwindow* s_window = nullptr;
static int s_width = 1600, s_height = 900;
static bgfx::ViewId s_view_id = 0;
static bgfx::ProgramHandle s_prog = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle s_tex_color = BGFX_INVALID_HANDLE;
static bgfx::TextureHandle s_white_tex = BGFX_INVALID_HANDLE;
static SceneData s_scene;
static int s_selected_object = -1;
static glm::vec3 s_cam_pos{0, 30, 80};
static glm::vec3 s_cam_target{0, 0, 0};
static float s_cam_yaw = 0, s_cam_pitch = -20;
static float s_cam_dist = 80;
static bool s_dragging = false;
static double s_last_mx = 0, s_last_my = 0;
static char s_load_path[256] = "";
static char s_save_path[256] = "scene.json";
static bool s_show_grid = true;
static bool s_show_objects = true;

// ── Forward declarations ───────────────────────────────────────────────────

static bool InitBgfx(GLFWwindow* window);
static void ShutdownBgfx();
static void RebuildTerrainMesh();
static void RenderScene(const glm::mat4& view, const glm::mat4& proj);
static void SaveScene(const std::string& path);
static void LoadScene(const std::string& path);
static bool LoadShader(const char* name, bgfx::ProgramHandle& out_prog);

// ── Helpers ────────────────────────────────────────────────────────────────

static const bgfx::Memory* LoadShaderData(const char* path) {
    std::string search[] = {
        std::string("build/bin/") + path,
        std::string("bin/") + path,
        path,
        std::string("../") + path,
        std::string("../../") + path,
    };
    for (auto& p : search) {
        std::ifstream f(p, std::ios::binary | std::ios::ate);
        if (f) {
            size_t sz = f.tellg();
            f.seekg(0);
            auto* mem = bgfx::alloc((uint32_t)sz);
            f.read((char*)mem->data, sz);
            return mem;
        }
    }
    return nullptr;
}

static bool LoadShader(const char* name, bgfx::ProgramHandle& out_prog) {
    std::string vs_name = std::string("shaders/") + name + ".vs.bin";
    std::string fs_name = std::string("shaders/") + name + ".fs.bin";
    auto vs = LoadShaderData(vs_name.c_str());
    auto fs = LoadShaderData(fs_name.c_str());
    if (!vs || !fs) return false;
    out_prog = bgfx::createProgram(bgfx::createShader(vs), bgfx::createShader(fs), true);
    return bgfx::isValid(out_prog);
}

// ── Main ImGui UI ─────────────────────────────────────────────────────────

static void ShowMainMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene")) {
                s_scene = SceneData{};
                s_scene.name = "Untitled";
                s_selected_object = -1;
                RebuildTerrainMesh();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Open...")) { ImGui::OpenPopup("Open Scene"); }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                if (strlen(s_save_path) > 0) SaveScene(s_save_path);
            }
            if (ImGui::MenuItem("Save As...")) { ImGui::OpenPopup("Save Scene"); }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) { glfwSetWindowShouldClose(s_window, 1); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Show Grid", nullptr, &s_show_grid);
            ImGui::MenuItem("Show Objects", nullptr, &s_show_objects);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Open popup
    if (ImGui::BeginPopupModal("Open Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Path", s_load_path, sizeof(s_load_path));
        if (ImGui::Button("Open")) {
            LoadScene(s_load_path);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Save Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Path", s_save_path, sizeof(s_save_path));
        if (ImGui::Button("Save")) {
            SaveScene(s_save_path);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

static void ShowTerrainPanel() {
    if (ImGui::Begin("Terrain Editor")) {
        ImGui::Text("Terrain Settings");
        ImGui::Separator();

        bool rebuild = false;
        rebuild |= ImGui::SliderFloat("Size X", &s_scene.terrain_size.x, 50, 500);
        rebuild |= ImGui::SliderFloat("Size Z", &s_scene.terrain_size.z, 50, 500);
        rebuild |= ImGui::SliderFloat("Height Scale", &s_scene.height_scale, 1, 50);
        rebuild |= ImGui::SliderInt("Segments", &s_scene.terrain_segments, 8, 256);

        if (rebuild) RebuildTerrainMesh();

        ImGui::Separator();
        ImGui::Text("Height Editing");
        ImGui::Text("(Click on terrain in preview to raise/lower)");

        static float brush_size = 10.0f;
        static float brush_strength = 1.0f;
        ImGui::SliderFloat("Brush Size", &brush_size, 1, 50);
        ImGui::SliderFloat("Strength", &brush_strength, 0.1f, 5.0f);

        if (ImGui::Button("Flatten Terrain")) {
            std::fill(s_scene.heights.begin(), s_scene.heights.end(), 0.0f);
            RebuildTerrainMesh();
        }
        ImGui::SameLine();
        if (ImGui::Button("Randomize")) {
            for (auto& h : s_scene.heights) {
                h = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
            }
            RebuildTerrainMesh();
        }
    }
    ImGui::End();
}

static void ShowObjectPanel() {
    if (ImGui::Begin("Object Palette")) {
        ImGui::Text("Placement Tools");
        ImGui::Separator();

        static char mesh_path[256] = "assets/objects/tree.obj";
        static int spawn_count = 1;
        static float spawn_radius = 10.0f;
        static float obj_scale = 1.0f;
        static int hp = 100;
        static int level = 1;
        static bool respawn = true;
        static float respawn_time = 30.0f;
        static char ai_type[64] = "passive";

        const char* types[] = { "tree", "rock", "building", "npc", "monster" };
        static int current_type = 0;

        ImGui::Combo("Type", &current_type, types, IM_ARRAYSIZE(types));
        ImGui::InputText("Mesh Path", mesh_path, sizeof(mesh_path));
        ImGui::SliderFloat("Scale", &obj_scale, 0.1f, 10.0f);

        ImGui::Separator();
        ImGui::Text("Spawn Config");
        ImGui::SliderInt("Count", &spawn_count, 1, 50);
        ImGui::SliderFloat("Radius", &spawn_radius, 1, 100);

        if (current_type == 3 || current_type == 4) {
            ImGui::Separator();
            ImGui::Text("NPC/Monster Config");
            ImGui::InputInt("HP", &hp);
            ImGui::InputInt("Level", &level);
            ImGui::Checkbox("Respawn", &respawn);
            ImGui::SliderFloat("Respawn Time", &respawn_time, 1, 300);
            ImGui::InputText("AI Type", ai_type, sizeof(ai_type));
        }

        if (ImGui::Button("Place at Origin")) {
            PlacedObject obj;
            obj.type = types[current_type];
            obj.mesh_path = mesh_path;
            obj.position = glm::vec3(0, 0, 0);
            obj.scale = obj_scale;
            obj.spawn_count = spawn_count;
            obj.spawn_radius = spawn_radius;
            obj.hp = hp;
            obj.level = level;
            obj.is_respawn = respawn;
            obj.respawn_time = respawn_time;
            obj.ai_type = ai_type;
            s_scene.objects.push_back(obj);
            spdlog::info("MapEditor: placed {} at origin", obj.type);
        }

        ImGui::Separator();
        ImGui::Text("Object List (%zu)", s_scene.objects.size());
        ImGui::BeginChild("ObjList", ImVec2(0, 200), true);

        for (size_t i = 0; i < s_scene.objects.size(); i++) {
            auto& obj = s_scene.objects[i];
            bool selected = (s_selected_object == (int)i);
            char label[128];
            snprintf(label, sizeof(label), "%s##%zu", obj.type.c_str(), i);
            if (ImGui::Selectable(label, &selected)) {
                s_selected_object = (int)i;
            }
        }

        ImGui::EndChild();

        if (s_selected_object >= 0 && s_selected_object < (int)s_scene.objects.size()) {
            auto& obj = s_scene.objects[s_selected_object];
            ImGui::Separator();
            ImGui::Text("Selected: %s", obj.type.c_str());
            ImGui::DragFloat3("Position", glm::value_ptr(obj.position), 0.5f);
            ImGui::DragFloat3("Rotation", glm::value_ptr(obj.rotation), 0.1f);
            ImGui::DragFloat("Scale", &obj.scale, 0.1f);

            if (ImGui::Button("Delete")) {
                s_scene.objects.erase(s_scene.objects.begin() + s_selected_object);
                s_selected_object = -1;
            }
            ImGui::SameLine();
            if (ImGui::Button("Duplicate")) {
                s_scene.objects.push_back(obj);
                s_scene.objects.back().position.x += 5;
            }
        }
    }
    ImGui::End();
}

// ── Terrain mesh generation ────────────────────────────────────────────────

static float GetTerrainHeight(const SceneData& scene, int ix, int iz) {
    int segs = scene.terrain_segments;
    if (scene.heights.empty()) return 0;
    if (ix < 0 || ix > segs || iz < 0 || iz > segs) return 0;
    return scene.heights[iz * (segs + 1) + ix] * scene.height_scale;
}

static void RebuildTerrainMesh() {
    // Destroy old
    if (bgfx::isValid(s_scene.terrain_vb)) bgfx::destroy(s_scene.terrain_vb);
    if (bgfx::isValid(s_scene.terrain_ib)) bgfx::destroy(s_scene.terrain_ib);
    s_scene.terrain_vb = BGFX_INVALID_HANDLE;
    s_scene.terrain_ib = BGFX_INVALID_HANDLE;

    int segs = s_scene.terrain_segments;
    int vcount = (segs + 1) * (segs + 1);
    int icount = segs * segs * 6;

    // Initialize heights if empty
    if (s_scene.heights.empty()) {
        s_scene.heights.resize(vcount, 0);
    }

    std::vector<TerrainVertex> verts(vcount);
    std::vector<uint16_t> idx(icount);

    float half_x = s_scene.terrain_size.x * 0.5f;
    float half_z = s_scene.terrain_size.z * 0.5f;

    for (int iz = 0; iz <= segs; iz++) {
        for (int ix = 0; ix <= segs; ix++) {
            int vi = iz * (segs + 1) + ix;
            float wx = -half_x + ix * s_scene.terrain_size.x / segs;
            float wz = -half_z + iz * s_scene.terrain_size.z / segs;
            float h = GetTerrainHeight(s_scene, ix, iz);

            // Simple normal from neighbors
            float hL = GetTerrainHeight(s_scene, ix - 1, iz);
            float hR = GetTerrainHeight(s_scene, ix + 1, iz);
            float hD = GetTerrainHeight(s_scene, ix, iz - 1);
            float hU = GetTerrainHeight(s_scene, ix, iz + 1);
            glm::vec3 n = glm::normalize(glm::vec3(hL - hR, 2.0f, hD - hU));

            // Height-based color
            float t = (h + s_scene.height_scale) / (s_scene.height_scale * 2);
            t = std::max(0.0f, std::min(1.0f, t));
            uint8_t r = (uint8_t)(50 + t * 150);
            uint8_t g = (uint8_t)(100 + t * 100);
            uint8_t b = (uint8_t)(30 + t * 50);
            uint32_t color = 0xff000000 | (b << 16) | (g << 8) | r;

            verts[vi] = {wx, h, wz, n.x, n.y, n.z, color, (float)ix / segs, (float)iz / segs};
        }
    }

    int ii = 0;
    for (int iz = 0; iz < segs; iz++) {
        for (int ix = 0; ix < segs; ix++) {
            int a = iz * (segs + 1) + ix;
            int b = a + 1;
            int c = (iz + 1) * (segs + 1) + ix;
            int d = c + 1;
            idx[ii++] = a; idx[ii++] = c; idx[ii++] = b;
            idx[ii++] = b; idx[ii++] = c; idx[ii++] = d;
        }
    }

    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    s_scene.terrain_vb = bgfx::createVertexBuffer(
        bgfx::copy(verts.data(), (uint32_t)(verts.size() * sizeof(TerrainVertex))), layout);
    s_scene.terrain_ib = bgfx::createIndexBuffer(
        bgfx::copy(idx.data(), (uint32_t)(idx.size() * sizeof(uint16_t))));
    s_scene.terrain_indices = (int)idx.size();

    spdlog::info("MapEditor: rebuilt terrain mesh ({} verts, {} indices)", vcount, icount);
}

// ── Scene JSON I/O ─────────────────────────────────────────────────────────

static void SaveScene(const std::string& path) {
    std::ofstream f(path);
    if (!f) { spdlog::error("MapEditor: cannot write {}", path); return; }

    f << "{\n";
    f << "  \"name\": \"" << JsonEscape(s_scene.name) << "\",\n";
    f << "  \"description\": \"" << JsonEscape(s_scene.description) << "\",\n";
    f << "  \"terrain\": {\n";
    f << "    \"size\": " << s_scene.terrain_size.x << ",\n";
    f << "    \"segments\": " << s_scene.terrain_segments << ",\n";
    f << "    \"height_scale\": " << s_scene.height_scale << ",\n";
    f << "    \"heights\": [";
    for (size_t i = 0; i < s_scene.heights.size(); i++) {
        if (i > 0) f << ",";
        f << s_scene.heights[i];
    }
    f << "]\n  },\n";
    f << "  \"objects\": [\n";
    for (size_t i = 0; i < s_scene.objects.size(); i++) {
        auto& obj = s_scene.objects[i];
        f << "    {\n";
        f << "      \"type\": \"" << JsonEscape(obj.type) << "\",\n";
        f << "      \"mesh_path\": \"" << JsonEscape(obj.mesh_path) << "\",\n";
        f << "      \"position\": [" << obj.position.x << "," << obj.position.y << "," << obj.position.z << "],\n";
        f << "      \"rotation\": [" << obj.rotation.x << "," << obj.rotation.y << "," << obj.rotation.z << "],\n";
        f << "      \"scale\": " << obj.scale << ",\n";
        f << "      \"spawn_count\": " << obj.spawn_count << ",\n";
        f << "      \"spawn_radius\": " << obj.spawn_radius << ",\n";
        f << "      \"hp\": " << obj.hp << ",\n";
        f << "      \"level\": " << obj.level << ",\n";
        f << "      \"is_respawn\": " << (obj.is_respawn ? "true" : "false") << ",\n";
        f << "      \"respawn_time\": " << obj.respawn_time << ",\n";
        f << "      \"ai_type\": \"" << JsonEscape(obj.ai_type) << "\"\n";
        f << "    }";
        if (i < s_scene.objects.size() - 1) f << ",";
        f << "\n";
    }
    f << "  ]\n";
    f << "}\n";
    spdlog::info("MapEditor: scene saved to {}", path);
}

static void LoadScene(const std::string& path) {
    std::ifstream f(path);
    if (!f) { spdlog::error("MapEditor: cannot read {}", path); return; }

    std::stringstream buf;
    buf << f.rdbuf();
    std::string json = buf.str();

    // Very basic JSON parsing — just extracts key fields
    s_scene = SceneData{};
    s_scene.name = "Untitled";

    // Extract name
    auto name_pos = json.find("\"name\"");
    if (name_pos != std::string::npos) {
        auto q1 = json.find('"', name_pos + 6);
        auto q2 = json.find('"', q1 + 1);
        if (q1 != std::string::npos && q2 != std::string::npos)
            s_scene.name = json.substr(q1 + 1, q2 - q1 - 1);
    }

    // Extract terrain size
    auto size_pos = json.find("\"size\"");
    if (size_pos != std::string::npos) {
        auto colon = json.find(':', size_pos);
        if (colon != std::string::npos)
            s_scene.terrain_size.x = (float)atof(json.c_str() + colon + 1);
    }

    auto seg_pos = json.find("\"segments\"");
    if (seg_pos != std::string::npos) {
        auto colon = json.find(':', seg_pos);
        if (colon != std::string::npos)
            s_scene.terrain_segments = atoi(json.c_str() + colon + 1);
    }

    auto hs_pos = json.find("\"height_scale\"");
    if (hs_pos != std::string::npos) {
        auto colon = json.find(':', hs_pos);
        if (colon != std::string::npos)
            s_scene.height_scale = (float)atof(json.c_str() + colon + 1);
    }

    // Extract heights array
    auto he_pos = json.find("\"heights\"");
    if (he_pos != std::string::npos) {
        auto br1 = json.find('[', he_pos);
        auto br2 = json.find(']', br1);
        if (br1 != std::string::npos && br2 != std::string::npos) {
            std::string hstr = json.substr(br1 + 1, br2 - br1 - 1);
            std::stringstream hs(hstr);
            float val;
            while (hs >> val) {
                s_scene.heights.push_back(val);
                if (hs.peek() == ',') hs.ignore();
            }
        }
    }

    RebuildTerrainMesh();
    spdlog::info("MapEditor: scene loaded from {}", path);
    strncpy(s_save_path, path.c_str(), sizeof(s_save_path) - 1);
}

// ── Bgfx initialization ───────────────────────────────────────────────────

static bool InitBgfx(GLFWwindow* window) {
    int w, h;
    glfwGetWindowSize(window, &w, &h);

    bgfx::PlatformData pd{};
#ifdef _WIN32
    pd.nwh = glfwGetWin32Window(window);
#elif defined(__APPLE__)
    pd.nwh = glfwGetCocoaWindow(window);
#else
    pd.ndt = glfwGetX11Display();
    pd.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
#endif

    bgfx::Init init;
    init.type = bgfx::RendererType::Count;
    init.vendorId = BGFX_PCI_ID_NONE;
    init.platformData = pd;
    bgfx::init(init);
    bgfx::reset((uint32_t)w, (uint32_t)h, BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, (uint16_t)w, (uint16_t)h);

    s_tex_color = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    uint32_t white = 0xffffffff;
    s_white_tex = bgfx::createTexture2D(1, 1, false, 1, bgfx::TextureFormat::RGBA8, 0, bgfx::makeRef(&white, 4));

    // Try to load the terrain shader
    LoadShader("vs_terrain", s_prog);
    if (!bgfx::isValid(s_prog)) {
        LoadShader("vs_default", s_prog);  // fallback
    }

    RebuildTerrainMesh();
    spdlog::info("MapEditor: bgfx initialized ({}x{})", w, h);
    return true;
}

static void ShutdownBgfx() {
    if (bgfx::isValid(s_scene.terrain_vb)) bgfx::destroy(s_scene.terrain_vb);
    if (bgfx::isValid(s_scene.terrain_ib)) bgfx::destroy(s_scene.terrain_ib);
    if (bgfx::isValid(s_prog)) bgfx::destroy(s_prog);
    if (bgfx::isValid(s_tex_color)) bgfx::destroy(s_tex_color);
    if (bgfx::isValid(s_white_tex)) bgfx::destroy(s_white_tex);
    bgfx::shutdown();
}

static void RenderScene(const glm::mat4& view, const glm::mat4& proj) {
    bgfx::setViewTransform(0, &view, &proj);
    bgfx::touch(0);

    if (!bgfx::isValid(s_prog)) return;

    // Render terrain
    if (bgfx::isValid(s_scene.terrain_vb)) {
        float mtx[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
        bgfx::setTransform(mtx);
        bgfx::setTexture(0, s_tex_color, s_white_tex);
        bgfx::setVertexBuffer(0, s_scene.terrain_vb);
        bgfx::setIndexBuffer(s_scene.terrain_ib);
        bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z);
        bgfx::submit(0, s_prog);
    }

    // Render objects as simple lines/spheres (placeholder)
    if (s_show_objects) {
        for (auto& obj : s_scene.objects) {
            // Draw a small cross at object position
            glm::mat4 m = glm::translate(glm::mat4(1.0f), obj.position);
            m = glm::scale(m, glm::vec3(2.0f));
            bgfx::setTransform(glm::value_ptr(m));
            // TODO: draw actual mesh when loaded
        }
    }
}

// ── Main ───────────────────────────────────────────────────────────────────

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::info);

    if (!glfwInit()) {
        fprintf(stderr, "ERROR: glfwInit failed\n");
        return 1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    s_window = glfwCreateWindow(s_width, s_height, "Luna Plus Reborn — Map Editor", nullptr, nullptr);
    if (!s_window) {
        fprintf(stderr, "ERROR: glfwCreateWindow failed\n");
        glfwTerminate();
        return 1;
    }

    glfwSetWindowPos(s_window, 100, 100);

    if (!InitBgfx(s_window)) {
        fprintf(stderr, "ERROR: bgfx init failed\n");
        glfwDestroyWindow(s_window);
        glfwTerminate();
        return 1;
    }

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(s_window, true);
    ImGui_ImplBgfx_Init();

    // Main loop
    while (!glfwWindowShouldClose(s_window)) {
        glfwPollEvents();

        // Camera orbit controls
        if (glfwGetMouseButton(s_window, GLFW_MOUSE_BUTTON_RIGHT)) {
            double mx, my;
            glfwGetCursorPos(s_window, &mx, &my);
            if (!s_dragging) {
                s_dragging = true;
                s_last_mx = mx;
                s_last_my = my;
            }
            double dx = mx - s_last_mx;
            double dy = my - s_last_my;
            s_cam_yaw += dx * 0.005f;
            s_cam_pitch += dy * 0.005f;
            s_cam_pitch = std::max(-89.0f, std::min(89.0f, s_cam_pitch));
            s_last_mx = mx;
            s_last_my = my;
        } else {
            s_dragging = false;
        }

        // Scroll to zoom
        static double scroll_y = 0;
        double new_scroll;
        glfwGetWindowSize(s_window, &s_width, &s_height);

        // Update camera position
        s_cam_pos.x = s_cam_target.x + s_cam_dist * cos(s_cam_pitch) * sin(s_cam_yaw);
        s_cam_pos.y = s_cam_target.y + s_cam_dist * sin(s_cam_pitch);
        s_cam_pos.z = s_cam_target.z + s_cam_dist * cos(s_cam_pitch) * cos(s_cam_yaw);

        glm::mat4 view = glm::lookAt(s_cam_pos, s_cam_target, glm::vec3(0, 1, 0));
        float aspect = (float)s_width / (float)s_height;
        glm::mat4 proj = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 1000.0f);

        // Bgfx frame
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, (uint16_t)s_width, (uint16_t)s_height);

        int old_w, old_h;
        glfwGetWindowSize(s_window, &old_w, &old_h);
        if (old_w != s_width || old_h != s_height) {
            bgfx::reset((uint32_t)old_w, (uint32_t)old_h, BGFX_RESET_VSYNC);
            s_width = old_w;
            s_height = old_h;
        }

        RenderScene(view, proj);

        // ImGui
        ImGui_ImplBgfx_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ShowMainMenu();
        ShowTerrainPanel();
        ShowObjectPanel();

        // Info overlay
        ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Camera: (%.1f, %.1f, %.1f)", s_cam_pos.x, s_cam_pos.y, s_cam_pos.z);
            ImGui::Text("Objects: %zu", s_scene.objects.size());
            ImGui::Text("Terrain Verts: %d", (s_scene.terrain_segments + 1) * (s_scene.terrain_segments + 1));
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplBgfx_RenderDrawLists(ImGui::GetDrawData());

        bgfx::frame();
    }

    // Cleanup
    ImGui_ImplBgfx_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ShutdownBgfx();
    glfwDestroyWindow(s_window);
    glfwTerminate();

    return 0;
}
