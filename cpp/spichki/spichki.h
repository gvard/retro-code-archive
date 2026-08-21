#ifndef spichkiH
#define spichkiH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>

class TForm1 : public TForm
{
__published:
    TMemo *Memo2;
    TButton *Button1;
    TGroupBox *GroupBox1;
    TGroupBox *GroupBox2;
    TRadioGroup *RadioGroup1;
    TRadioGroup *RadioGroup2;
    TRadioButton *RadioButton1;
    TRadioButton *RadioButton2;
    TRadioButton *RadioButton3;
    TRadioButton *RadioButton4;
    TMemo *Memo1;
    TLabel *Label1;
    TLabel *Label2;

    void __fastcall FormCreate(TObject *Sender);
    void __fastcall KeyPress(TObject *Sender, char &Key);
    void __fastcall Click(TObject *Sender);
    void __fastcall Click1(TObject *Sender);

private:

public:
    __fastcall TForm1(TComponent* Owner);
};

extern TForm1 *Form1;
TShape *MyShape[30];

#endif
