#include "storage_manager.h"
#include <Vcl.Forms.hpp>
#include <memory>

class storage_service_vcl_impl : public IStorageService
{
private:
    std::filesystem::path root_path;
    std::filesystem::path data_path;
    std::filesystem::path sound_path;
    std::filesystem::path saves_path;
    std::vector<race_data> cached_races;

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

    const std::vector<race_data>& get_races() override
    {
        if (cached_races.empty())
        {
            auto raw_data = load_race_file();
            for (const auto& raw_line : raw_data.lines)
            {
                std::wstring line = raw_line;

                // Очистка строки от пробелов и управляющих символов (\r, \n, \t) на концах
                size_t start = line.find_first_not_of(L" \t\r\n");
                if (start == std::wstring::npos)
                    continue;

                line = line.substr(start, line.find_last_not_of(L" \t\r\n") - start + 1);

                // Выделение имени и модификатора по знакам модификатора
                if (size_t sign_pos = line.find_first_of(L"+-"); sign_pos != std::wstring::npos)
                {
                    race_data race;
                    size_t name_end = line.find_last_not_of(L" \t", sign_pos - 1);
                    race.name = line.substr(0, name_end + 1);

                    try
                    {
                        race.modifier = std::stoi(line.substr(sign_pos));
                    }
                    catch (...)
                    {
                        race.modifier = 0;
                    }

                    cached_races.push_back(std::move(race));
                }
            }
        }
        return cached_races;
    }

    bool validate_required_resources() override
    {
        if (!std::filesystem::is_directory(data_path))
        {
            return false;
        }
        if (!std::filesystem::exists(data_path / "chapt.txt") ||
            !std::filesystem::exists(data_path / "invent.txt") ||
            !std::filesystem::exists(data_path / "test.txt"))
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

    GameResourceData load_chapter_file() override
    {
        return read_file_internal(data_path / "chapt.txt");
    }
    GameResourceData load_test_file() override
    {
        return read_file_internal(data_path / "test.txt");
    }
    GameResourceData load_inventory_file() override
    {
        return read_file_internal(data_path / "invent.txt");
    }

    GameResourceData load_race_file() override
    {
        return read_file_internal(data_path / "crt.txt");
    }

    GameResourceData load_weapon_file() override
    {
        return read_file_internal(data_path / "weap.txt");
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
