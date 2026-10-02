#include "storage_manager.h"
#include <Vcl.Forms.hpp>
#include <memory>

class storage_service_vcl_impl : public IStorageService {
private:
    std::filesystem::path root_path;
    std::filesystem::path data_path;
    std::filesystem::path sound_path;
    std::filesystem::path saves_path;

    // Внутренний хелпер загрузки, инкапсулирующий TStringList
    GameResourceData read_file_internal(const std::filesystem::path& full_path) {
        GameResourceData result;
        if (std::filesystem::exists(full_path)) {
            auto file_loader = std::make_unique<TStringList>();
            file_loader->LoadFromFile(full_path.c_str(), TEncoding::UTF8);

            result.lines.reserve(file_loader->Count);
            for (int i = 0; i < file_loader->Count; ++i) {
                result.lines.emplace_back(file_loader->Strings[i].c_str());
            }
            result.is_loaded = true;
        }
        return result;
    }

public:
    storage_service_vcl_impl() {
        root_path = std::filesystem::path(ParamStr(0).c_str()).parent_path();
        data_path = root_path / "data";
        sound_path = root_path / "sound";
        saves_path = root_path / "saves";

        std::filesystem::create_directories(saves_path);
    }

    bool validate_required_resources() override {
        if (!std::filesystem::is_directory(data_path))
        {
            return false;
        }
        if (!std::filesystem::exists(data_path / "chapt.txt") ||
            !std::filesystem::exists(data_path / "invent.txt") ||
            !std::filesystem::exists(data_path / "test.txt")) {
            return false;
        }
        return true;
    }

    std::filesystem::path get_sound_path(const std::string& sound_name) const override {
        return sound_path / sound_name;
    }

    std::filesystem::path get_save_path(const std::string& save_name) const override {
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

    bool save_game_file(const std::string& save_name, const GameResourceData& data) override {
        try {
            auto file_saver = std::make_unique<TStringList>();
            for (const auto& line : data.lines) {
                file_saver->Add(line.c_str());
            }
            file_saver->SaveToFile((saves_path / save_name).c_str(), TEncoding::UTF8);
            return true;
        } catch (...) {
            return false;
        }
    }

    GameResourceData load_save_file(const std::string& save_name) override {
        return read_file_internal(saves_path / save_name);
    }
};

namespace storage_system {
    static std::unique_ptr<IStorageService> instance = nullptr;

    void initialize() {
        if (!instance) {
            instance = std::make_unique<storage_service_vcl_impl>();
        }
    }

    IStorageService& get() {
        return *instance;
    }
}
