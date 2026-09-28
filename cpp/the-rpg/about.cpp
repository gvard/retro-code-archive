#include "about.h"
#include "first.h"

#pragma resource "*.dfm"

__fastcall TAboutBox::TAboutBox(TComponent* AOwner)
    : TForm(AOwner)
{
    Version->Caption = L"Version " + APP_VERSION;
}

void __fastcall TAboutBox::OKButtonClick(TObject* Sender)
{
    ModalResult = mrOk;
}
