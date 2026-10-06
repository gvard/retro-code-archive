#include <Winapi.MMSystem.hpp>
#include "nlohmann/json.hpp"
#include <string_view>
#include <memory>
#include <cstdio>
#include <filesystem>

#include "chapt_view.h"
#include "first.h"
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
    save = new TStringList;
    chapter = new TStringList;
    hasUnsavedChanges = false;
    is_test_mode = false;
}

__fastcall TfrmChapt::~TfrmChapt()
{
    delete save;
    delete chapter;
}
void __fastcall TfrmChapt::ExitClick(TObject* /*Sender*/)
{
    frmChapt->Close();
}

void __fastcall TfrmChapt::lbChoicesDblClick(TObject* Sender)
{
    ConfirmChoiceClick(Sender);
}

void TfrmChapt::LoadNext(int target_qid)
{
    if (this->is_test_mode)
    {
        Memo1->Lines->Clear();
        lbChoices->Items->Clear();

        if (!((test_qptr < chapter->Count) && (chapter->Strings[test_qptr].Length() > 0)))
        {
            test_qptr++;
            return;
        }

        wchar_t ch = chapter->Strings[test_qptr][1];
        if (ch == L'{')
        {
            while ((++test_qptr < chapter->Count) && (chapter->Strings[test_qptr].Length() > 0))
            {
                if (chapter->Strings[test_qptr][1] == L'}')
                {
                    break;
                }
                Memo1->Lines->Add(chapter->Strings[test_qptr]);
            }
        }
        else
        {
            Memo1->Lines->Add(chapter->Strings[test_qptr]);
        }

        aptr = test_qptr + 1;
        while ((++test_qptr < chapter->Count) && (chapter->Strings[test_qptr].Length() > 0))
        {
            String str = chapter->Strings[test_qptr];
            int istr, idex, imag;
            swscanf(str.c_str(), L"%d %d %d", &istr, &idex, &imag);
            str = str.Trim();

            // Выделяем текст ответа, идущий после трех чисел модификаторов
            wchar_t buf[255];
            wcscpy(buf, str.c_str());
            wchar_t* tmp = nullptr;
            for (size_t i = 0, j = 0; i < wcslen(buf); ++i)
            {
                if ((buf[i] == L' ') && (j < 3))
                {
                    tmp = &buf[i + 1];
                    j++;
                }
            }
            lbChoices->Items->Add(tmp);
        }
        test_qptr++;
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
            auto fight_form = std::make_unique<TfrmFight>(this);

            // Название битвы
            std::string event_name = current_node.value("name", "Битва");
            fight_form->battle_caption = UTF8String(const_cast<char*>(event_name.c_str()));

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
            int target_jump = target_qid + 1;
            if (current_node.contains("routes")) {
                target_jump = current_node["routes"].value("victory", target_qid + 1);
            }

            this->Hide();
            PlaySound(storage_system::get().get_sound_path("fight.wav").c_str(), nullptr, SND_ASYNC);
            const int battle_result = fight_form->ShowModal();
            this->Show();

            if (battle_result == mrOk)
            {
                this->LoadNext(target_jump);
            }
            else
            {
                User->hlth = 15;
                User->stamina = 10;

                int defeat_jump = this->previousTextQid;
                if (current_node.contains("routes")) {
                    defeat_jump = current_node["routes"].value("defeat", this->previousTextQid);
                }
                this->LoadNext(defeat_jump);

                constexpr auto title = L"The RPG"sv;
                constexpr auto msg = L"Вы потерпели поражение и убежали от врагов."sv;
                ::MessageBoxW(this->Handle, msg.data(), title.data(), MB_OK | MB_ICONWARNING);
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
        int istr = 0, idex = 0, imag = 0;

        // Читаем модификаторы параметров из оригинальной строки
        swscanf(chapter->Strings[aptr + selected_index].c_str(), L"%d %d %d", &istr, &idex, &imag);
        User->strength += istr;
        User->dex += idex;
        User->mag += imag;

        // Проверяем: закончился ли файл теста?
        if (aptr + lbChoices->Items->Count >= chapter->Count)
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

            // Переходим к Главе 1 сюжета
            this->LoadNext(1);
            return;
        }

        // Если тест не кончился, парсим следующий вопрос
        this->LoadNext(0);
        return;
    }

    // Режим 2: стандартный игровой процесс
    int jump = 0;

    const nlohmann::json& current_node = storage_system::get().get_chapter_json(this->currentQid);

    if (!current_node.empty())
    {
        if (current_node.contains("choices") && current_node["choices"].is_array() && selected_index < current_node["choices"].size())
        {
            auto choice_obj = current_node["choices"][selected_index];
            jump = choice_obj.value("target_id", 1);

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
    this->test_qptr = 0;
    this->hasUnsavedChanges = false;
    this->Menu = nullptr;

    GameResourceData res = storage_system::get().load_test_file();
    if (res.is_loaded) {
        chapter->Clear();
        for (const auto& line : res.lines) {
            chapter->Add(line.c_str());
        }
    }

    this->LoadNext(0);
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

void __fastcall TfrmChapt::lbChoicesMeasureItem(TWinControl *Control, int Index, int &Height)
{
    Height = lbChoices->Canvas->TextHeight("Wg") + 2;
}

void __fastcall TfrmChapt::lbChoicesDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State)
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
