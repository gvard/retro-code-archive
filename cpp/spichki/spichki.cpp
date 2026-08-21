#include <vcl.h>
#pragma hdrstop

#include "spichki.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;

int n = 0, i, j = 0, x = 0;

__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
}

void __fastcall TForm1::FormCreate(TObject *Sender)
{
    randomize();
    x = 25 + random(6);

    for (i = 0; i < x; i++)
    {
        MyShape[i] = new TShape(0);
        MyShape[i]->Parent = this;
        MyShape[i]->Top = 50;
        MyShape[i]->Width = 20;
        MyShape[i]->Height = 300;
        MyShape[i]->Left = 200 - (x - 25) * 20 + 40 * i;
    }

    Memo1->Clear();
    Memo2->Clear();

    RadioButton1->Enabled = true;
    RadioButton2->Enabled = true;
    RadioButton3->Enabled = false;
    RadioButton4->Enabled = false;

    n = 0;
    j = 0;
}

void __fastcall TForm1::KeyPress(TObject *Sender, char &Key)
{
    if ((Key == '1' || Key == '2') && (j + StrToInt(Key) <= x))
    {
        int countToRemove = StrToInt(Key);

        for (i = j; i < j + countToRemove; i++)
        {
            if (MyShape[i] != nullptr)
            {
                delete MyShape[i];
                MyShape[i] = nullptr;
            }
        }

        j += countToRemove;

        if (n % 2 == 0)
        {
            if (Key == '1')
            {
                RadioButton1->Checked = false;
            }
            if (Key == '2')
            {
                RadioButton2->Checked = false;
            }

            RadioButton1->Enabled = false;
            RadioButton2->Enabled = false;
            RadioButton3->Enabled = true;
            RadioButton4->Enabled = true;
            Memo1->Lines->Add(Key);
        }
        else
        {
            if (Key == '1')
            {
                RadioButton3->Checked = false;
            }
            if (Key == '2')
            {
                RadioButton4->Checked = false;
            }

            RadioButton1->Enabled = true;
            RadioButton2->Enabled = true;
            RadioButton3->Enabled = false;
            RadioButton4->Enabled = false;
            Memo2->Lines->Add(Key);
        }

        n++;

        if (j >= x)
        {
            if (n % 2 == 0)
            {
                ShowMessage(L"Выиграл второй игрок!");
            }
            else
            {
                ShowMessage(L"Выиграл первый игрок!");
            }
            FormCreate(Form1);
        }
    }
}

void __fastcall TForm1::Click(TObject *Sender)
{
    char KeyValue;

    if (n % 2 == 0)
    {
        if (RadioButton1->Checked == true)
        {
            KeyValue = '1';
            KeyPress(Form1, KeyValue);
        }
        else
        {
            KeyValue = '2';
            KeyPress(Form1, KeyValue);
        }
    }
    else
    {
        if (RadioButton3->Checked == true)
        {
            KeyValue = '1';
            KeyPress(Form1, KeyValue);
        }
        else
        {
            KeyValue = '2';
            KeyPress(Form1, KeyValue);
        }
    }
}

void __fastcall TForm1::Click1(TObject *Sender)
{
    Form1->Close();
}
