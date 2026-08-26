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

__fastcall TfrmFirst::TfrmFirst(TComponent* Owner)
    : TForm(Owner)
{
    save = new TStringList;
    User = new TUser;
    User->Clear();
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
    String FileName;
    if (OpenDialog1->Execute())
    {
        save->LoadFromFile(OpenDialog1->FileName, TEncoding::UTF8);
    }
    else
    {
        return;
    }

    // Защита от поврежденных/пустых файлов сохранений
    if (save->Count < 9)
    {
        Application->MessageBox(L"Извините, но сейв слишком короткий или пустой.", L"The RPG", MB_OK | MB_ICONERROR);
        return;
    }

    int x;
    User->Name = save->Strings[0];
    User->CrType = save->Strings[1];
    User->SexType = save->Strings[2];

    try
    {
        User->age = StrToInt(save->Strings[3]);
        User->str = StrToInt(save->Strings[4]);
        User->dex = StrToInt(save->Strings[5]);
        User->mag = StrToInt(save->Strings[6]);
        User->hlth = StrToInt(save->Strings[7]);
        x = StrToInt(save->Strings[8]);
    }
    catch (EConvertError&)
    {
        Application->MessageBox(L"Извините, но сейв глюченый.", L"The RPG", MB_OK | MB_ICONERROR);
        return;
    }

    frmChapt->LoadNext(x);
    frmFirst->Hide();
    frmChapt->Show();
}
