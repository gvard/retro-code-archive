#include <Winapi.MMSystem.hpp>
#include "nlohmann/json.hpp"
#include <string_view>
#include <memory>
#include <cstdio>
#include <filesystem>

#include "chapt_view.h"
#include "main_menu_view.h"
#include "about_view.h"
#include "fight_view.h"
#include "charstats_view.h"
#include "inventory_view.h"
#include "storage_manager.h"

#pragma resource "*.dfm"

TfrmChapt* frmChapt;

__fastcall TfrmChapt::TfrmChapt(TComponent* Owner)
    : TForm(Owner)
{
    hasUnsavedChanges = false;
    is_test_mode = false;
}

__fastcall TfrmChapt::~TfrmChapt()
{
}

void __fastcall TfrmChapt::ExitClick(TObject* /*Sender*/)
{
    frmChapt->Close();
}

void __fastcall TfrmChapt::lbChoicesDblClick(TObject* Sender)
{
    ConfirmChoiceClick(Sender);
}

void TfrmChapt::LoadNext(const std::string& target_qid)
{
    if (this->is_test_mode)
    {
        this->currentQid = target_qid;

        Memo1->Lines->Clear();
        lbChoices->Items->Clear();

        const auto& test_obj = storage_system::get().get_skill_test_json();

        if (!test_obj.contains(target_qid))
        {
            Application->MessageBox(L"Вопрос теста не найден в JSON!", L"Ошибка", MB_ICONERROR | MB_OK);
            return;
        }

        const auto& current_question = test_obj[target_qid];

        if (current_question.contains("text") && current_question["text"].is_array())
        {
            for (const auto& paragraph : current_question["text"])
            {
                std::string p_utf8 = paragraph.get<std::string>();
                Memo1->Lines->Add(UTF8String(p_utf8.c_str()));
            }
        }

        // Наполняем варианты ответов в lbChoices
        if (current_question.contains("choices") && current_question["choices"].is_array())
        {
            for (const auto& choice : current_question["choices"])
            {
                std::string choice_text = choice.value("text", "...");
                lbChoices->Items->Add(UTF8String(choice_text.c_str()));
            }
        }
        return;
    }

    if (this->currentQid != target_qid && User->EnvironmentItems != nullptr)
    {
        User->EnvironmentItems->Clear();
    }

    this->currentQid = target_qid;

    Memo1->Lines->Clear();
    lbChoices->Items->Clear();

    try
    {
        const nlohmann::json& current_node = storage_system::get().get_chapter_json(target_qid);

        if (current_node.empty())
        {
            Application->MessageBox(L"Глава не найдена в JSON!", L"The RPG", MB_ICONEXCLAMATION | MB_OK);
            frmChapt->Hide();
            frmMainMenu->Show();
            return;
        }

        std::string type = current_node.value("type", "story");

        if (type == "fight")
        {
            using namespace std::string_view_literals;
            auto fight_form = std::make_unique<TfrmFight>(nullptr); // this

            // Название битвы
            std::string event_name = current_node.value("name", "Битва");
            fight_form->battle_caption = UTF8String(event_name.c_str());

            if (current_node.contains("enemies") && current_node["enemies"].is_array())
            {
                const auto& creatures_root = storage_system::get().get_creatures_json();

                for (const auto& enemy : current_node["enemies"])
                {
                    std::string race_key = enemy.value("race", "goblin");
                    std::string profile_key = enemy.value("profile", "thug");
                    std::string enemy_name = enemy.value("name", "Враг");

                    // Значения по умолчанию
                    String race_title = L"Неизвестно";
                    int final_hp = 30;
                    int final_dmg = 3;

                    // Находим расу и её профиль в creatures.json
                    if (creatures_root.contains(race_key))
                    {
                        const auto& race_node = creatures_root[race_key];
                        std::string race_dname = race_node.value("display_name", "");
                        race_title = UTF8String(race_dname.c_str());

                        if (race_node.contains("combat_profiles") && race_node["combat_profiles"].contains(profile_key))
                        {
                            const auto& profile_node = race_node["combat_profiles"][profile_key];
                            final_hp = profile_node.value("hp", final_hp);
                            final_dmg = profile_node.value("dmg", final_dmg);
                        }
                    }

                    fight_form->raw_enemies.push_back({
                        race_title,
                        UTF8String(enemy_name.c_str()),
                        final_hp,
                        final_dmg
                    });
                }
            }

            // Маршруты победы и поражения берутся из объекта "routes"
            std::string target_jump = "";
            if (current_node.contains("routes"))
            {
                target_jump = current_node["routes"].value("victory", "");
            }

            this->Hide();
            PlaySound(storage_system::get().get_sound_path("fight.wav").c_str(), nullptr, SND_ASYNC);
            const int battle_result = fight_form->ShowModal();
            this->Show();

            if (battle_result == mrOk)
            {
                this->LoadNext(target_jump);
                return;
            }
            else
            {
                User->hlth = 15;
                User->stamina = 10;

                std::string defeat_jump = this->previousTextQid;
                if (current_node.contains("routes"))
                {
                    defeat_jump = current_node["routes"].value("defeat", this->previousTextQid);
                }
                this->LoadNext(defeat_jump);

                constexpr auto title = L"The RPG"sv;
                constexpr auto msg = L"Вы потерпели поражение и убежали от врагов."sv;
                ::MessageBoxW(this->Handle, msg.data(), title.data(), MB_OK | MB_ICONWARNING);
                return;
            }
            return;
        }

        // if (type == "story")
        if (current_node.contains("location") && current_node["location"].contains("name"))
        {
            std::string loc_name = current_node["location"].value("name", "Неизвестная локация");
            this->Caption = L"The RPG: " + UTF8String(loc_name.c_str());
        }
        else
        {
            // Фоллбек, если у главы нет конкретного имени локации
            this->Caption = L"The RPG: Приключение";
        }

        this->current_location_key = "";

        if (current_node.contains("location") && current_node["location"].is_object())
        {
            const auto& loc_obj = current_node["location"];

            if (loc_obj.contains("loc_id") && loc_obj["loc_id"].is_string())
            {
                this->current_location_key = loc_obj["loc_id"].get<std::string>();
            }

            // Если loc_id нет или он пуст, ищем по вложенному "name"
            if (this->current_location_key.empty() && loc_obj.contains("name") && loc_obj["name"].is_string())
            {
                std::string target_name = loc_obj["name"].get<std::string>();
                const auto& loc_database = storage_system::get().get_locations_json();

                // Перебираем locations.json в поисках совпадения по display name
                for (auto& [key, value] : loc_database.items())
                {
                    if (value.is_object() && value.value("name", "") == target_name)
                    {
                        this->current_location_key = key;
                        break;
                    }
                }
            }
        }

        // Устанавливаем заголовок окна по умолчанию
        this->Caption = L"The RPG: Приключение";

        if (!this->current_location_key.empty())
        {
            const auto& locations = storage_system::get().get_locations_json();
            if (locations.contains(this->current_location_key))
            {
                std::string loc_name = locations[this->current_location_key].value("name", "");
                if (!loc_name.empty())
                {
                    this->Caption = L"The RPG: " + UTF8String(loc_name.c_str());
                }
            }
        }

        // Если по ключу имя извлечь не удалось, используем "name" из ноды истории как фоллбек для Caption
        if (this->Caption == L"The RPG: Приключение" && current_node.contains("location") && current_node["location"].is_object())
        {
            std::string fallback_name = current_node["location"].value("name", "Приключение");
            this->Caption = L"The RPG: " + UTF8String(fallback_name.c_str());
        }

        if (current_node.contains("text") && current_node["text"].is_array())
        {
            for (const auto& paragraph : current_node["text"])
            {
                std::string p_utf8 = paragraph.get<std::string>();
                String vcl_line = UTF8String(p_utf8.c_str());
                // String vcl_line = UnicodeString(p_utf8.c_str());
                Memo1->Lines->Add(L"      " + vcl_line);
            }
        }

        // Наполнение вещей на земле (EnvironmentItems) из json свойств локации
        if (current_node.contains("location") && current_node["location"].contains("items"))
        {
            for (const auto& item : current_node["location"]["items"])
            {
                std::string item_name = item.value("name", "Неизвестный предмет");
                int item_weight = item.value("weight", 1);

                String vcl_name = UnicodeString(UTF8String(item_name.c_str()));
                User->EnvironmentItems->AddObject(vcl_name, reinterpret_cast<TObject*>(static_cast<intptr_t>(item_weight)));
            }
        }

        // Заполнение вариантов ответов
        if (current_node.contains("choices") && current_node["choices"].is_array())
        {
            for (const auto& choice : current_node["choices"])
            {
                std::string choice_text = choice.value("text", "...");
                String vcl_choice = UTF8String(choice_text.c_str());
                lbChoices->Items->Add(vcl_choice);
            }
        }
    }
    catch (...)
    {
        Application->MessageBox(L"Критическая ошибка синтаксиса файла story.json!", L"Ошибка", MB_ICONERROR | MB_OK);
    }
}

