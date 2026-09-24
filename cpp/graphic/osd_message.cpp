#include "osd_message.h"

TOsdMessage::TOsdMessage(const UnicodeString& message_text, int duration_ms)
    : FDisplayTime(duration_ms)
{
    FForm = new TForm(static_cast<TComponent*>(nullptr));

    FForm->BorderStyle = bsNone;
    FForm->Color = static_cast<TColor>(RGB(40, 40, 40)); // clBlack, clWebDarkSlateGray;
    FForm->AlphaBlend = true;
    FForm->AlphaBlendValue = 180;
    FForm->FormStyle = fsStayOnTop;

    FLabel = new TLabel(FForm);
    FLabel->Parent = FForm;
    FLabel->Align = alClient;
    FLabel->Alignment = taCenter;
    FLabel->Layout = tlCenter;
    FLabel->Caption = message_text;
    FLabel->Transparent = true;
    FLabel->Font->Name = "Arial";
    FLabel->Font->Size = 12;
    // FLabel->Font->Color = clWhite;
    FLabel->Font->Color = static_cast<TColor>(RGB(50, 205, 50));
    FLabel->Font->Style = TFontStyles() << fsBold;

    FForm->Width = FLabel->Canvas->TextWidth(message_text) + 40;
    FForm->Height = 45;
    FForm->Left = (Screen->Width - FForm->Width) / 2;
    FForm->Top = Screen->Height - FForm->Height - 100;

    FTimer = new TTimer(FForm);
    FTimer->Interval = FDisplayTime;
    FTimer->OnTimer = OnTimerTick;
    FTimer->Enabled = true;

    FForm->Show();
}

void __fastcall TOsdMessage::OnTimerTick(TObject* Sender)
{
    FTimer->Enabled = false;
    FForm->Close();

    delete FForm;
    delete this;
}

void TOsdMessage::Show(const UnicodeString& text, int duration_ms)
{
    new TOsdMessage(text, duration_ms);
}
