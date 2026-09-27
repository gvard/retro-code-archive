#ifndef utypeH
#define utypeH

#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>

class TfrmUType : public TForm
{
__published:
    TButton *OKBtn;
    TBevel *Bvl1;
    TLabel *Lbl1;
    TLabel *Lbl2;
    TLabel *Lbl3;
    TLabel *Lbl4;
    TLabel *Lbl5;
    TLabel *Lbl6;
    TLabel *Lbl7;
    TLabel *Lbl8;
    TLabel *Lbl9;
	TLabel *lblStamina;

    void __fastcall frmShow(TObject *Sender);
    void __fastcall OKBtnClick(TObject *Sender);
    void __fastcall frmDeactiv(TObject *Sender);

private:

public:
    __fastcall TfrmUType(TComponent* AOwner) override;
    __fastcall virtual ~TfrmUType() {}
    void UpdateStaminaDisplay();
};

extern PACKAGE TfrmUType *frmUType;

#endif
