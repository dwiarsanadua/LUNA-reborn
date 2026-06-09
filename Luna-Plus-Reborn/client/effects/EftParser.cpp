#include "EftParser.hpp"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>

std::unordered_map<std::string, EftDefinition> EftParser::cache_;

std::string EftParser::Trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

EftUnit::Type EftParser::ParseUnitType(const std::string& token) {
    if (token == "#PARTICLE" || token == "PARTICLE") return EftUnit::Particle;
    if (token == "#BILLBOARD" || token == "BILLBOARD") return EftUnit::Billboard;
    if (token == "#MODEL" || token == "MODEL") return EftUnit::Model;
    if (token == "#ANIMATION" || token == "ANIMATION") return EftUnit::Animation;
    if (token == "#LIGHT" || token == "LIGHT") return EftUnit::Light;
    if (token == "#SOUND" || token == "SOUND") return EftUnit::Sound;
    if (token == "#CAMERASHAKE" || token == "CAMERASHAKE" || token == "#SHAKE") return EftUnit::CameraShake;
    if (token == "#DECAL" || token == "DECAL") return EftUnit::Decal;
    return EftUnit::Particle;
}

uint32_t EftParser::ParseColor(const std::string& str) {
    if (str.empty()) return 0xffffffff;
    unsigned int r = 255, g = 255, b = 255, a = 255;
    if (str.find(',') != std::string::npos) {
        sscanf(str.c_str(), "%u,%u,%u,%u", &r, &g, &b, &a);
    } else if (str.find(' ') != std::string::npos) {
        sscanf(str.c_str(), "%u %u %u %u", &r, &g, &b, &a);
    } else {
        unsigned int hex = 0xffffffff;
        sscanf(str.c_str(), "%x", &hex);
        return hex;
    }
    return (a << 24) | (b << 16) | (g << 8) | r;
}

