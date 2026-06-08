#include "RenderQueue.hpp"

void RenderQueue::Clear() {
    items_.clear();
}

void RenderQueue::Push(const RenderItem& item) {
    items_.push_back(item);
}

void RenderQueue::Sort() {
    std::sort(items_.begin(), items_.end(),
        [](const RenderItem& a, const RenderItem& b) {
            return a.sort_key < b.sort_key;
        });
}

void RenderQueue::SortByDepth() {
    std::sort(items_.begin(), items_.end(),
        [](const RenderItem& a, const RenderItem& b) {
            return a.depth < b.depth;
        });
}

void RenderQueue::SubmitAll(bgfx::ViewId view_id, uint64_t state) {
    for (auto& item : items_) {
        bgfx::setTransform(glm::value_ptr(item.transform));
        bgfx::setVertexBuffer(0, item.vb);
        bgfx::setIndexBuffer(item.ib);
        bgfx::setState(state);
        bgfx::submit(view_id, item.program);
    }
    Clear();
}
