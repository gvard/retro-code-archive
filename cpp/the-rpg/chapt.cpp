#include <vcl.h>
#include <Winapi.Mmsystem.hpp>
#pragma hdrstop

#include "chapt.h"
#include "first.h"
#include "about.h"
#include "fight.h"
#include "utype.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TfrmChapt *frmChapt;

__fastcall TfrmChapt::TfrmChapt(TComponent* Owner)
    : TForm(Owner)
{
    save = new TStringList;
    chapter = new TStringList;
}

// Добавлен деструктор для предотвращения утечки памяти
__fastcall TfrmChapt::~TfrmChapt()
{
    delete save;
    delete chapter;
}

void __fastcall TfrmChapt::FormCreate(TObject *Sender)
{
    chapter->LoadFromFile("chapt.txt", TEncoding::UTF8);
}

void __fastcall TfrmChapt::ExitClick(TObject *Sender)
{
    frmChapt->Close();
}

void TfrmChapt::LoadNext(int qid)
{
    wchar_t ch = 0;
    int q = 0;
    int qptr = -1;

    Memo1->Lines->Clear();
    ListBox1->Items->Clear();

    // Поиск нужного ID вопроса
    for (int i = 0; i < chapter->Count; i++)
    {
        ch = 0;
        q = -1;

        swscanf(chapter->Strings[i].c_str(), L"%d%c", &q, &ch);

        if (q == qid)
        {
            qptr = i;
            break;
        }

        if (ch == L'{')
        {
            while (++i < chapter->Count)
            {
                if (chapter->Strings[i].Length() > 0)
                {
                    if (chapter->Strings[i][1] == L'}')
                    {
                        break;
                    }
                }
            }
        }
        while ((++i < chapter->Count) && (chapter->Strings[i].Length() > 0));
    }

    if (qptr < 0)
    {
        Application->MessageBox(L"Вопрос не найден!", L"The RPG", MB_ICONEXCLAMATION | MB_OK);
        frmChapt->Hide();
        frmFirst->Show();
        return;
    }

    // Чтение заголовка/типа вопроса
    ch = 0;
    swscanf(chapter->Strings[qptr].c_str(), L"%d%c", &q, &ch);

    switch (ch)
    {
        case L'{':
            while (++qptr < chapter->Count)
            {
                if (chapter->Strings[qptr].Length() > 0)
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
            Application->CreateForm(__classid(TfrmFight), &frmFight);
            frmFight->qptr = qptr;
            PlaySound(L"fight.wav", NULL, SND_ASYNC);
            frmChapt->Hide();
            frmFight->Show();
            return;

        case L'e':
            Application->MessageBox(L"Игра окончена!", L"The RPG", MB_OK);
            frmChapt->Hide();
            frmFirst->Show();
            return;
    }

    // Заполнение вариантов ответов в ListBox1
    aptr = qptr + 1;
    while ((++qptr < chapter->Count) && (chapter->Strings[qptr].Length() > 0))
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

void __fastcall TfrmChapt::Button1Click(TObject *Sender)
{
    int i;
    int jump;
    for (i = 0; i < ListBox1->Items->Count; i++)
    {
        if (ListBox1->Selected[i])
        {
            swscanf(chapter->Strings[aptr + i].c_str(), L"%d ", &jump);
            User->Refresh();
            LoadNext(jump);
        }
    }
}

void __fastcall TfrmChapt::Help1Click(TObject *Sender)
{
    ShowMessage(L"Сами разберетесь!");
}

void __fastcall TfrmChapt::AboutClick(TObject *Sender)
{
    AboutBox->Show();
}

void __fastcall TfrmChapt::ListBox1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_RETURN)
    {
        int i;
        int jump;
        for (i = 0; i < ListBox1->Items->Count; i++)
        {
            if (ListBox1->Selected[i])
            {
                swscanf(chapter->Strings[aptr + i].c_str(), L"%d ", &jump);
                User->Refresh();
                LoadNext(jump);
            }
        }
    }
}

void __fastcall TfrmChapt::frmChaptCloseQuery(TObject *Sender, bool &CanClose)
{
    switch (Application->MessageBox(L"Вы уверены?!", L"The RPG", MB_YESNOCANCEL + MB_ICONQUESTION))
    {
        case IDYES:
            frmFirst->Show();
            frmChapt->Hide();
            CanClose = true; // Исправлено: без этого форма не закрывалась
            break;

        case IDCANCEL:
            CanClose = false;
            break;

        case IDNO:
            CanClose = false;
            break;
    }
}

void __fastcall TfrmChapt::LoadClick(TObject *Sender)
{
    String FileName;
    if (OpenDialog1->Execute())
    {
        save->LoadFromFile(OpenDialog1->FileName);
    }
    else
    {
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
        Application->MessageBox(L"Извините, но сейв глюченый.", L"The RPG", MB_OK);
        return;
    }
    frmChapt->LoadNext(x);
}

void __fastcall TfrmChapt::SaveClick(TObject *Sender)
{
    String FileName;
    if (SaveDialog1->Execute())
    {
        // User->SavetoFile(SaveDialog1->FileName);
    }
}

void __fastcall TfrmChapt::InvClick(TObject *Sender)
{
    ShowMessage(L"Еще не работает.");
}

void __fastcall TfrmChapt::ustype1Click(TObject *Sender)
{
    Application->CreateForm(__classid(TfrmUType), &frmUType);
    frmUType->Show();
}
