#include <vcl.h>
#pragma hdrstop

#include "uinfo.h"
#include "first.h"
#include "utest.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TfrmUInfo *frmUInfo;

__fastcall TfrmUInfo::TfrmUInfo(TComponent* Owner)
    : TForm(Owner)
{
}

TRaceData TfrmUInfo::ParseRaceLine(const String& ALine)
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

void __fastcall TfrmUInfo::Button1Click(TObject *Sender)
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

    // Данные валидны, сохраняем в объект игрока
    User->Name = Edit1->Text;
    User->CrType = ComboBox1->Text;
    User->SexType = ComboBox2->Text;

    TStringList *lCrType = new TStringList;
    String CrtPath = ExePath + L"data\\crt.txt";

    if (FileExists(CrtPath))
    {
        lCrType->LoadFromFile(CrtPath, TEncoding::UTF8);

        for (int i = 0; i < lCrType->Count; i++)
        {
            String fileLineLower = LowerCase(lCrType->Strings[i]);
            String userRaceLower = LowerCase(User->CrType);

            if (fileLineLower.Pos(userRaceLower) > 0)
            {
                TRaceData race = ParseRaceLine(lCrType->Strings[i]);

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

    // Скрываем текущую форму, чтобы она не мешала переходу
    this->Hide();

    // Безопасное создание и отображение формы теста
    if (frmUTest != nullptr)
    {
        delete frmUTest;
        frmUTest = nullptr;
    }
    Application->CreateForm(__classid(TfrmUTest), &frmUTest);
    frmUTest->Show();
    wchar_t debugBuf[1024];

    swprintf(debugBuf, 1024,
    L"--- ХАРАКТЕРИСТИКИ ПЕРСОНАЖА ---\n"
    L"Раса: %s\n"
    L"Пол: %s\n"
    L"Сила (str): %d\n"
    L"Ловкость (dex): %d\n"
    L"Магия (mag): %d\n"
    L"Здоровье (hlth): %d\n"
    L"Мана (man): %d\n"
    L"Выносливость (s): %d\n"
    L"Макс. вес (maxWeight): %d\n"
    L"--------------------------------",
    User->CrType.c_str(),
    User->SexType.c_str(),
    User->str,
    User->dex,
    User->mag,
    User->hlth,
    User->man,
    User->s,
    User->maxWeight
    );

    OutputDebugString(debugBuf);
    String testMsg = L"Клик по кнопке ОК. Выбрана раса: " + User->CrType;
    OutputDebugString(testMsg.c_str());
    // Теперь, когда тест открыт, закрываем (и уничтожаем через caFree) окно ввода инфо
    frmUInfo->Close();
    ModalResult = mrOk;
}

void __fastcall TfrmUInfo::Edit1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_RETURN)
    {
        ComboBox1->SetFocus();
    }
}

void __fastcall TfrmUInfo::Edit2KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_RETURN)
    {
        Button1->SetFocus();
    }
}

void __fastcall TfrmUInfo::frmUInfoClose(TObject *Sender, TCloseAction &Action)
{
    Action = caFree;

    // Если окно закрыли крестиком (а не перешли к тесту), возвращаем игрока в главное меню
    if (!frmUTest || !frmUTest->Visible)
    {
        frmFirst->Show();
    }
}
void __fastcall TfrmUInfo::FormDestroy(TObject *Sender)
{
    frmUInfo = nullptr;
}

void __fastcall TfrmUInfo::FormShow(TObject *Sender)
{
    ComboBox1->Items->Clear();

    TStringList *lCrType = new TStringList;
    String CrtPath = ExePath + L"data\\crt.txt";

    if (FileExists(CrtPath))
    {
        lCrType->LoadFromFile(CrtPath, TEncoding::UTF8);

        for (int i = 0; i < lCrType->Count; i++)
        {
            TRaceData race = ParseRaceLine(lCrType->Strings[i]);

            // Что распарсил ParseRaceLine для каждой строки
            wchar_t lineLog[512];
            swprintf(lineLog, 512, L"[ОТЛАДКА] Строка %d | Исходная: '%s' | Имя: '%s' | Модификатор: %d",
                     i, lCrType->Strings[i].c_str(), race.Name.c_str(), race.Modifier);
            OutputDebugString(lineLog);

            if (!race.Name.IsEmpty())
            {
                ComboBox1->Items->Add(race.Name);
            }
        }
    }
    else
    {
        OutputDebugString(L"crt.txt not found!");
    }

    if (ComboBox1->Items->Count == 0)
    {
        OutputDebugString(L"[ОТЛАДКА] Запуск аварийного заполнения базовыми расами");
        ComboBox1->Items->Add(L"Человек");
        ComboBox1->Items->Add(L"Эльф");
    }

    ComboBox1->ItemIndex = 0;
    delete lCrType;
}
