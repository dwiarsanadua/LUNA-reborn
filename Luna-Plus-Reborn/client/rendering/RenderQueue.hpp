#pragma once
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <algorithm>
#include <cstring>

struct RenderItem {
    bgfx::ProgramHandle program;
    bgfx::VertexBufferHandle vb;
    bgfx::IndexBufferHandle ib;
    glm::mat4 transform;
    uint32_t sort_key = 0;
    float depth = 0.0f;
};

class RenderQueue {
public:
    void Clear();
    void Push(const RenderItem& item);
    void Sort();
    void SortByDepth();
    void SubmitAll(bgfx::ViewId view_id, uint64_t state = BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_Z);

    int GetCount() const { return (int)items_.size(); }

private:
    std::vector<RenderItem> items_;
};
