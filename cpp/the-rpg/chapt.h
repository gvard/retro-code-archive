#ifndef chaptH
#define chaptH

#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>

class TfrmChapt : public TForm
{
__published:
    TMainMenu* MainMenu1;
    TMenuItem* File;
    TMenuItem* Game;
    TMenuItem* Help;
    TMenuItem* Exit;
    TMenuItem* Help1;
    TMenuItem* Save;
    TMemo* Memo1;
    TListBox* ListBox1;
    TButton* btnConfirmChoice;
    TMenuItem* menuCharacterStats;
    TMenuItem* About;
    TMenuItem* Load;
    TSaveDialog* SaveDialog1;
    TOpenDialog* OpenDialog1;
    TMenuItem* menuInventory;

    void __fastcall FormCreate(TObject* Sender);
    void __fastcall ExitClick(TObject* Sender);
    void __fastcall ConfirmChoiceClick(TObject* Sender);
    void __fastcall Help1Click(TObject* Sender);
    void __fastcall AboutClick(TObject* Sender);
    void __fastcall ListBox1KeyDown(TObject* Sender, WORD& Key, TShiftState Shift);
    void __fastcall frmChaptCloseQuery(TObject* Sender, bool& CanClose);
    void __fastcall LoadClick(TObject* Sender);
    void __fastcall SaveClick(TObject* Sender);
    void __fastcall menuInventoryClick(TObject* Sender);
    void __fastcall menuCharacterStatsClick(TObject* Sender);
    void __fastcall ListBox1DblClick(TObject* Sender);
    void __fastcall FormResize(TObject* Sender);
    void __fastcall FormKeyDown(TObject* Sender, WORD& Key, TShiftState Shift);

private:
    TStringList* chapter;
    TStringList* save;
    int aptr;
    int currentQid;
    int previousTextQid = 1;
    bool is_test_mode = false;
    int test_qptr = 0;

    void initialize_starting_inventory();

public:
    __fastcall TfrmChapt(TComponent* Owner) override;
    __fastcall ~TfrmChapt();

    void LoadNext(int target_qid);
    void start_character_test();
    bool hasUnsavedChanges = false;
    void ResetUnsavedChanges() { hasUnsavedChanges = false; }
};

extern PACKAGE TfrmChapt* frmChapt;

#endif
