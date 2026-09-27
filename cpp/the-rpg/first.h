#ifndef firstH
#define firstH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ActnList.hpp>
#include <System.Actions.hpp>

const String APP_VERSION = L"0.1.5";

class TfrmFirst : public TForm
{
__published:
    TMainMenu *MainMenu1;
    TMenuItem *File;
    TMenuItem *Help;
    TMenuItem *About;
    TMenuItem *New;
    TMenuItem *Load;
    TMenuItem *Exit;
    TLabel *Label1;
    TLabel *Label2;
    TImage *Image1;
    TOpenDialog *OpenDialog1;
	TActionList *ActionList1;
	TAction *actNewGame;
	TAction *actSaveGame;
	TAction *actLoadGame;

    void __fastcall NewClick(TObject *Sender);
    void __fastcall ExitClick(TObject *Sender);
    void __fastcall AboutClick(TObject *Sender);
    void __fastcall frmFirstCreate(TObject *Sender);
    void __fastcall LoadClick(TObject *Sender);

private:
    TStringList* save;

public:
    __fastcall TfrmFirst(TComponent* Owner);
    __fastcall virtual ~TfrmFirst();
};

class TUser
{
public:
    void Clear();
    void Refresh();
    bool LoadGame(const String& AFileName);
    bool SaveGame(const String& AFileName, int ACurrentQid);

    int GetMaxStamina();  // Returns maximum stamina based on Strength and Dexterity
    void RecalculateStamina(int totalWeight);

    TStringList* UserItems;
    TStringList* EnvironmentItems;
    bool isInventoryLoaded;

    String Name;
    String CrType;
    String SexType;

    int age;
    int str;
    int dex;
    int mag;
    int hlth;
    int man;
    int s;
	int maxWeight;
};

extern String ExePath;
extern TUser *User;
extern PACKAGE TfrmFirst *frmFirst;

#endif
