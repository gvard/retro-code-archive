#include "storage_manager.h"
#include <Vcl.Forms.hpp>
#include <memory>
#include <fstream>

class storage_service_vcl_impl : public IStorageService
{
private:
    std::filesystem::path root_path;
    std::filesystem::path data_path;
    std::filesystem::path sound_path;
    std::filesystem::path saves_path;
    std::vector<race_data> cached_races;
    std::vector<weapon_data> cached_weapons;
    nlohmann::json cached_story_json;
    nlohmann::json cached_creatures_json;

    // Внутренний хелпер загрузки, инкапсулирующий TStringList
    GameResourceData read_file_internal(const std::filesystem::path& full_path)
    {
        GameResourceData result;
        if (std::filesystem::exists(full_path))
        {
            auto file_loader = std::make_unique<TStringList>();
            file_loader->LoadFromFile(full_path.c_str(), TEncoding::UTF8);

            result.lines.reserve(file_loader->Count);
            for (int i = 0; i < file_loader->Count; ++i)
            {
                result.lines.emplace_back(file_loader->Strings[i].c_str());
            }
            result.is_loaded = true;
        }
        return result;
    }

public:
    storage_service_vcl_impl()
    {
        root_path = std::filesystem::path(ParamStr(0).c_str()).parent_path();
        data_path = root_path / "data";
        sound_path = root_path / "sound";
        saves_path = root_path / "saves";

        std::filesystem::create_directories(saves_path);
    }

    void preload_creatures_json() override
    {
        // Если кэш уже заполнен, ничего не делаем
        if (!cached_creatures_json.empty())
            return;

        std::filesystem::path json_path = data_path / "creatures.json";
        if (std::filesystem::exists(json_path))
        {
            std::ifstream file(json_path);
            if (file.is_open())
            {
                try {
                    file >> cached_creatures_json;
                }
                catch (...) {
                    cached_creatures_json = nlohmann::json::object();
                }
            }
        }
    }

    const nlohmann::json& get_creatures_json() override
    {
        return cached_creatures_json;
    }

    void preload_story_json() override
    {
        // Если кэш уже заполнен, ничего не делаем
        if (!cached_story_json.empty())
            return;

        std::string raw_json = load_story_json_raw();
        try {
            cached_story_json = nlohmann::json::parse(raw_json);
        }
        catch (...) {
            cached_story_json = nlohmann::json::array(); // Фоллбек на пустой массив
        }
    }

    const nlohmann::json& get_story_json() override
    {
        return cached_story_json;
    }

    std::string load_story_json_raw() override
    {
        std::filesystem::path json_path = data_path / "story.json";
        if (std::filesystem::exists(json_path))
        {
            std::ifstream file(json_path);
            if (file.is_open())
            {
                // Считываем весь файл в строку UTF-8
                return std::string((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
            }
        }
        return "[]";
    }

    const std::vector<weapon_data>& storage_service_vcl_impl::get_weapons() override
    {
        if (cached_weapons.empty())
        {
            std::filesystem::path weapon_file_path = data_path / "weapons.json";
            if (std::filesystem::exists(weapon_file_path))
            {
                std::ifstream file(weapon_file_path);
                if (file.is_open())
                {
                    try
                    {
                        nlohmann::json json_data;
                        file >> json_data;

                        for (const auto& item : json_data)
                        {
                            weapon_data weapon;

                            weapon.name = item.value("name", "");
                            weapon.hp = item.value("hp", 0);
                            weapon.mana = item.value("mana", 0);
                            weapon.ap = item.value("ap", 0);

                            cached_weapons.push_back(std::move(weapon));
                        }
                    }
                    catch (...)
                    {
                        cached_weapons.clear();
                    }
                }
            }

            // Дефолтный фоллбек, если файл пуст или отсутствует
            if (cached_weapons.empty())
            {
                cached_weapons.push_back({ "Кулак", 5, 0, 1 });
            }
        }
        return cached_weapons;
    }

    bool validate_required_resources() override
    {
        if (!std::filesystem::is_directory(data_path))
        {
            return false;
        }
        if (!std::filesystem::exists(data_path / "invent.txt") ||
            !std::filesystem::exists(data_path / "test.txt"))
        {
            return false;
        }
        if (!std::filesystem::exists(data_path / "story.json"))
        {
            return false;
        }
        return true;
    }

    std::filesystem::path get_sound_path(const std::string& sound_name) const override
    {
        return sound_path / sound_name;
    }

    std::filesystem::path get_save_path(const std::string& save_name) const override
    {
        return saves_path / save_name;
    }

    GameResourceData load_test_file() override
    {
        return read_file_internal(data_path / "test.txt");
    }
    GameResourceData load_inventory_file() override
    {
        return read_file_internal(data_path / "invent.txt");
    }

    bool save_game_file(const std::string& save_name, const GameResourceData& data) override
    {
        try
        {
            auto file_saver = std::make_unique<TStringList>();
            for (const auto& line : data.lines)
            {
                file_saver->Add(line.c_str());
            }
            file_saver->SaveToFile((saves_path / save_name).c_str(), TEncoding::UTF8);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    GameResourceData load_save_file(const std::string& save_name) override
    {
        return read_file_internal(saves_path / save_name);
    }
};

namespace storage_system
{
    static std::unique_ptr<IStorageService> instance = nullptr;

void initialize()
{
    if (!instance)
    {
            instance = std::make_unique<storage_service_vcl_impl>();
        }
    }

IStorageService& get()
{
        return *instance;
    }
}
