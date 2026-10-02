#pragma once

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>

class TfrmAbout : public TForm
{
__published:
    TPanel* Panel1;
    TImage* imgLogoSquare;
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
    __fastcall TfrmAbout(TComponent* AOwner) override;
    __fastcall ~TfrmAbout() = default;
};
