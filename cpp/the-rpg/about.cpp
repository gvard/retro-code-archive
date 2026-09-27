#include <vcl.h>

#include "about.h"
#include "first.h"

#pragma resource "*.dfm"

TAboutBox *AboutBox;

__fastcall TAboutBox::TAboutBox(TComponent* AOwner)
    : TForm(AOwner)
{
    Version->Caption = L"Version " + APP_VERSION;
    // Version->ParentFont = false;
    // Version->Font->Name = L"Tahoma";
    // Version->Font->Size = 8;
    // Принудительно отключаем сглаживание Windows ClearType для этой метки
    LOGFONT lf;
    if (GetObject(Version->Font->Handle, sizeof(LOGFONT), &lf))
    {
        lf.lfQuality = NONANTIALIASED_QUALITY;
        Version->Font->Handle = CreateFontIndirect(&lf);
    }
}

void __fastcall TAboutBox::OKButtonClick(TObject *Sender)
{
    AboutBox->Close();
}

void __fastcall TAboutBox::FrmDeactivate(TObject *Sender)
{
    AboutBox->Close();
}
