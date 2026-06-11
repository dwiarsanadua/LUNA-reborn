#pragma once
#include <string>
#include <yaml-cpp/yaml.h>

struct ServerConfig {
    struct ServerInfo {
        std::string name = "LunaPlus Server";
        std::string type = "map";
        int port = 10001;
        int max_connections = 500;
        int worker_threads = 4;
        std::string bind_address = "0.0.0.0";
    } server;

    struct DatabaseInfo {
        std::string type = "sqlite";
        std::string sqlite_member_path = "data/luna_member.db";
        std::string sqlite_game_path = "data/luna_game.db";
        std::string sqlite_log_path = "data/luna_log.db";
        std::string postgres_host = "localhost";
        int postgres_port = 5432;
        std::string postgres_member_db = "LUNA_MEMBERDB";
        std::string postgres_game_db = "LUNA_GAMEDB";
        std::string postgres_log_db = "LUNA_LOGDB";
        std::string postgres_user = "game";
        std::string postgres_password = "luna";
    } database;

    struct LoggingInfo {
        std::string level = "debug";
        std::string file = "logs/server.log";
        bool console = true;
    } logging;

    struct NetworkInfo {
        bool tcp_nodelay = true;
        int send_buffer_size = 65536;
        int recv_buffer_size = 65536;
    } network;
};

inline ServerConfig LoadConfig(const std::string& path) {
    YAML::Node cfg = YAML::LoadFile(path);
    ServerConfig config;

    if (cfg["server"]) {
        config.server.name = cfg["server"]["name"].as<std::string>(config.server.name);
        config.server.type = cfg["server"]["type"].as<std::string>(config.server.type);
        config.server.port = cfg["server"]["port"].as<int>(config.server.port);
        config.server.max_connections = cfg["server"]["max_connections"].as<int>(config.server.max_connections);
        config.server.worker_threads = cfg["server"]["worker_threads"].as<int>(config.server.worker_threads);
        config.server.bind_address = cfg["server"]["bind_address"].as<std::string>(config.server.bind_address);
    }

    if (cfg["database"]) {
        config.database.type = cfg["database"]["type"].as<std::string>(config.database.type);
        if (cfg["database"]["sqlite"]) {
            config.database.sqlite_member_path = cfg["database"]["sqlite"]["member_db"].as<std::string>(config.database.sqlite_member_path);
            config.database.sqlite_game_path = cfg["database"]["sqlite"]["game_db"].as<std::string>(config.database.sqlite_game_path);
            config.database.sqlite_log_path = cfg["database"]["sqlite"]["log_db"].as<std::string>(config.database.sqlite_log_path);
        }
        if (cfg["database"]["postgres"]) {
            config.database.postgres_host = cfg["database"]["postgres"]["host"].as<std::string>(config.database.postgres_host);
            config.database.postgres_port = cfg["database"]["postgres"]["port"].as<int>(config.database.postgres_port);
            config.database.postgres_user = cfg["database"]["postgres"]["user"].as<std::string>(config.database.postgres_user);
            config.database.postgres_password = cfg["database"]["postgres"]["password"].as<std::string>(config.database.postgres_password);
            config.database.postgres_member_db = cfg["database"]["postgres"]["member_db"].as<std::string>(config.database.postgres_member_db);
            config.database.postgres_game_db = cfg["database"]["postgres"]["game_db"].as<std::string>(config.database.postgres_game_db);
            config.database.postgres_log_db = cfg["database"]["postgres"]["log_db"].as<std::string>(config.database.postgres_log_db);
        }
    }

    if (cfg["logging"]) {
        config.logging.level = cfg["logging"]["level"].as<std::string>(config.logging.level);
        config.logging.file = cfg["logging"]["file"].as<std::string>(config.logging.file);
        config.logging.console = cfg["logging"]["console"].as<bool>(config.logging.console);
    }

    if (cfg["network"]) {
        config.network.tcp_nodelay = cfg["network"]["tcp_nodelay"].as<bool>(config.network.tcp_nodelay);
        config.network.send_buffer_size = cfg["network"]["send_buffer_size"].as<int>(config.network.send_buffer_size);
        config.network.recv_buffer_size = cfg["network"]["recv_buffer_size"].as<int>(config.network.recv_buffer_size);
    }

    return config;
}
