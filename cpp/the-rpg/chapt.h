#ifndef chaptH
#define chaptH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>

#include <cstdio>
#include <Winapi.MMSystem.hpp>

class TfrmChapt : public TForm
{
__published:    // IDE-managed Components
    TMainMenu *MainMenu1;
    TMenuItem *File;
    TMenuItem *Game;
    TMenuItem *Help;
    TMenuItem *Exit;
    TMenuItem *Help1;
    TMenuItem *Save;
    TMemo *Memo1;
    TListBox *ListBox1;
    TButton *Button1;
    TMenuItem *ustype1;
    TMenuItem *About;
    TMenuItem *Load;
    TSaveDialog *SaveDialog1;
    TOpenDialog *OpenDialog1;
    TMenuItem *N1;

    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ExitClick(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall Help1Click(TObject *Sender);
    void __fastcall AboutClick(TObject *Sender);
    void __fastcall ListBox1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall frmChaptCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall LoadClick(TObject *Sender);
    void __fastcall SaveClick(TObject *Sender);
    void __fastcall InvClick(TObject *Sender);
    void __fastcall ustype1Click(TObject *Sender);

private:        // User declarations
    TStringList* chapter;
    TStringList* save;
    int aptr;

public:         // User declarations
    __fastcall TfrmChapt(TComponent* Owner);
    __fastcall virtual ~TfrmChapt(); // Добавлен деструктор для очистки памяти

    void LoadNext(int qid);
};

extern PACKAGE TfrmChapt *frmChapt;

#endif
