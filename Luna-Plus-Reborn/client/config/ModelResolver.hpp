#pragma once
#include <string>

// Resolve legacy model names (.chx / .mod) to runtime GLB/OBJ paths.
namespace ModelResolver {
std::string NormalizeBaseName(const std::string& model_file);
std::string ResolveMonsterModel(const std::string& model_file);
std::string ResolveCharacterModel(const std::string& model_file);
}
