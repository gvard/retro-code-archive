#ifndef uinfoH
#define uinfoH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>

struct TRaceData
{
    String Name;
    int Modifier;
};

class TfrmUInfo : public TForm
{
__published:    // IDE-managed Components
    TButton *Button1;
    TEdit *Edit1;
    TComboBox *ComboBox1;
    TComboBox *ComboBox2;
    TEdit *Edit2;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label4;

    void __fastcall Button1Click(TObject *Sender);
    void __fastcall Edit1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall Edit2KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall frmUInfoClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormDestroy(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);

private:        // User declarations
    TRaceData ParseRaceLine(const String& ALine);

public:         // User declarations
    __fastcall TfrmUInfo(TComponent* Owner);
    __fastcall virtual ~TfrmUInfo() {} // Добавлен деструктор для соблюдения единого стиля проекта
};

extern PACKAGE TfrmUInfo *frmUInfo;

#endif
