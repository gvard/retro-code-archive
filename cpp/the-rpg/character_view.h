#ifndef character_viewH
#define character_viewH

#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>

class TfrmUType : public TForm
{
__published:
    TButton *OKBtn;
    TBevel* Bvl1;
    TLabel* Lbl1;
    TLabel* Lbl2;
    TLabel* Lbl3;
    TLabel* Lbl4;
    TLabel* Lbl5;
    TLabel* Lbl6;
    TLabel* Lbl7;
    TLabel* Lbl8;
    TLabel* Lbl9;
    TLabel* lblStamina;

    void __fastcall frmShow(TObject* Sender);
    void __fastcall OKBtnClick(TObject* Sender);
    void __fastcall FormKeyDown(TObject* Sender, WORD& Key, TShiftState Shift);

private:
    void update_stamina_display();

public:
    explicit __fastcall TfrmUType(TComponent* AOwner) override;
    __fastcall ~TfrmUType() override = default;
};

#endif
