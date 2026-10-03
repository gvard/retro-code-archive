#pragma once
#include <filesystem>
#include <vector>
#include <string>

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
    virtual const std::vector<race_data>& get_races() = 0;
    virtual const std::vector<weapon_data>& get_weapons() = 0;

    virtual GameResourceData load_chapter_file() = 0;
    virtual GameResourceData load_test_file() = 0;
    virtual GameResourceData load_inventory_file() = 0;
    virtual GameResourceData load_race_file() = 0;

    virtual bool save_game_file(const std::string& save_name, const GameResourceData& data) = 0;
    virtual GameResourceData load_save_file(const std::string& save_name) = 0;
};

namespace storage_system
{
    void initialize();
    IStorageService& get();
}
