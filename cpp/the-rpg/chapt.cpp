#include <Winapi.MMSystem.hpp>
#include <string_view>
#include <memory>
#include <cstdio>
#include <filesystem>

#include "chapt.h"
#include "first.h"
#include "about_view.h"
#include "fight.h"
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

void __fastcall TfrmChapt::FormCreate(TObject* /*Sender*/)
{
    GameResourceData res = storage_system::get().load_chapter_file();
    if (res.is_loaded) {
        chapter->Clear();
        for (const auto& line : res.lines) {
            chapter->Add(line.c_str());
        }
    }
}

void __fastcall TfrmChapt::ExitClick(TObject* /*Sender*/)
{
    frmChapt->Close();
}

void __fastcall TfrmChapt::ListBox1DblClick(TObject* Sender)
{
    ConfirmChoiceClick(Sender);
}

void TfrmChapt::LoadNext(int target_qid)
{
    if (this->is_test_mode)
    {
        Memo1->Lines->Clear();
        ListBox1->Items->Clear();

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
            ListBox1->Items->Add(tmp);
        }
        test_qptr++;
        return;
    }

    // Глава изменилась — сбрасываем состояние вещей на земле
    if (this->currentQid != target_qid && User->EnvironmentItems != nullptr)
    {
        User->EnvironmentItems->Clear();
    }

    this->currentQid = target_qid;

    wchar_t ch = 0;
    int q = 0;
    int qptr = -1;

    Memo1->Lines->Clear();
    ListBox1->Items->Clear();

    // Поиск нужного ID вопроса
    for (int i = 0; i < chapter->Count; ++i)
    {
        ch = 0;
        q = -1;

        swscanf(chapter->Strings[i].c_str(), L"%d%c", &q, &ch);

        if (q == target_qid)
        {
            qptr = i;
            break;
        }

        if (ch == L'{')
        {
            while (++i < chapter->Count)
            {
                if (!chapter->Strings[i].IsEmpty())
                {
                    if (chapter->Strings[i][1] == L'}')
                    {
                        break;
                    }
                }
            }
        }
        while ((++i < chapter->Count) && (!chapter->Strings[i].IsEmpty()));
    }

    if (qptr < 0)
    {
        Application->MessageBox(L"Вопрос не найден!", L"The RPG", MB_ICONEXCLAMATION | MB_OK);
        frmChapt->Hide();
        frmMainMenu->Show();
        return;
    }

    // Чтение заголовка и типа вопроса
    ch = 0;
    swscanf(chapter->Strings[qptr].c_str(), L"%d%c", &q, &ch);

    switch (ch)
    {
        case L'{':
            while (++qptr < chapter->Count)
            {
                if (!chapter->Strings[qptr].IsEmpty())
                {
                    if (chapter->Strings[qptr][1] == L'}')
                    {
                        break;
                    }
                }
                Memo1->Lines->Add(chapter->Strings[qptr]);
            }
            break;

        case L' ':
            {
                String str = chapter->Strings[qptr].Trim();
                int lastSpace = str.LastDelimiter(L" ");
                if (lastSpace > 0)
                {
                    Memo1->Lines->Add(str.SubString(lastSpace + 1, str.Length() - lastSpace));
                }
                else
                {
                    Memo1->Lines->Add(str);
                }
            }
            break;

        case L'f':
            {
                using namespace std::string_view_literals;
                const int current_battle_qid = this->currentQid;
                AnsiString fight_str = chapter->Strings[qptr++];
                fight_str.Trim();

                char buf[256];
                std::strcpy(buf, fight_str.c_str());

                int qid = 0, target_jump = 0;
                char ch = 0;
                std::sscanf(buf, "%d%c %d", &qid, &ch, &target_jump);

                char* caption_ptr = nullptr;
                for (size_t i = 0, j = 0; i < std::strlen(buf); ++i)
                {
                    if (buf[i] == ' ') {
                        j++;
                        if (j >= 2) {
                            caption_ptr = &buf[i + 1];
                            break;
                        }
                    }
                }

                auto fight_form = std::make_unique<TfrmFight>(this);
                fight_form->battle_caption = caption_ptr;

                // Кэшируем расы из подсистемы хранения, чтобы передать в окно боя
                const auto race_res = storage_system::get().load_race_file();
                if (race_res.is_loaded) {
                    for (const auto& line : race_res.lines) {
                        fight_form->raw_race_types.push_back(line);
                    }
                }

                while (qptr < chapter->Count) {
                    String line_data = chapter->Strings[qptr];
                    if (line_data.Length() <= 0) break;

                    char enemy_buf[256];
                    std::strcpy(enemy_buf, AnsiString(line_data).c_str());

                    int enemy_type = 0, enemy_hits = 0;
                    std::sscanf(enemy_buf, "%d %d", &enemy_type, &enemy_hits);

                    char* enemy_name_ptr = nullptr;
                    for (size_t i = 0, j = 0; i < std::strlen(enemy_buf); ++i) {
                        if (enemy_buf[i] == ' ') {
                            j++;
                            if (j >= 2) { enemy_name_ptr = &enemy_buf[i + 1]; break; }
                        }
                    }

                    fight_form->raw_enemies.push_back({enemy_type, enemy_hits, enemy_name_ptr});
                    qptr++;
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

                    this->LoadNext(this->previousTextQid);

                    constexpr auto title = L"The RPG"sv;
                    constexpr auto msg = L"Вы потерпели поражение и убежали от врагов."sv;
                    ::MessageBoxW(this->Handle, msg.data(), title.data(), MB_OK | MB_ICONWARNING);
                }
            }
            return;

        case L'e':
            Application->MessageBox(L"Игра окончена!", L"The RPG", MB_OK);
            frmChapt->Hide();
            frmMainMenu->Show();
            return;
    }

    // Заполнение вариантов ответов
    aptr = qptr + 1;
    while ((++qptr < chapter->Count) && (!chapter->Strings[qptr].IsEmpty()))
    {
        String str = chapter->Strings[qptr];
        int jump = 0;

        swscanf(str.c_str(), L"%d", &jump);
        str = str.Trim();

        int firstSpace = str.Pos(L" ");
        if (firstSpace > 0)
        {
            String tmp = str.SubString(firstSpace + 1, str.Length() - firstSpace).Trim();
            ListBox1->Items->Add(tmp);
        }
        else
        {
            ListBox1->Items->Add(str);
        }
    }
}

void __fastcall TfrmChapt::ConfirmChoiceClick(TObject* /*Sender*/)
{
    int selected_index = -1;
    for (int i = 0; i < ListBox1->Items->Count; ++i)
    {
        if (ListBox1->Selected[i])
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
        if (aptr + ListBox1->Items->Count >= chapter->Count)
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

            // Загружаем основной файл сюжета и сбрасываем флаг dirty-состояния
            chapter->LoadFromFile(ExePath + L"data\\chapt.txt", TEncoding::UTF8);
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
    swscanf(chapter->Strings[aptr + selected_index].c_str(), L"%d ", &jump);
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

void __fastcall TfrmChapt::ListBox1KeyDown(TObject* Sender, WORD& Key, TShiftState Shift)
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
