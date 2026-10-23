#include "about_view.h"
#include "main_menu_view.h"

#pragma resource "*.dfm"

__fastcall TfrmAbout::TfrmAbout(TComponent* AOwner)
    : TForm(AOwner)
{
    Version->Caption = L"Version " + APP_VERSION;
}

void __fastcall TfrmAbout::OKButtonClick(TObject* /*Sender*/)
{
    ModalResult = mrOk;
}
