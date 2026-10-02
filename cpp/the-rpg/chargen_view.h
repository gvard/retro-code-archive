#ifndef chargen_viewH
#define chargen_viewH

#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>

struct race_data
{
    String name;
    int modifier;
};

class TfrmCharGen : public TForm
{
__published:
    TButton* Button1;
    TEdit* Edit1;
    TComboBox* cbRace;
    TComboBox* cbGender;
    TEdit* Edit2;
    TLabel* Label1;
    TLabel* Label2;
    TLabel* Label3;
    TLabel* Label4;

    void __fastcall Button1Click(TObject* Sender);
    void __fastcall Edit1KeyDown(TObject* Sender, WORD& Key, TShiftState Shift);
    void __fastcall Edit2KeyDown(TObject* Sender, WORD& Key, TShiftState Shift);
    void __fastcall FormShow(TObject* Sender);

public:
    explicit __fastcall TfrmCharGen(TComponent* Owner) override;
    __fastcall ~TfrmCharGen() override = default;
};

#endif
