// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>

class Database {
public:
    Database();
    ~Database();

    bool Initialize(const std::string& path);
    void Shutdown();
    bool Execute(const std::string& sql);
    std::vector<std::vector<std::string>> Query(const std::string& sql);

    int Prepare(const std::string& sql);
    void BindInt(int stmt, int index, int value);
    void BindText(int stmt, int index, const std::string& value);
    bool Step(int stmt);
    int GetColumnInt(int stmt, int col);
    std::string GetColumnText(int stmt, int col);
    void Finalize(int stmt);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};