void __fastcall TfrmChapt::ConfirmChoiceClick(TObject* /*Sender*/)
{
    int selected_index = -1;
    for (int i = 0; i < lbChoices->Items->Count; ++i)
    {
        if (lbChoices->Selected[i])
        {
            selected_index = i;
            break;
        }
    }

    if (selected_index == -1)
        return;

    // Режим 1: обработка внутри теста создания персонажа
    if (this->is_test_mode)
    {
        const auto& test_obj = storage_system::get().get_skill_test_json();
        std::string jump = "";

        if (test_obj.contains(this->currentQid))
        {
            const auto& current_question = test_obj[this->currentQid];
            if (current_question.contains("choices") && selected_index < current_question["choices"].size())
            {
                const auto& choice_obj = current_question["choices"][selected_index];

                jump = choice_obj.value("target_id", "");

                // Начисляем статы из ability_modifiers
                if (choice_obj.contains("ability_modifiers") && choice_obj["ability_modifiers"].is_object())
                {
                    const auto& mods = choice_obj["ability_modifiers"];
                    User->strength += mods.value("str", 0);
                    User->dex += mods.value("dex", 0);
                    User->mag += mods.value("mag", 0);
                }
            }
        }

        // Проверяем: это был последний вопрос теста?
        if (jump == "END_TEST" || jump.empty())
        {
            // Тест завершен, фиксируем производные характеристики персонажа
            User->maxWeight = static_cast<int>(std::lround(User->strength * 7.5));
            User->stamina = User->GetMaxStamina();

            // Показываем модальное окно характеристик
            auto stats_form = std::make_unique<TfrmCharStats>(this);
            stats_form->ShowModal();

            // Выходим из режима теста и переключаемся на сюжет игры
            this->is_test_mode = false;
            this->initialize_starting_inventory();
            this->Menu = this->MainMenu1;
            this->ResetUnsavedChanges();

            // Переходим к Главе "1" сюжета
            this->LoadNext("1");
            return;
        }

        // Если тест не закончился, загружаем следующий ID вопроса
        this->LoadNext(jump);
        return;
    }

    // Режим 2: стандартный игровой процесс
    std::string jump = "";

    const nlohmann::json& current_node = storage_system::get().get_chapter_json(this->currentQid);

    if (!current_node.empty())
    {
        if (current_node.contains("choices") && current_node["choices"].is_array() && selected_index < current_node["choices"].size())
        {
            auto choice_obj = current_node["choices"][selected_index];
            jump = choice_obj.value("target_id", "1");

            if (choice_obj.contains("effects") && choice_obj["effects"].is_array())
            {
                for (const auto& effect : choice_obj["effects"])
                {
                    std::string action = effect.value("action", "");
                    if (action == "change_gold")
                    {
                        User->gold += effect.value("value", 0);
                    }
                }
            }
        }
    }

    this->previousTextQid = this->currentQid;
    User->Refresh();
    this->hasUnsavedChanges = true;
    this->LoadNext(jump);
}

