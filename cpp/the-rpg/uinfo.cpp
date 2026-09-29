#include <Winapi.Windows.hpp>
#include <cstdio>
#include <cwchar>

#include "uinfo.h"
#include "first.h"

#pragma resource "*.dfm"

__fastcall TfrmUInfo::TfrmUInfo(TComponent* Owner)
    : TForm(Owner)
{
}

TRaceData TfrmUInfo::parse_race_line(const String& ALine)
{
    TRaceData data;
    data.Name = L"";
    data.Modifier = 0;

    String currentLine = ALine.Trim();
    if (currentLine.IsEmpty()) return data;

    int lastSpace = currentLine.LastDelimiter(L" ");
    if (lastSpace > 0)
    {
        data.Name = currentLine.SubString(1, lastSpace - 1).Trim();
        String modStr = currentLine.SubString(lastSpace + 1, currentLine.Length() - lastSpace).Trim();

        if (modStr.Pos(L"+") == 1)
        {
            modStr = modStr.SubString(2, modStr.Length() - 1);
        }
        data.Modifier = StrToIntDef(modStr, 0);
    }
    else
    {
        data.Name = currentLine;
    }
    return data;
}

void __fastcall TfrmUInfo::Button1Click(TObject* /*Sender*/)
{
    try
    {
        User->age = StrToInt(Edit2->Text);
    }
    catch (EConvertError&)
    {
        Application->MessageBox(L"Возраст задается в ДЕСЯТИЧНОЙ системе исчисления!", L"The RPG", MB_OK | MB_ICONWARNING);
        Edit2->SetFocus();
        return;
    }

    if (User->age > 100)
    {
        Application->MessageBox(L"Столько не живут!", L"The RPG", MB_OK | MB_ICONWARNING);
        Edit2->SetFocus();
        return;
    }
    if (User->age < 15)
    {
        Application->MessageBox(L"Такой маленький, а уже ноги чешутся из дому смотаться?!", L"The RPG", MB_OK | MB_ICONWARNING);
        Edit2->SetFocus();
        return;
    }
    if (User->age > 70)
    {
        Application->MessageBox(L"А по дороге не развалишься?!", L"The RPG", MB_OK | MB_ICONWARNING);
        Edit2->SetFocus();
        return;
    }

    User->Name = Edit1->Text;
    User->CrType = ComboBox1->Text;
    User->SexType = ComboBox2->Text;

    TStringList *lCrType = new TStringList;
    String CrtPath = ExePath + L"data\\crt.txt";

    if (FileExists(CrtPath))
    {
        lCrType->LoadFromFile(CrtPath, TEncoding::UTF8);

        for (int i = 0; i < lCrType->Count; ++i)
        {
            String fileLineLower = LowerCase(lCrType->Strings[i]);
            String userRaceLower = LowerCase(User->CrType);

            if (fileLineLower.Pos(userRaceLower) > 0)
            {
                TRaceData race = parse_race_line(lCrType->Strings[i]);
                User->str += race.Modifier;

                wchar_t logBuf[256];
                swprintf(logBuf, 256, L"Отладка (uinfo): Раса: %s, Модификатор силы: %d, Итоговая сила: %d",
                         User->CrType.c_str(), race.Modifier, User->str);
                OutputDebugString(logBuf);
                break;
            }
        }
    }
    delete lCrType;

    this->ModalResult = mrOk;
}

void __fastcall TfrmUInfo::Edit1KeyDown(TObject* /*Sender*/, WORD &Key, TShiftState /*Shift*/)
{
    if (Key == VK_RETURN)
    {
        ComboBox1->SetFocus();
    }
}

void __fastcall TfrmUInfo::Edit2KeyDown(TObject* /*Sender*/, WORD &Key, TShiftState /*Shift*/)
{
    if (Key == VK_RETURN)
    {
        Button1->SetFocus();
    }
}

void __fastcall TfrmUInfo::FormShow(TObject* /*Sender*/)
{
    ComboBox1->Items->Clear();

    TStringList *lCrType = new TStringList;
    String CrtPath = ExePath + L"data\\crt.txt";

    if (FileExists(CrtPath))
    {
        lCrType->LoadFromFile(CrtPath, TEncoding::UTF8);

        for (int i = 0; i < lCrType->Count; ++i)
        {
            TRaceData race = parse_race_line(lCrType->Strings[i]);

            if (!race.Name.IsEmpty())
            {
                ComboBox1->Items->Add(race.Name);
            }
        }
    }
    delete lCrType;

    if (ComboBox1->Items->Count == 0)
    {
        ComboBox1->Items->Add(L"Человек");
        ComboBox1->Items->Add(L"Эльф");
    }

    ComboBox1->ItemIndex = 0;
}
