#include <Winapi.Windows.hpp>
#include <cstdio>
#include <memory>

#include "first.h"
#include "about_view.h"
#include "chargen_view.h"
#include "chapt_view.h"
#include "inventory_view.h"
#include "charstats_view.h"
#include "fight_view.h"
#include "storage_manager.h"

#pragma resource "*.dfm"

TfrmMainMenu* frmMainMenu;
TUser* User;
String ExePath;

// КЛАСС СУЩНОСТИ ИГРОКА (TUser)

void TUser::Clear()
{
    if (frmFight != nullptr && frmFight->Visible)
    {
        frmFight->Hide();
    }

    if (frmChapt != nullptr && frmChapt->Visible)
    {
        frmChapt->Hide();
    }

    Name = L"";
    CrType = L"";
    SexType = L"";
    age = 0;
    strength = 5;
    dex = 5;
    mag = 5;
    hlth = 100;
    mana = 100;
    maxWeight = 0;
    gold = 0;

    // Безопасное выделение памяти под списки предметов, если они еще не созданы
    if (UserItems == nullptr)
        UserItems = new TStringList;
    if (EnvironmentItems == nullptr)
        EnvironmentItems = new TStringList;

    UserItems->Clear();
    EnvironmentItems->Clear();

    // Запись стартового маркера инициализации
    UserItems->Add(L"__INIT_NEW_GAME__");

    stamina = GetMaxStamina();
}

void TUser::Refresh()
{
    hlth += 5;
    mana += 5;
    stamina += 5;

    if (hlth > 100)
        hlth = 100;
    if (mana > 100)
        mana = 100;

    int maxStamina = GetMaxStamina();
    if (stamina > maxStamina)
        stamina = maxStamina;
}

auto TUser::GetMaxStamina() -> int
{
    // [Core Mechanic] Returns maximum stamina based on Strength and Dexterity
    return 50 + (strength * 4) + (dex * 2);
}

void TUser::RecalculateStamina(int totalWeight)
{
    int maxStamina = GetMaxStamina();

    if (totalWeight > maxWeight)
    {
        stamina = maxStamina - 20; // Штраф -20 единиц при перегрузе
    }
    else
    {
        stamina = maxStamina;
    }
}

auto TUser::LoadGame(const String& AFileName) -> bool
{
    auto* saveList = new TStringList;
    bool success = false;

    try
    {
        saveList->LoadFromFile(AFileName, TEncoding::UTF8);

        if (saveList->Count >= 11)
        {
            // Создаем временные независимые списки для чтения вещей из файла.
            TStringList* tempUserItems = new TStringList;
            TStringList* tempEnvironmentItems = new TStringList;

            Name = saveList->Strings[0];
            CrType = saveList->Strings[1];
            SexType = saveList->Strings[2];
            age = StrToInt(saveList->Strings[3]);
            strength = StrToInt(saveList->Strings[4]);
            dex = StrToInt(saveList->Strings[5]);
            mag = StrToInt(saveList->Strings[6]);
            hlth = StrToInt(saveList->Strings[7]);
            mana = StrToInt(saveList->Strings[8]);
            stamina = StrToInt(saveList->Strings[9]);
            maxWeight = static_cast<int>(std::lround(strength * 7.5));

            int nextChapter = StrToInt(saveList->Strings[10]);

            TStringList* currentTargetList = nullptr;

            for (int i = 11; i < saveList->Count; ++i)
            {
                String currentLine = saveList->Strings[i].Trim();
                if (currentLine.IsEmpty())
                    continue;

                if (currentLine.UpperCase() == L"[INVENTORY]")
                {
                    currentTargetList = tempUserItems; // Переключаем на временный рюкзак
                    continue;
                }
                if (currentLine.UpperCase() == L"[ENVIRONMENT]")
                {
                    currentTargetList = tempEnvironmentItems; // Переключаем на временное окружение
                    continue;
                }

                if (currentTargetList != nullptr)
                {
                    int lastSpace = currentLine.LastDelimiter(L" ");
                    if (lastSpace > 0)
                    {
                        String itemName = currentLine.SubString(1, lastSpace - 1).Trim();
                        String itemWeightStr = currentLine.SubString(lastSpace + 1, currentLine.Length() - lastSpace).Trim();
                        int itemWeight = StrToIntDef(itemWeightStr, 1);

                        currentTargetList->AddObject(itemName, reinterpret_cast<TObject*>(static_cast<intptr_t>(itemWeight)));
                    }
                }
            }

            // Создаем форму квеста frmChapt, если игрок загружается самым первым действием со старта
            if (frmChapt == nullptr)
            {
                Application->CreateForm(__classid(TfrmChapt), &frmChapt);
            }

            // Разворачиваем сюжет квеста.
            // Если внутри LoadNext или сопутствующих VCL-методов сработает скрытый сброс Clear(),
            // он сотрет только пустые дефолтные списки TUser, но не временные массивы
            frmChapt->LoadNext(nextChapter);

            if (frmFight != nullptr && frmFight->Visible)
            {
                frmFight->Hide();
            }

            // Все скрытые формы инициализировались и затихли.
            // Очищаем списки TUser от любого мусора (включая строку __INIT_NEW_GAME__)
            if (UserItems != nullptr)
                UserItems->Clear();
            if (EnvironmentItems != nullptr)
                EnvironmentItems->Clear();

            // Переносим вещи из временной безопасной памяти в постоянные структуры TUser
            if (UserItems != nullptr)
                UserItems->AddStrings(tempUserItems);
            if (EnvironmentItems != nullptr)
                EnvironmentItems->AddStrings(tempEnvironmentItems);

            // Удаляем временные контейнеры, закрывая утечки памяти Windows
            delete tempUserItems;
            delete tempEnvironmentItems;

            // Инвентарь для текущей сессии официально и успешно восстановлен
            this->isInventoryLoaded = true;

            if (frmChapt != nullptr)
            {
                frmChapt->ResetUnsavedChanges();
            }

            success = true;
        }
        else
        {
            Application->MessageBox(L"Извините, но сейв слишком короткий или пустой.", L"The RPG", MB_OK | MB_ICONERROR);
        }
    }
    catch (EConvertError&)
    {
        Application->MessageBox(L"Извините, но сейв поврежден.", L"The RPG", MB_OK | MB_ICONERROR);
    }
    catch (...)
    {
        Application->MessageBox(L"Не удалось прочитать файл сохранения.", L"The RPG", MB_OK | MB_ICONERROR);
    }

    delete saveList;

    wchar_t finalLog[256];
    swprintf(finalLog, 256, L"Загружено в рюкзак: %d | Загружено в окружение: %d",
             UserItems->Count, EnvironmentItems->Count);
    OutputDebugString(finalLog);

    return success;
}