void __fastcall TfrmChapt::Help1Click(TObject* /*Sender*/)
{
    ShowMessage(L"Сами разберетесь!");
}

void __fastcall TfrmChapt::AboutClick(TObject* /*Sender*/)
{
    auto temporaryAbout = std::make_unique<TfrmAbout>(nullptr);
    temporaryAbout->ShowModal();
}

void __fastcall TfrmChapt::lbChoicesKeyDown(TObject* Sender, WORD& Key, TShiftState Shift)
{
    if (Key == VK_RETURN)
    {
        ConfirmChoiceClick(Sender);
    }
}

void __fastcall TfrmChapt::frmChaptCloseQuery(TObject* /*Sender*/, bool& CanClose)
{
    if (!this->hasUnsavedChanges)
    {
        if (frmMainMenu)
        {
            frmMainMenu->Show();
        }
        this->Hide();
        CanClose = true;
        return;
    }

    using namespace std::string_view_literals;

    constexpr auto title = L"The RPG: выход из приключения"sv;
    constexpr auto msg = L"Покинуть игру?\nВесь несохраненный прогресс будет безвозвратно потерян."sv;

    const int result = ::MessageBoxW(this->Handle, msg.data(), title.data(), MB_OKCANCEL | MB_ICONQUESTION);

    if (result == IDOK)
    {
        if (frmMainMenu)
        {
            frmMainMenu->Show();
        }
        this->Hide();
        CanClose = true;
    }
    else
    {
        CanClose = false;
    }
}

