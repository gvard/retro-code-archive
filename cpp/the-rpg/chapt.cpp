#include <Winapi.Mmsystem.hpp>
#include <string_view>
#include <memory>
#include <cstdio>

#include "chapt.h"
#include "first.h"
#include "about.h"
#include "fight.h"
#include "utype.h"
#include "invent.h"

#pragma resource "*.dfm"

TfrmChapt* frmChapt;

__fastcall TfrmChapt::TfrmChapt(TComponent* Owner)
    : TForm(Owner)
{
    save = new TStringList;
    chapter = new TStringList;
    hasUnsavedChanges = false;
}

__fastcall TfrmChapt::~TfrmChapt()
{
    delete save;
    delete chapter;
}

void __fastcall TfrmChapt::FormCreate(TObject* Sender)
{
    chapter->LoadFromFile(ExePath + L"data\\chapt.txt", TEncoding::UTF8);
}

void __fastcall TfrmChapt::ExitClick(TObject* Sender)
{
    frmChapt->Close();
}

void __fastcall TfrmChapt::ListBox1DblClick(TObject* Sender)
{
    Button1Click(Sender);
}

void TfrmChapt::LoadNext(int qid)
{
    // Глава изменилась — сбрасываем состояние вещей на земле
    if (this->currentQid != qid && User->EnvironmentItems != nullptr)
    {
        User->EnvironmentItems->Clear();
    }

    this->currentQid = qid;

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
        frmFirst->Show();
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
            Application->CreateForm(__classid(TfrmFight), &frmFight);
            frmFight->qptr = qptr;
            PlaySound((ExePath + L"sound\\fight.wav").c_str(), NULL, SND_ASYNC);
            frmChapt->Hide();
            frmFight->Show();
            return;

        case L'e':
            Application->MessageBox(L"Игра окончена!", L"The RPG", MB_OK);
            frmChapt->Hide();
            frmFirst->Show();
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

void __fastcall TfrmChapt::Button1Click(TObject* Sender)
{
    int jump;
    for (int i = 0; i < ListBox1->Items->Count; i++)
    {
        if (ListBox1->Selected[i])
        {
            swscanf(chapter->Strings[aptr + i].c_str(), L"%d ", &jump);
            User->Refresh();
            this->hasUnsavedChanges = true;
            LoadNext(jump);
            return;
        }
    }
}

void __fastcall TfrmChapt::Help1Click(TObject* Sender)
{
    ShowMessage(L"Сами разберетесь!");
}

void __fastcall TfrmChapt::AboutClick(TObject* Sender)
{
    auto temporaryAbout = std::make_unique<TAboutBox>(nullptr);
    temporaryAbout->ShowModal();
}

void __fastcall TfrmChapt::ListBox1KeyDown(TObject* Sender, WORD& Key, TShiftState Shift)
{
    if (Key == VK_RETURN)
    {
        Button1Click(Sender);
    }
}

void __fastcall TfrmChapt::frmChaptCloseQuery(TObject* Sender, bool& CanClose)
{
    if (!this->hasUnsavedChanges)
    {
        if (frmFirst)
        {
            frmFirst->Show();
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
        if (frmFirst)
        {
            frmFirst->Show();
        }
        this->Hide();
        CanClose = true;
    }
    else
    {
        CanClose = false;
    }
}

void __fastcall TfrmChapt::LoadClick(TObject* Sender)
{
    OpenDialog1->FileName = L"";
    OpenDialog1->InitialDir = ExpandFileName(ExePath);
    // OpenDialog1->Filter = L"Файлы сохранений (*.sav)|*.sav|Все файлы (*.*)|*.*";

    if (OpenDialog1->Execute())
    {
        User->LoadGame(OpenDialog1->FileName);
    }
}

void __fastcall TfrmChapt::SaveClick(TObject* Sender)
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

void __fastcall TfrmChapt::InvClick(TObject* /*Sender*/)
{
    auto inventory_dialog = std::make_unique<TDualListDlg>(this);
    inventory_dialog->ShowModal();
}

void __fastcall TfrmChapt::ustype1Click(TObject* /*Sender*/)
{
    auto stats_form = std::make_unique<TfrmUType>(this);
    stats_form->ShowModal();
}

void __fastcall TfrmChapt::FormResize(TObject* Sender)
{
    Button1->Left = (ClientWidth - Button1->Width) / 2;
}

void __fastcall TfrmChapt::FormKeyDown(TObject* Sender, WORD& Key, TShiftState Shift)

{
    if (frmFirst != nullptr && frmFirst->ActionList1 != nullptr)
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

        TWMKey msg;
        msg.Msg = WM_KEYDOWN;
        msg.CharCode = Key;
        msg.KeyData = 0;

        if (frmFirst->ActionList1->IsShortCut(msg))
        {
            Key = 0;
        }
    }
}
