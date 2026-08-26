#ifndef aboutH
#define aboutH

#include <Winapi.Windows.hpp>
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ExtCtrls.hpp>

class TAboutBox : public TForm
{
__published:    // IDE-managed Components
    TPanel *Panel1;
    TImage *Ico;
    TLabel *ProductName;
    TLabel *Version;
    TLabel *Copyright;
    TLabel *Comments;
    TButton *OKButton;
    TLabel *Label5;
    TLabel *Label1;

    void __fastcall OKButtonClick(TObject *Sender);
    void __fastcall FrmDeactivate(TObject *Sender);

private:        // User declarations

public:         // User declarations
    __fastcall TAboutBox(TComponent* AOwner) override;
    __fastcall virtual ~TAboutBox() {} // Добавлен деструктор для соблюдения единого стиля проекта
};

extern PACKAGE TAboutBox *AboutBox;

#endif
