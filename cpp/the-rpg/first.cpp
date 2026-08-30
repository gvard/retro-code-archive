#include <vcl.h>
#pragma hdrstop

#include "first.h"
#include "about.h"
#include "uinfo.h"
#include "chapt.h"
#include "invent.h"
#include "utype.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TfrmFirst *frmFirst;
TUser *User;
String ExePath;

// =========================================================================
// КЛАСС СУЩНОСТИ ИГРОКА (TUser)
// =========================================================================

void TUser::Clear()
{
    Name = L"";
    CrType = L"";
    SexType = L"";
    age = 0;
    str = 5;
    dex = 5;
    mag = 5;
    hlth = 100;
    man = 100;
    maxWeight = 0;

    // Безопасное выделение памяти под списки предметов, если они еще не созданы
    if (UserItems == nullptr)   UserItems = new TStringList;
    if (EnvironmentItems == nullptr) EnvironmentItems = new TStringList;

    UserItems->Clear();
    EnvironmentItems->Clear();

    // Запись стартового маркера инициализации
    UserItems->Add(L"__INIT_NEW_GAME__");

    s = GetMaxStamina();
}

void TUser::Refresh()
{
    hlth += 5;
    man += 5;
    s += 5;

    if (hlth > 100) hlth = 100;
    if (man > 100)  man = 100;

    int maxStamina = GetMaxStamina();
    if (s > maxStamina) s = maxStamina;
}

int TUser::GetMaxStamina()
{
    // [Core Mechanic] Returns maximum stamina based on Strength and Dexterity
    return 50 + (str * 4) + (dex * 2);
}

void TUser::RecalculateStamina(int totalWeight)
{
    int maxStamina = GetMaxStamina();

    if (totalWeight > maxWeight)
    {
        s = maxStamina - 20; // Штраф -20 единиц при перегрузе
    }
    else
    {
        s = maxStamina;
    }
}

bool TUser::LoadGame(const String& AFileName)
{
    TStringList* saveList = new TStringList;
    bool success = false;

    try
    {
        saveList->LoadFromFile(AFileName, TEncoding::UTF8);

        if (saveList->Count >= 11)
        {
            // Создаем временные независимые списки для чтения вещей из файла.
            TStringList* tempUserItems = new TStringList;
            TStringList* tempEnvironmentItems = new TStringList;

            Name    = saveList->Strings[0];
            CrType  = saveList->Strings[1];
            SexType = saveList->Strings[2];
            age     = StrToInt(saveList->Strings[3]);
            str     = StrToInt(saveList->Strings[4]);
            dex     = StrToInt(saveList->Strings[5]);
            mag     = StrToInt(saveList->Strings[6]);
            hlth    = StrToInt(saveList->Strings[7]);
            man     = StrToInt(saveList->Strings[8]);
            s       = StrToInt(saveList->Strings[9]);

            int nextChapter = StrToInt(saveList->Strings[10]);

            TStringList* currentTargetList = nullptr;

            for (int i = 11; i < saveList->Count; i++)
            {
                String currentLine = saveList->Strings[i].Trim();
                if (currentLine.IsEmpty()) continue;

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

                        currentTargetList->AddObject(itemName, (TObject*)itemWeight);
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

            // Все скрытые формы инициализировались и затихли.
            // Очищаем списки TUser от любого мусора (включая строку __INIT_NEW_GAME__)
            if (UserItems != nullptr)        UserItems->Clear();
            if (EnvironmentItems != nullptr) EnvironmentItems->Clear();

            // Переносим вещи из временной безопасной памяти в постоянные структуры TUser
            if (UserItems != nullptr)        UserItems->AddStrings(tempUserItems);
            if (EnvironmentItems != nullptr) EnvironmentItems->AddStrings(tempEnvironmentItems);

            // Удаляем временные контейнеры, закрывая утечки памяти Windows
            delete tempUserItems;
            delete tempEnvironmentItems;

            // Инвентарь для текущей сессии официально и успешно восстановлен
            this->isInventoryLoaded = true;

            if (frmUType != nullptr && frmUType->Visible)
            {
                frmUType->UpdateStaminaDisplay();
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

bool TUser::SaveGame(const String& AFileName, int ACurrentQid)
{
    TStringList* saveList = new TStringList;
    bool success = false;

    try
    {
        saveList->Add(Name);
        saveList->Add(CrType);
        saveList->Add(SexType);
        saveList->Add(IntToStr(age));
        saveList->Add(IntToStr(str));
        saveList->Add(IntToStr(dex));
        saveList->Add(IntToStr(mag));
        saveList->Add(IntToStr(hlth));
        saveList->Add(IntToStr(man));
        saveList->Add(IntToStr(s));
        saveList->Add(IntToStr(ACurrentQid));

        saveList->Add(L"[INVENTORY]");
        for (int i = 0; i < UserItems->Count; i++)
        {
            int itemWeight = (int)(UserItems->Objects[i]);
            saveList->Add(UserItems->Strings[i] + L" " + IntToStr(itemWeight));
        }

        saveList->Add(L"[ENVIRONMENT]");
        for (int i = 0; i < EnvironmentItems->Count; i++)
        {
            int itemWeight = (int)(EnvironmentItems->Objects[i]);
            saveList->Add(EnvironmentItems->Strings[i] + L" " + IntToStr(itemWeight));
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

// =========================================================================
// ГЛАВНОЕ МЕНЮ ИГРЫ (TfrmFirst)
// =========================================================================

__fastcall TfrmFirst::TfrmFirst(TComponent* Owner)
    : TForm(Owner)
{
    save = new TStringList;

    User = new TUser;
    User->UserItems = new TStringList;
    User->EnvironmentItems = new TStringList;
    User->Clear();

    ExePath = ExtractFilePath(Application->ExeName);
}

__fastcall TfrmFirst::~TfrmFirst()
{
    if (User != nullptr)
    {
        delete User->UserItems;
        delete User->EnvironmentItems;
    }

    delete save;
    delete User;
}

void __fastcall TfrmFirst::NewClick(TObject *Sender)
{
    User->Clear();
    frmFirst->Hide();

    if (frmUInfo != nullptr)
    {
        delete frmUInfo;
        frmUInfo = nullptr;
    }

    Application->CreateForm(__classid(TfrmUInfo), &frmUInfo);
    frmUInfo->Show();
}

void __fastcall TfrmFirst::LoadClick(TObject *Sender)
{
    OpenDialog1->FileName = L"";
    OpenDialog1->InitialDir = ExePath;

    if (OpenDialog1->Execute())
    {
        if (User->LoadGame(OpenDialog1->FileName))
        {
            frmFirst->Hide();
            frmChapt->Show();
        }
    }
}

void __fastcall TfrmFirst::frmFirstCreate(TObject *Sender)
{
    Label2->Caption = L"Version " + APP_VERSION;
}

void __fastcall TfrmFirst::ExitClick(TObject *Sender)
{
    this->Close();
}

void __fastcall TfrmFirst::AboutClick(TObject *Sender)
{
    AboutBox->Show();
}
