#ifndef utestH
#define utestH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>

class TfrmUTest : public TForm
{
__published:    // IDE-managed Components
    TMemo *Memo1;
    TListBox *ListBox1;
    TButton *Button1;

    void __fastcall Button1Click(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall ListBox1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);

private:        // User declarations
    TStringList* test;
    int qptr;
    int aptr;

    void LoadNext();
    void ProcessSelection(); // Добавлен метод для исключения дублирования кода

public:         // User declarations
    __fastcall TfrmUTest(TComponent* Owner);
    __fastcall virtual ~TfrmUTest(); // Добавлен деструктор для предотвращения утечек памяти
};

extern PACKAGE TfrmUTest *frmUTest;

#endif
