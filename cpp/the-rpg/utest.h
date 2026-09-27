#ifndef utestH
#define utestH

#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>

class TfrmUTest : public TForm
{
__published:
    TMemo *Memo1;
    TListBox *ListBox1;
    TButton *Button1;

    void __fastcall Button1Click(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ListBox1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall ListBox1DblClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);

private:
    TStringList* test;
    int qptr;
    int aptr;

    void LoadNext();
    void ProcessSelection();

public:
    __fastcall TfrmUTest(TComponent* Owner);
    __fastcall virtual ~TfrmUTest();
};

extern PACKAGE TfrmUTest *frmUTest;

#endif
