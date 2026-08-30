#include <vcl.h>
#pragma hdrstop

#include "first.h"
#include "utype.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TfrmUType *frmUType;

__fastcall TfrmUType::TfrmUType(TComponent* AOwner)
    : TForm(AOwner)
{
}

void __fastcall TfrmUType::frmShow(TObject *Sender)
{
    Lbl1->Caption = L"Имя: " + User->Name;
    Lbl2->Caption = L"Раса: " + User->CrType;
    Lbl3->Caption = L"Пол: " + User->SexType;
    Lbl4->Caption = L"Возраст: " + IntToStr(User->age);
    Lbl5->Caption = L"Сила: " + IntToStr(User->str);
    Lbl6->Caption = L"Ловкость: " + IntToStr(User->dex);
    Lbl7->Caption = L"Магия: " + IntToStr(User->mag);
    Lbl8->Caption = L"Здоровье: " + IntToStr(User->hlth);
    Lbl9->Caption = L"Мана: " + IntToStr(User->man);
    UpdateStaminaDisplay();
}

void __fastcall TfrmUType::OKBtnClick(TObject *Sender)
{
    this->Close();
}

void __fastcall TfrmUType::frmDeactiv(TObject *Sender)
{
    this->Hide();
}

void TfrmUType::UpdateStaminaDisplay()
{
    int maxStamina = User->GetMaxStamina();
    lblStamina->Caption = L"Выносливость: " + IntToStr(User->s) + L" / " + IntToStr(maxStamina);

    if (User->s < maxStamina)
    {
        lblStamina->Font->Color = clRed;
    }
    else
    {
        lblStamina->Font->Color = clWindowText;
    }
}
