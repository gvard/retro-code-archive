#include <vcl.h>
#include <Winapi.Mmsystem.hpp>
#pragma hdrstop

#include <algorithm>
#include "fight.h"
#include "first.h"
#include "chapt.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TfrmFight *frmFight;

__fastcall TfrmFight::TfrmFight(TComponent* Owner)
    : TForm(Owner)
{
}

void __fastcall TfrmFight::FormCreate(TObject *Sender)
{
    int i;
    int hit, mana, apt;
    TStringList *lWeapon = new TStringList;
    lWeapon->LoadFromFile("weap.txt", TEncoding::UTF8);

    grEnemy->Cells[1][0] = "Вид противника";
    grEnemy->Cells[2][0] = "Имя";
    grEnemy->Cells[3][0] = "Жизнь";
    grEnemy->Cells[4][0] = "Хиты";

    // Увеличиваем ширину 3-й колонки, чтобы текст не обрезался
    grEnemy->ColWidths[3] = 70;

    grWeapon->Cells[1][0] = "Оружие";
    grWeapon->Cells[2][0] = "Хиты";
    grWeapon->Cells[3][0] = "Мана";
    grWeapon->Cells[4][0] = "ActPts";

    grWeapon->RowCount = std::max(2, lWeapon->Count / 2 + 1);
    for (i = 0; i < lWeapon->Count; i += 2)
    {
        swscanf(lWeapon->Strings[i].c_str(), L"%d%d%d", &hit, &mana, &apt);

        grWeapon->Cells[1][i / 2 + 1] = lWeapon->Strings[i + 1];
        grWeapon->Cells[2][i / 2 + 1] = hit;
        grWeapon->Cells[3][i / 2 + 1] = mana;
        grWeapon->Cells[4][i / 2 + 1] = apt;
    }

    delete lWeapon;
}

void __fastcall TfrmFight::FormShow(TObject *Sender)
{
    AnsiString str;
    int i, j, qid, count, type, h;
    char ch;
    char buf[256];
    char *tmp = nullptr;

    TStringList *lChapter = new TStringList;
    TStringList *lCrType = new TStringList;
    lChapter->LoadFromFile("chapt.txt", TEncoding::UTF8);
    lCrType->LoadFromFile("crt.txt", TEncoding::UTF8);

    selEnemy = 0;
    selWeapon = 0;
    updateApt();
    this->update();

    str = lChapter->Strings[qptr++];
    str.Trim();
    strcpy(buf, str.c_str());
    sscanf(buf, "%d%c %d", &qid, &ch, &jump);

    for (i = 0, j = 0; i < strlen(buf); i++)
    {
        if (buf[i] == ' ')
        {
            j++;
            if (j >= 2)
            {
                tmp = &buf[i + 1];
                break;
            }
        }
    }

    frmFight->Caption = tmp;
    for (count = 0; qptr < lChapter->Count; qptr++, count++)
    {
        str = lChapter->Strings[qptr];
        strcpy(buf, str.c_str());
        if (str.Length() <= 0)
        {
            break;
        }

        sscanf(buf, "%d %d", &type, &h);
        for (i = 0, j = 0; i < strlen(buf); i++)
        {
            if (buf[i] == ' ')
            {
                j++;
                if (j >= 2)
                {
                    tmp = &buf[i + 1];
                    break;
                }
            }
        }

        grEnemy->Cells[0][count + 1] = " ";
        grEnemy->Cells[1][count + 1] = lCrType->Strings[type - 1];
        grEnemy->Cells[2][count + 1] = tmp;
        grEnemy->Cells[3][count + 1] = 100;
        grEnemy->Cells[4][count + 1] = h;
    }

    grEnemy->RowCount = count + 1;
    for (i = 1; i < grWeapon->RowCount; i++)
    {
        grWeapon->Cells[0][i] = " ";
    }

    grEnemy->Cells[0][1] = ">";
    grWeapon->Cells[0][1] = ">";

    delete lChapter;
    delete lCrType;
}

void TfrmFight::updateApt()
{
    apt = User->dex * User->s / 100;
    if (apt <= 0)
    {
        opponentAttack();
    }
}

