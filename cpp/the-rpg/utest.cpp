#include <vcl.h>
#pragma hdrstop

#include "utest.h"
#include "first.h"
#include "chapt.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TfrmUTest *frmUTest;

__fastcall TfrmUTest::TfrmUTest(TComponent* Owner)
    : TForm(Owner)
{
    test = new TStringList;
    qptr = 0;
}

// Добавлен деструктор для очистки памяти и предотвращения утечек
__fastcall TfrmUTest::~TfrmUTest()
{
    delete test;
}

void TfrmUTest::ProcessSelection()
{
    int i;
    int istr = 0, idex = 0, imag = 0;
    wchar_t buf[255];

    for (i = 0; i < ListBox1->Items->Count; i++)
    {
        if (ListBox1->Selected[i])
        {
            swscanf(test->Strings[aptr + i].c_str(), L"%d %d %d", &istr, &idex, &imag);
            User->str += istr;
            User->dex += idex;
            User->mag += imag;

            if (qptr >= test->Count)
            {
                qptr = 0;
                swprintf(buf, 255, L"Сила = %d\nЛовкость = %d\nМагия = %d", User->str, User->dex, User->mag);
                Application->MessageBox(buf, L"Ваши умения", MB_OK);
                frmUTest->Close();
                frmChapt->Show();
                return;
            }

            LoadNext();
            return;
        }
    }
}

void __fastcall TfrmUTest::Button1Click(TObject *Sender)
{
    ProcessSelection(); // Вынесли дублирующийся код в отдельный метод
}

void __fastcall TfrmUTest::FormCreate(TObject *Sender)
{
    qptr = 0;
    test->LoadFromFile("test.txt", TEncoding::UTF8);
}

void __fastcall TfrmUTest::FormShow(TObject *Sender)
{
    LoadNext();
}

void __fastcall TfrmUTest::FormClose(TObject *Sender, TCloseAction &Action)
{
    Action = caFree;
    frmFirst->Show();
}

void TfrmUTest::LoadNext()
{
    wchar_t ch;
    wchar_t buf[255];
    wchar_t *tmp = nullptr;
    int istr, idex, imag;
    int i, j;
    String str;

    Memo1->Lines->Clear();
    ListBox1->Items->Clear();

    if (!((qptr < test->Count) && (test->Strings[qptr].Length() > 0)))
    {
        qptr++;
        return;
    }

    ch = test->Strings[qptr][1];
    switch (ch)
    {
        case L'{':
            while ((++qptr < test->Count) && (test->Strings[qptr].Length() > 0))
            {
                if (test->Strings[qptr][1] == L'}')
                {
                    break;
                }
                Memo1->Lines->Add(test->Strings[qptr]);
            }
            break;

        default:
            Memo1->Lines->Add(test->Strings[qptr]);
    }

    aptr = qptr + 1;
    while ((++qptr < test->Count) && (test->Strings[qptr].Length() > 0))
    {
        str = test->Strings[qptr];
        swscanf(str.c_str(), L"%d %d %d", &istr, &idex, &imag);
        str = str.Trim();
        wcscpy(buf, str.c_str());

        for (i = 0, j = 0; i < wcslen(buf); i++)
        {
            if ((buf[i] == L' ') && (j < 3))
            {
                tmp = &buf[i + 1];
                j++;
            }
        }
        ListBox1->Items->Add(tmp);
    }
    qptr++;
}

void __fastcall TfrmUTest::ListBox1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_RETURN)
    {
        ProcessSelection();
    }
}
