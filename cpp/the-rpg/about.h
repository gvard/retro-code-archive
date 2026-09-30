#ifndef aboutH
#define aboutH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>

class TAboutBox : public TForm
{
__published:
    TPanel* Panel1;
    TImage* Ico;
    TLabel* ProductName;
    TLabel* Version;
    TLabel* Copyright;
    TLabel* Comments;
    TButton* OKButton;
    TLabel* lblEmail1;
    TLabel* lblEmail2;

    void __fastcall OKButtonClick(TObject* Sender);

private:

public:
    __fastcall TAboutBox(TComponent* AOwner) override;
    __fastcall ~TAboutBox() = default;
};

#endif
