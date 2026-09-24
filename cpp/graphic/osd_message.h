#ifndef OSD_MESSAGE_H
#define OSD_MESSAGE_H

#include <vcl.h>

class TOsdMessage : public TObject
{
private:
    TForm* FForm;
    TLabel* FLabel;
    TTimer* FTimer;
    int FDisplayTime;

    void __fastcall OnTimerTick(TObject* Sender);

public:
    TOsdMessage(const UnicodeString& message_text, int duration_ms = 2000);
    static void Show(const UnicodeString& text, int duration_ms = 2000);
};

#endif
