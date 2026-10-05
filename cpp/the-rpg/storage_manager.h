#pragma once
#include <filesystem>
#include <vector>
#include <string>

#include "nlohmann/json.hpp"

struct race_data
{
    std::wstring name;
    int modifier = 0;
};

struct weapon_data {
    std::string name;
    int hp = 0;
    int mana = 0;
    int ap = 0;
};

struct GameResourceData
{
    std::vector<std::wstring> lines;
    bool is_loaded = false;
};

class IStorageService
{
public:
    virtual ~IStorageService() = default;

    virtual bool validate_required_resources() = 0;

    virtual std::filesystem::path get_sound_path(const std::string& sound_name) const = 0;
    virtual std::filesystem::path get_save_path(const std::string& save_name) const = 0;
    virtual const std::vector<weapon_data>& get_weapons() = 0;
    virtual std::string load_story_json_raw() = 0;

    virtual void preload_creatures_json() = 0;
    virtual const nlohmann::json& get_creatures_json() = 0;

    virtual void preload_story_json() = 0;
    virtual const nlohmann::json& get_chapter_json(int id) = 0;

    virtual GameResourceData load_test_file() = 0;
    virtual GameResourceData load_inventory_file() = 0;

    virtual bool save_game_file(const std::string& save_name, const GameResourceData& data) = 0;
    virtual GameResourceData load_save_file(const std::string& save_name) = 0;
};

namespace storage_system
{
    void initialize();
    IStorageService& get();
}
