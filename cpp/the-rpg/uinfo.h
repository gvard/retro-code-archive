#ifndef uinfoH
#define uinfoH

#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>

struct TRaceData
{
    String Name;
    int Modifier;
};

class TfrmUInfo : public TForm
{
__published:
    TButton* Button1;
    TEdit* Edit1;
    TComboBox* ComboBox1;
    TComboBox* ComboBox2;
    TEdit* Edit2;
    TLabel* Label1;
    TLabel* Label2;
    TLabel* Label3;
    TLabel* Label4;

    void __fastcall Button1Click(TObject* Sender);
    void __fastcall Edit1KeyDown(TObject* Sender, WORD& Key, TShiftState Shift);
    void __fastcall Edit2KeyDown(TObject* Sender, WORD& Key, TShiftState Shift);
    void __fastcall FormShow(TObject* Sender);

private:
    auto parse_race_line(const String& ALine) -> TRaceData;

public:
    explicit __fastcall TfrmUInfo(TComponent* Owner) override;
    __fastcall ~TfrmUInfo() override = default;
};

#endif
