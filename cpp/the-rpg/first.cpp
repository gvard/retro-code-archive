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
    if (GroundItems == nullptr) GroundItems = new TStringList;

    UserItems->Clear();
    GroundItems->Clear();

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
            if (UserItems != nullptr)   UserItems->Clear();
            if (GroundItems != nullptr) GroundItems->Clear();

            if (UserItems != nullptr)
            {
                UserItems->Add(L"__INIT_NEW_GAME__");
            }

            if (DualListDlg != nullptr)
            {
                DualListDlg->SrcList->Items->Clear();
                DualListDlg->DstList->Items->Clear();
            }

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

            frmChapt->LoadNext(nextChapter);

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
    User->GroundItems = new TStringList;
    User->Clear();

    ExePath = ExtractFilePath(Application->ExeName);
}

__fastcall TfrmFirst::~TfrmFirst()
{
    if (User != nullptr)
    {
        delete User->UserItems;
        delete User->GroundItems;
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
