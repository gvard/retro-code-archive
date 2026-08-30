#ifndef utypeH
#define utypeH

#include <Winapi.Windows.hpp>
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ExtCtrls.hpp>

class TfrmUType : public TForm
{
__published:    // IDE-managed Components
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

private:        // User declarations

public:         // User declarations
    __fastcall TfrmUType(TComponent* AOwner) override;
    __fastcall virtual ~TfrmUType() {} // Добавлен деструктор для соблюдения единого стиля проекта
    void UpdateStaminaDisplay();
};

extern PACKAGE TfrmUType *frmUType;

#endif