bool EftParser::ParseBool(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

float EftParser::ParseFloat(const std::string& str, float def) {
    if (str.empty()) return def;
    try { return std::stof(str); } catch (...) { return def; }
}

int EftParser::ParseInt(const std::string& str, int def) {
    if (str.empty()) return def;
    try { return std::stoi(str); } catch (...) { return def; }
}

EftDefinition EftParser::Parse(const std::string& path) {
    if (cache_.count(path)) return cache_[path];

    std::ifstream f(path);
    if (!f) {
        spdlog::error("EftParser: Cannot open {}", path);
        return {};
    }

    EftDefinition def;
    def.name = path;

    std::string line;
    EftUnit* current_unit = nullptr;
    int line_num = 0;

    while (std::getline(f, line)) {
        line_num++;
        line = Trim(line);
        if (line.empty() || line[0] == '@' || line[0] == ';' || line[0] == '/') continue;

        // --- Effect-level directives ---

        if (line.find("#NAME") == 0) {
            def.name = Trim(line.substr(5));
        } else if (line.find("#DURATION") == 0 || line.find("#TIME") == 0) {
            def.total_duration = ParseFloat(Trim(line.substr(line.find_first_of(" \t") + 1)), 0.0f);
        } else if (line.find("#REPEAT") == 0) {
            auto val = Trim(line.substr(7));
            if (val.find("COUNT") == 0 || val.find("count") == 0) {
                def.loop_count = ParseInt(Trim(val.substr(5)), 1);
                def.loop = (def.loop_count > 1);
            } else {
                def.loop = ParseBool(val);
                def.loop_count = def.loop ? -1 : 1;
            }
        } else if (line.find("#FADEIN") == 0) {
            def.fade_in = ParseFloat(Trim(line.substr(7)));
        } else if (line.find("#FADEOUT") == 0) {
            def.fade_out = ParseFloat(Trim(line.substr(7)));
        } else if (line.find("#TARGET") == 0) {
            auto val = Trim(line.substr(7));
            if (val == "SELF" || val == "self") def.default_target = EftTargetType::Self;
            else if (val == "TARGET" || val == "target") def.default_target = EftTargetType::Target;
            else if (val == "POSITION" || val == "position") def.default_target = EftTargetType::Position;
            else if (val == "FORWARD" || val == "forward") def.default_target = EftTargetType::Forward;
            else if (val.find("RANDOM") == 0 || val.find("random") == 0) def.default_target = EftTargetType::RandomInRange;

        // --- Unit type directives (start a new unit) ---

        } else if (line.find("#NEWEFFECTUNIT") == 0 || line.find("#EFFECTUNIT") == 0 ||
                   line.find("#BILLBOARD") == 0 || line.find("#PARTICLE") == 0 ||
                   line.find("#MODEL") == 0 || line.find("#ANIMATION") == 0 ||
                   line.find("#LIGHT") == 0 || line.find("#SOUND") == 0 ||
                   line.find("#CAMERASHAKE") == 0 || line.find("#SHAKE") == 0 ||
                   line.find("#DECAL") == 0) {
            def.units.push_back(EftUnit{});
            current_unit = &def.units.back();
            current_unit->type = ParseUnitType(line.substr(0, line.find_first_of(" \t")));
            current_unit->billboard = (current_unit->type == EftUnit::Billboard);

        } else if (current_unit) {
            // --- Unit-level properties ---

            std::string key, value;
            auto sep = line.find_first_of("= \t");
            if (sep != std::string::npos) {
                key = Trim(line.substr(0, sep));
                value = Trim(line.substr(sep + 1));
            } else {
                key = line;
            }

            // Life / duration
            if (key == "#LIFE" || key == "LIFE" || key == "DURATION") {
                current_unit->life_time = ParseFloat(value, 1.0f);
            }
            // Fade in/out
            else if (key == "#FADEIN" || key == "FADEIN") {
                current_unit->fade_in = ParseFloat(value);
            } else if (key == "#FADEOUT" || key == "FADEOUT") {
                current_unit->fade_out = ParseFloat(value);
            }
            // Start time
            else if (key == "#START" || key == "START" || key == "STARTTIME") {
                current_unit->start_time = ParseFloat(value);
            }
            // Position offset
            else if (key == "#OFFSET" || key == "OFFSET" || key == "POSITION" || key == "#POSITION") {
                sscanf(value.c_str(), "%f %f %f", &current_unit->pos_offset.x,
                       &current_unit->pos_offset.y, &current_unit->pos_offset.z);
            }
            // Rotation
            else if (key == "#ROTATE" || key == "ROTATE" || key == "#ROTATION") {
                sscanf(value.c_str(), "%f %f %f", &current_unit->rot_offset.x,
                       &current_unit->rot_offset.y, &current_unit->rot_offset.z);
            }
            // Scale
            else if (key == "#SCALE" || key == "SCALE") {
                sscanf(value.c_str(), "%f %f %f", &current_unit->scale_start.x,
                       &current_unit->scale_start.y, &current_unit->scale_start.z);
            }
            // Scale end (for animated scale)
            else if (key == "#SCALEEND" || key == "SCALE_END" || key == "SCALEEND") {
                sscanf(value.c_str(), "%f %f %f", &current_unit->scale_end.x,
                       &current_unit->scale_end.y, &current_unit->scale_end.z);
            }
            // Direction
            else if (key == "#DIRECTION" || key == "DIRECTION" || key == "#DIR") {
                sscanf(value.c_str(), "%f %f %f", &current_unit->direction.x,
                       &current_unit->direction.y, &current_unit->direction.z);
            }
            // Velocity (speed range)
            else if (key == "#VELOCITY" || key == "VELOCITY" || key == "#SPEED" || key == "SPEED") {
                sscanf(value.c_str(), "%f %f", &current_unit->speed_min, &current_unit->speed_max);
            }
            // Gravity
            else if (key == "#GRAVITY" || key == "GRAVITY") {
                current_unit->gravity = ParseFloat(value, -9.8f);
            }
            // Alpha
            else if (key == "#ALPHA" || key == "ALPHA") {
                sscanf(value.c_str(), "%f %f", &current_unit->alpha_start, &current_unit->alpha_end);
            }
            // Color
            else if (key == "#COLOR" || key == "COLOR") {
                current_unit->color_start = ParseColor(value);
            }
            // Color end
            else if (key == "#COLOREND" || key == "COLOR_END" || key == "COLOREND") {
                current_unit->color_end = ParseColor(value);
            }
            // Texture / file
            else if (key == "#FILE" || key == "FILE" || key == "#TEXTURE" || key == "TEXTURE") {
                current_unit->asset_name = value;
            }
            // Bone attachment
            else if (key == "#BONE" || key == "BONE" || key == "ATTACHBONE") {
                current_unit->follow_bone = true;
                current_unit->bone_name = value;
            }
            // Follow target
            else if (key == "#FOLLOW" || key == "FOLLOW" || key == "#TRACK") {
                current_unit->follow_target = ParseBool(value);
            }
            // Repeat
            else if (key == "#REPEAT" || key == "REPEAT") {
                if (value.find("COUNT") == 0 || value.find("count") == 0) {
                    current_unit->repeat_count = ParseInt(Trim(value.substr(5)), 1);
                    current_unit->repeat = (current_unit->repeat_count > 1);
                } else if (value.find("INTERVAL") == 0 || value.find("interval") == 0) {
                    current_unit->repeat_interval = ParseFloat(Trim(value.substr(8)));
                } else {
                    current_unit->repeat = ParseBool(value);
                    current_unit->repeat_count = current_unit->repeat ? -1 : 1;
                }
            }
            // Emit rate
            else if (key == "#EMIT" || key == "EMIT" || key == "#EMITRATE" || key == "EMITRATE") {
                current_unit->emit_rate = ParseInt(value, 10);
            }
            // Emit count
            else if (key == "#EMITCOUNT" || key == "EMITCOUNT" || key == "COUNT") {
                current_unit->emit_count = ParseInt(value, 0);
            }
            // Additive blending
            else if (key == "#ADDITIVE" || key == "ADDITIVE" || key == "#BLEND") {
                current_unit->additive = ParseBool(value);
            }
            // Billboard mode
            else if (key == "#BILLBOARD" || key == "BILLBOARD") {
                current_unit->billboard = ParseBool(value);
            }

            // --- Sound properties ---
            else if (key == "#VOLUME" || key == "VOLUME") {
                current_unit->volume = ParseFloat(value, 1.0f);
            } else if (key == "#PITCH" || key == "PITCH") {
                current_unit->pitch = ParseFloat(value, 1.0f);
            } else if (key == "#RANGE" || key == "RANGE" || key == "#RADIUS") {
                current_unit->range = ParseFloat(value, 30.0f);
            } else if ((key == "#LOOP" || key == "LOOP") && current_unit->type == EftUnit::Sound) {
                current_unit->loop_sound = ParseBool(value);
            }

            // --- Camera shake properties ---
            else if (key == "#INTENSITY" || key == "INTENSITY" || key == "#SHAKEINTENSITY") {
                current_unit->shake_intensity = ParseFloat(value, 1.0f);
            } else if (key == "#FREQUENCY" || key == "FREQUENCY" || key == "#SHAKEFREQ") {
                current_unit->shake_frequency = ParseFloat(value, 10.0f);
            }

            // --- Light properties ---
            else if (key == "#LIGHTRANGE" || key == "LIGHTRANGE") {
                current_unit->light_range = ParseFloat(value, 10.0f);
            } else if (key == "#LIGHTINTENSITY" || key == "LIGHTINTENSITY") {
                current_unit->light_intensity = ParseFloat(value, 1.0f);
            }

            // --- Attributes (damage/buffs) ---
            else if (key == "#ATTR" || key == "ATTR" || key == "#ATTRIBUTE" || key == "ATTRIBUTE") {
                // Format: ATTR damage,stun_chance,stun_dur,poison_dmg,poison_dur,slow,slow_dur,knockback,heal
                sscanf(value.c_str(), "%d,%f,%f,%f,%f,%f,%f,%f,%d",
                       &current_unit->attr.damage,
                       &current_unit->attr.stun_chance,
                       &current_unit->attr.stun_duration,
                       &current_unit->attr.poison_damage,
                       &current_unit->attr.poison_duration,
                       &current_unit->attr.slow_amount,
                       &current_unit->attr.slow_duration,
                       &current_unit->attr.knockback_distance,
                       &current_unit->attr.heal_amount);
            } else if (key == "#ATTR_DMG" || key == "ATTR_DMG" || key == "#DAMAGE") {
                sscanf(value.c_str(), "%f", &current_unit->attr.damage_mult);
                if (value.find(',') != std::string::npos || value.find(' ') != std::string::npos) {
                    current_unit->attr.damage = ParseInt(value);
                } else {
                    current_unit->attr.damage = static_cast<int32_t>(ParseFloat(value));
                }
            } else if (key == "#BUFF" || key == "BUFF" || key == "#BUFFID") {
                current_unit->attr.buff_id = static_cast<uint32_t>(ParseInt(value));
            } else if (key == "#BUFFDURATION" || key == "BUFF_DURATION") {
                current_unit->attr.buff_duration = ParseFloat(value);
            }
            // Heal attribute
            else if (key == "#HEAL" || key == "HEAL") {
                sscanf(value.c_str(), "%d,%f", &current_unit->attr.heal_amount,
                       &current_unit->attr.heal_mult);
                if (current_unit->attr.heal_amount == 0 && current_unit->attr.heal_mult == 0) {
                    current_unit->attr.heal_amount = ParseInt(value);
                }
            }
            // Stun attribute
            else if (key == "#STUN" || key == "STUN") {
                sscanf(value.c_str(), "%f,%f", &current_unit->attr.stun_chance,
                       &current_unit->attr.stun_duration);
            }

            // --- Target type ---
            else if (key == "#TARGETTYPE" || key == "TARGETTYPE" || key == "#TARGET") {
                auto v = value;
                std::transform(v.begin(), v.end(), v.begin(), ::toupper);
                if (v == "SELF") current_unit->target_type = EftTargetType::Self;
                else if (v == "TARGET") current_unit->target_type = EftTargetType::Target;
                else if (v == "POSITION" || v == "POS") current_unit->target_type = EftTargetType::Position;
                else if (v == "FORWARD") current_unit->target_type = EftTargetType::Forward;
                else current_unit->target_type = EftTargetType::Target;
            }

            // --- Chaining (next effect) ---
            else if (key == "#NEXT" || key == "NEXT" || key == "#CHAIN") {
                current_unit->next_effect_id = static_cast<uint32_t>(ParseInt(value));
            } else if (key == "#NEXTDELAY" || key == "NEXTDELAY" || key == "#CHAINDELAY") {
                current_unit->next_delay = ParseFloat(value);
            }

            // --- Unknown token warning ---
            else if (key[0] == '#') {
                spdlog::warn("EftParser: Unknown token '{}' at line {} in {}", key, line_num, path);
            }
        }
    }

    // Auto-calculate total_duration if not set
    if (def.total_duration <= 0.0f) {
        for (auto& u : def.units) {
            float end = u.start_time + u.life_time;
            if (u.repeat && u.repeat_count > 0) {
                end += u.repeat_interval * (u.repeat_count - 1);
            }
            if (end > def.total_duration) def.total_duration = end;
        }
    }

    cache_[path] = def;
    spdlog::debug("EftParser: Parsed '{}' ({} units, {:.2f}s duration)", path, def.units.size(), def.total_duration);
    return def;
}

std::vector<std::string> EftParser::GetParsedEffectNames() {
    std::vector<std::string> names;
    for (auto& [k, v] : cache_) {
        names.push_back(k);
    }
    return names;
}
