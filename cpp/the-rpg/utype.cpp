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
    Lbl1->Caption = Lbl1->Caption + User->Name;
    Lbl2->Caption = Lbl2->Caption + User->CrType;
    Lbl3->Caption = Lbl3->Caption + User->SexType;
    Lbl4->Caption = Lbl4->Caption + IntToStr(User->age);
    Lbl5->Caption = Lbl5->Caption + IntToStr(User->str);
    Lbl6->Caption = Lbl6->Caption + IntToStr(User->dex);
    Lbl7->Caption = Lbl7->Caption + IntToStr(User->mag);
    Lbl8->Caption = Lbl8->Caption + IntToStr(User->hlth);
    Lbl9->Caption = Lbl9->Caption + IntToStr(User->man);
}

void __fastcall TfrmUType::OKBtnClick(TObject *Sender)
{
    frmUType->Close();
}

void __fastcall TfrmUType::frmDeactiv(TObject *Sender)
{
    frmUType->Close();
}