auto TUser::SaveGame(const String& AFileName, int ACurrentQid) -> bool
{
    auto* saveList = new TStringList;
    bool success = false;

    try
    {
        saveList->Add(Name);
        saveList->Add(CrType);
        saveList->Add(SexType);
        saveList->Add(IntToStr(age));
        saveList->Add(IntToStr(strength));
        saveList->Add(IntToStr(dex));
        saveList->Add(IntToStr(mag));
        saveList->Add(IntToStr(hlth));
        saveList->Add(IntToStr(mana));
        saveList->Add(IntToStr(stamina));
        saveList->Add(IntToStr(ACurrentQid));

        saveList->Add(L"[INVENTORY]");
        for (int i = 0; i < UserItems->Count; ++i)
        {
            int itemWeight = reinterpret_cast<intptr_t>(UserItems->Objects[i]);
            saveList->Add(UserItems->Strings[i] + L" " + IntToStr(itemWeight));
        }

        saveList->Add(L"[ENVIRONMENT]");
        for (int i = 0; i < EnvironmentItems->Count; ++i)
        {
            int envWeight = reinterpret_cast<intptr_t>(EnvironmentItems->Objects[i]);
            saveList->Add(EnvironmentItems->Strings[i] + L" " + IntToStr(envWeight));
        }

        saveList->SaveToFile(AFileName, TEncoding::UTF8);
        success = true;
    }
    catch (...)
    {
        Application->MessageBox(L"Не удалось записать файл сохранения.", L"The RPG", MB_OK | MB_ICONERROR);
    }

    delete saveList;
    return success;
}

__fastcall TfrmMainMenu::TfrmMainMenu(TComponent* Owner)
    : TForm(Owner)
{
    storage_system::initialize();

    storage_system::get().preload_creatures_json();
    storage_system::get().preload_story_json();

    if (!storage_system::get().validate_required_resources())
    {
        Application->MessageBox(
            L"Критические ресурсы игры отсутствуют или повреждены! Приложение будет закрыто.",
            L"Ошибка запуска",
            MB_OK | MB_ICONERROR
        );
        Application->Terminate();
        return;
    }

    save = new TStringList;
    User = new TUser;
    User->UserItems = new TStringList;
    User->EnvironmentItems = new TStringList;
    User->Clear();

    ExePath = ExtractFilePath(Application->ExeName);
}

__fastcall TfrmMainMenu::~TfrmMainMenu()
{
    if (User != nullptr)
    {
        delete User->UserItems;
        delete User->EnvironmentItems;
    }

    delete save;
    delete User;
}

void __fastcall TfrmMainMenu::NewClick(TObject* /*Sender*/)
{
    User->Clear();
    this->Hide();

    auto info_form = std::make_unique<TfrmCharGen>(this);

    if (info_form->ShowModal() == mrOk)
    {
        if (frmChapt == nullptr)
        {
            Application->CreateForm(__classid(TfrmChapt), &frmChapt);
        }

        frmChapt->Show();
        frmChapt->start_character_test();
    }
    else
    {
        this->Show();
    }
}

void __fastcall TfrmMainMenu::LoadClick(TObject* /*Sender*/)
{
    OpenDialog1->FileName = L"";
    OpenDialog1->InitialDir = ExePath;

    if (OpenDialog1->Execute())
    {
        if (User->LoadGame(OpenDialog1->FileName))
        {
            frmMainMenu->Hide();
            frmChapt->Show();
        }
    }
}

void __fastcall TfrmMainMenu::frmMainMenuCreate(TObject* /*Sender*/)
{
    lblVersion->Caption = L"Version " + APP_VERSION;
}

void __fastcall TfrmMainMenu::ExitClick(TObject* /*Sender*/)
{
    this->Close();
}

void __fastcall TfrmMainMenu::AboutClick(TObject* /*Sender*/)
{
    auto temporaryAbout = std::make_unique<TfrmAbout>(nullptr);
    temporaryAbout->ShowModal();
}
