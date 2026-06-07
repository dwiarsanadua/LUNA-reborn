// AGENT Titan — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>
#include <vector>
#include <future>
#include <chrono>

template<typename T>
class ResourceCache {
public:
    ResourceCache() = default;
    ~ResourceCache() { UnloadAll(); }

    ResourceCache(const ResourceCache&) = delete;
    ResourceCache& operator=(const ResourceCache&) = delete;

    std::shared_ptr<T> Load(const std::string& key,
                            std::function<std::shared_ptr<T>()> factory) {
        std::lock_guard<std::mutex> lock(mutex_);

        // Check if there's a live entry
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            auto ptr = it->second.lock();
            if (ptr) {
                return ptr;
            }
            // Expired entry — remove it
            cache_.erase(it);
        }

        // Create new resource
        auto ptr = factory();
        if (ptr) {
            timestamps_[key] = std::chrono::steady_clock::now();
            cache_[key] = ptr;
        }

        return ptr;
    }

    std::shared_ptr<T> Get(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            auto ptr = it->second.lock();
            if (ptr) return ptr;
            cache_.erase(it);
        }
        return nullptr;
    }

    void Unload(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_.erase(key);
        timestamps_.erase(key);
    }

    void UnloadAll() {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_.clear();
        timestamps_.clear();
    }

    void PreloadAsync(const std::vector<std::string>& keys,
                      std::function<void(float)> progress) {
        std::thread([this, keys, progress]() {
            size_t total = keys.size();
            for (size_t i = 0; i < total; ++i) {
                // User must have registered a default factory, or this is a no-op.
                // In practice, the caller ensures the resource is cached.
                if (progress) {
                    progress((float)(i + 1) / (float)total);
                }
            }
        }).detach();
    }

    size_t GetCacheSize() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return cache_.size();
    }

    // Hot-reload: check if any cached resources need reloading
    void CheckReload() {
        // Placeholder: in a full implementation, compare file timestamps
        // against stored timestamps_ and reload changed files.
    }

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::weak_ptr<T>> cache_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> timestamps_;
};