void __fastcall TfrmChapt::LoadClick(TObject* /*Sender*/)
{
    OpenDialog1->FileName = L"";
    OpenDialog1->InitialDir = ExpandFileName(ExePath);
    // OpenDialog1->Filter = L"Файлы сохранений (*.sav)|*.sav|Все файлы (*.*)|*.*";

    if (OpenDialog1->Execute())
    {
        User->LoadGame(OpenDialog1->FileName);
    }
}

void __fastcall TfrmChapt::SaveClick(TObject* /*Sender*/)
{
    SaveDialog1->InitialDir = ExpandFileName(ExePath);
    SaveDialog1->FileName = L"save1.sav";

    if (SaveDialog1->Execute())
    {
        if (User->SaveGame(SaveDialog1->FileName, this->currentQid))
        {
            this->hasUnsavedChanges = false;
            Application->MessageBox(L"Игра успешно сохранена!", L"The RPG", MB_OK | MB_ICONINFORMATION);
        }
    }
}

void __fastcall TfrmChapt::menuInventoryClick(TObject* /*Sender*/)
{
    auto inventory_dialog = std::make_unique<TfrmInventory>(this);
    inventory_dialog->ShowModal();
}

void __fastcall TfrmChapt::menuCharacterStatsClick(TObject* /*Sender*/)
{
    auto stats_form = std::make_unique<TfrmCharStats>(this);
    stats_form->ShowModal();
}

void __fastcall TfrmChapt::menuLocationInfoClick(TObject* /*Sender*/)
{
    if (this->current_location_key.empty())
    {
        ::MessageBoxW(this->Handle, L"Вы находитесь в неопознанном месте.", L"Локация", MB_OK | MB_ICONINFORMATION);
        return;
    }

    const auto& locations = storage_system::get().get_locations_json();

    if (locations.contains(this->current_location_key))
    {
        const auto& loc_node = locations[this->current_location_key];

        std::string name_utf8 = loc_node.value("name", "Без названия");
        std::string desc_utf8 = loc_node.value("description", "Описание отсутствует.");

        // Конвертируем в строки VCL для вывода
        String vcl_name = UTF8String(name_utf8.c_str());
        String vcl_desc = UTF8String(desc_utf8.c_str());

        String message_text = vcl_name + L"\n\n" + vcl_desc;

        ::MessageBoxW(this->Handle, message_text.c_str(), L"Локация", MB_OK);
    }
    else
    {
        ::MessageBoxW(this->Handle, L"Информация о данной локации не найдена в базе данных.", L"Ошибка", MB_OK | MB_ICONWARNING);
    }
}

