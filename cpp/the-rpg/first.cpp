#include <vcl.h>
#pragma hdrstop

#include "first.h"
#include "about.h"
#include "uinfo.h"
#include "chapt.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TfrmFirst *frmFirst;
TUser *User;
String ExePath;

__fastcall TfrmFirst::TfrmFirst(TComponent* Owner)
    : TForm(Owner)
{
    save = new TStringList;
    User = new TUser;
    User->Clear();
	ExePath = ExtractFilePath(Application->ExeName);
}

// Добавлен деструктор для очистки глобальных объектов и предотвращения утечек памяти
__fastcall TfrmFirst::~TfrmFirst()
{
    delete save;
    delete User;
}

void __fastcall TfrmFirst::NewClick(TObject *Sender)
{
    User->Clear();
    frmChapt->LoadNext(1);
    frmFirst->Hide();

    // Безопасное пересоздание формы создания персонажа без утечек памяти
    if (frmUInfo != nullptr)
    {
        delete frmUInfo;
        frmUInfo = nullptr;
    }

    Application->CreateForm(__classid(TfrmUInfo), &frmUInfo);
    frmUInfo->Show();
}

void __fastcall TfrmFirst::ExitClick(TObject *Sender)
{
    frmFirst->Close();
}

void __fastcall TfrmFirst::AboutClick(TObject *Sender)
{
    AboutBox->Show();
}

void TUser::Clear()
{
    str = 5;
    dex = 5;
    mag = 5;

    hlth = 100;
    man = 100;
    s = 100;
}

void TUser::Refresh()
{
    hlth += 5;
    s += 5;
    man += 5;

    if (hlth > 100)
    {
        hlth = 100;
    }
    if (s > 100)
    {
        s = 100;
    }
    if (man > 100)
    {
        man = 100;
    }
}

void __fastcall TfrmFirst::frmFirstCreate(TObject *Sender)
{
    Label2->Caption = L"Version " + APP_VERSION;
}

void __fastcall TfrmFirst::LoadClick(TObject *Sender)
{
    OpenDialog1->FileName = L"";
    OpenDialog1->InitialDir = ExpandFileName(ExePath);
	OpenDialog1->InitialDir = ExePath;

	String FileName;
    if (OpenDialog1->Execute())
    {
        User->LoadGame(OpenDialog1->FileName);
    }
    else
    {
        return;
    }
    frmFirst->Hide();
    frmChapt->Show();
}

bool TUser::LoadGame(const String& AFileName)
{
    TStringList* saveList = new TStringList;
    bool success = false;

    try
    {
        saveList->LoadFromFile(AFileName, TEncoding::UTF8);

        if (saveList->Count >= 10)
        {
            Name = saveList->Strings[0];
            CrType = saveList->Strings[1];
            SexType = saveList->Strings[2];

            age = StrToInt(saveList->Strings[3]);
            str = StrToInt(saveList->Strings[4]);
            dex = StrToInt(saveList->Strings[5]);
            mag = StrToInt(saveList->Strings[6]);
            hlth = StrToInt(saveList->Strings[7]);
            man = StrToInt(saveList->Strings[8]);

            int nextChapter = StrToInt(saveList->Strings[9]);

            // Загружаем главу в форму квестов
            frmChapt->LoadNext(nextChapter);
            success = true;
        }
        else
        {
            Application->MessageBox(L"Извините, но сейв слишком короткий или пустой.", L"The RPG", MB_OK | MB_ICONERROR);
        }
    }
    catch (EConvertError&)
    {
        Application->MessageBox(L"Извините, но сейв глюченый.", L"The RPG", MB_OK | MB_ICONERROR);
    }
    catch (...)
    {
        Application->MessageBox(L"Не удалось прочитать файл сохранения.", L"The RPG", MB_OK | MB_ICONERROR);
    }

    delete saveList; // Чистим память локально, убираем утечки
    return success;
}