void TfrmFight::update()
{
    wchar_t buf[256];

    swprintf(buf, 256, L"Health: %d", User->hlth);
    sbBar->Panels->Items[0]->Text = buf;

    swprintf(buf, 256, L"Mana: %d", User->man);
    sbBar->Panels->Items[1]->Text = buf;

    swprintf(buf, 256, L"Stam: %d", User->s);
    sbBar->Panels->Items[2]->Text = buf;

    swprintf(buf, 256, L"Action points: %d", apt);
    sbBar->Panels->Items[3]->Text = buf;
}

void TfrmFight::opponentAttack()
{
    wchar_t buf[256];
    int hit, i;

    if (check())
    {
        return;
    }

    hit = 0;
    for (i = 1; i < grEnemy->RowCount; i++)
    {
        if (grEnemy->Cells[3][i].ToInt() > 0)
        {
            hit += grEnemy->Cells[4][i].ToInt();
        }
    }

    swprintf(buf, 256, L"противник нанес вам удар: %d", hit);
    Application->MessageBox(buf, frmFight->Caption.c_str(), MB_OK);

    User->hlth -= hit;
    User->s -= hit;

    User->Refresh();

    if (check())
    {
        return;
    }

    updateApt();
    this->update();
}

bool TfrmFight::check()
{
    int i;
    bool endFight = true;

    if (grEnemy->Cells[3][selEnemy + 1].ToInt() < 0)
    {
        grEnemy->Cells[3][selEnemy + 1] = "0";
    }

    if (grEnemy->Cells[3][selEnemy + 1].ToInt() <= 0 ||
        grWeapon->Cells[3][selWeapon + 1].ToInt() > User->man ||
        grWeapon->Cells[4][selWeapon + 1].ToInt() > apt)
    {
        btnAttack->Enabled = false;
    }
    else
    {
        btnAttack->Enabled = true;
    }

    for (i = 1; i < grEnemy->RowCount; i++)
    {
        if (grEnemy->Cells[3][i].ToInt() > 0)
        {
            endFight = false;
        }
    }

    if (User->hlth <= 0)
    {
        Application->MessageBox(L"Вы потерпели поражение в бою!", frmFight->Caption.c_str(), MB_OK | MB_ICONHAND);
        PlaySound(L"death.wav", NULL, SND_ASYNC);

        frmFight->Hide();
        frmFirst->Show();
        return true;
    }

    if (endFight)
    {
        Application->MessageBox(L"Вы их сделали!", frmFight->Caption.c_str(), MB_OK | MB_ICONINFORMATION);

        frmFight->Hide();
        frmChapt->LoadNext(jump);
        frmChapt->Show();
        return true;
    }

    return false;
}

void __fastcall TfrmFight::btnAttackClick(TObject *Sender)
{
    grEnemy->Cells[3][selEnemy + 1] = grEnemy->Cells[3][selEnemy + 1].ToInt() - grWeapon->Cells[2][selWeapon + 1].ToInt();
    User->man -= grWeapon->Cells[3][selWeapon + 1].ToInt();
    apt -= grWeapon->Cells[4][selWeapon + 1].ToInt();

    if (apt <= 0)
    {
        opponentAttack();
    }
    else
    {
        this->update();
        check();
    }

    if (User->hlth > 0 && frmFight->Visible)
    {
        grEnemy->SetFocus();
    }
}

void __fastcall TfrmFight::grEnemySelectCell(TObject *Sender, int ACol, int Row, bool &CanSelect)
{
    int i;
    for (i = 0; i < grEnemy->RowCount; i++)
    {
        grEnemy->Cells[0][i] = "";
    }
    grEnemy->Cells[0][Row] = ">";
    selEnemy = Row - 1;
    check();
}

void __fastcall TfrmFight::grWeaponSelectCell(TObject *Sender, int ACol, int Row, bool &CanSelect)
{
    int i;
    for (i = 0; i < grWeapon->RowCount; i++)
    {
        grWeapon->Cells[0][i] = "";
    }
    grWeapon->Cells[0][Row] = ">";
    selWeapon = Row - 1;
    check();
}

void __fastcall TfrmFight::grEnemyKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_RETURN)
    {
        grWeapon->SetFocus();
    }
}

void __fastcall TfrmFight::grWeaponKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_RETURN)
    {
        btnAttack->SetFocus();
    }
}

void __fastcall TfrmFight::FormClose(TObject *Sender, TCloseAction &Action)
{
    frmFirst->Show();
}