void __fastcall TfrmChapt::FormResize(TObject* /*Sender*/)
{
    btnConfirmChoice->Left = (ClientWidth - btnConfirmChoice->Width) / 2;
}

void __fastcall TfrmChapt::FormKeyDown(TObject* Sender, WORD& Key, TShiftState Shift)
{
    if (frmMainMenu != nullptr && frmMainMenu->ActionList1 != nullptr)
    {
        if (Key == 'L' && Shift.Contains(ssCtrl))
        {
            this->LoadClick(Sender);
            Key = 0;
            return;
        }
        if (Key == 'S' && Shift.Contains(ssCtrl))
        {
            this->SaveClick(Sender);
            Key = 0;
            return;
        }

        if (Shift == TShiftState{} && !this->is_test_mode)
        {
            if (Key == 'I')
            {
                Key = 0;
                this->menuInventoryClick(Sender);
                return;
            }
            if (Key == 'C')
            {
                Key = 0;
                this->menuCharacterStatsClick(Sender);
                return;
            }
            if (Key == 'M')
            {
                Key = 0;
                this->menuLocationInfoClick(Sender);
                return;
            }
        }

        TWMKey msg;
        msg.Msg = WM_KEYDOWN;
        msg.CharCode = Key;
        msg.KeyData = 0;

        if (frmMainMenu->ActionList1->IsShortCut(msg))
        {
            Key = 0;
        }
    }
}

void TfrmChapt::start_character_test()
{
    this->is_test_mode = true;
    this->hasUnsavedChanges = false;
    this->Menu = nullptr;
    this->Caption = L"The RPG: skill test";

    this->LoadNext("1");
}

void TfrmChapt::initialize_starting_inventory()
{
    if (User == nullptr || User->UserItems == nullptr)
    {
        return;
    }

    User->UserItems->Clear();
    GameResourceData res = storage_system::get().load_inventory_file();

    if (res.is_loaded)
    {
        for (const auto& raw_line : res.lines)
        {
            String currentLine = String(raw_line.c_str()).Trim();

            if (currentLine.IsEmpty())
            {
                continue;
            }

            int lastSpace = currentLine.LastDelimiter(L" ");
            if (lastSpace > 0)
            {
                String itemName = currentLine.SubString(1, lastSpace - 1).Trim();
                String itemWeightStr = currentLine.SubString(lastSpace + 1, currentLine.Length() - lastSpace).Trim();
                int itemWeight = StrToIntDef(itemWeightStr, 1);

                User->UserItems->AddObject(itemName, reinterpret_cast<TObject*>(static_cast<intptr_t>(itemWeight)));
            }
        }
    }
    else
    {
        User->UserItems->AddObject(L"Старый кухонный нож", reinterpret_cast<TObject*>(static_cast<intptr_t>(2)));
    }
}

void __fastcall TfrmChapt::lbChoicesMeasureItem(TWinControl* Control, int Index, int& Height)
{
    Height = lbChoices->Canvas->TextHeight("Wg") + 2;
}

void __fastcall TfrmChapt::lbChoicesDrawItem(TWinControl* Control, int Index, TRect& Rect, TOwnerDrawState State)
{
    TListBox* pListBox = dynamic_cast<TListBox*>(Control);
    if (!pListBox)
        return;

    TCanvas* pCanvas = pListBox->Canvas;

    // Очищаем фон (обрабатывает в том числе выделение строки)
    pCanvas->FillRect(Rect);

    // Берем текст текущей строки
    String text = pListBox->Items->Strings[Index];

    // Вычисляем вертикальную координату для центрирования текста внутри Rect
    int textHeight = pCanvas->TextHeight(text);
    int yOffset = Rect.Top + ((Rect.Height() - textHeight) / 2);

    pCanvas->TextOut(Rect.Left + 4, yOffset, text);
}
