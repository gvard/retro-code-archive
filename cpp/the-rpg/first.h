#ifndef firstH
#define firstH

#include <System.hpp>
#include <System.Classes.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ActnList.hpp>

const String APP_VERSION = L"0.1.5";

class TfrmFirst : public TForm
{
__published:
    TMainMenu* MainMenu1;
    TMenuItem* File;
    TMenuItem* Help;
    TMenuItem* About;
    TMenuItem* New;
    TMenuItem* Load;
    TMenuItem* Exit;
    TLabel* Label1;
    TLabel* Label2;
    TImage* Image1;
    TOpenDialog* OpenDialog1;
    TActionList* ActionList1;
    TAction* actNewGame;
    TAction* actSaveGame;
    TAction* actLoadGame;

    void __fastcall NewClick(TObject* Sender);
    void __fastcall ExitClick(TObject* Sender);
    void __fastcall AboutClick(TObject* Sender);
    void __fastcall frmFirstCreate(TObject* Sender);
    void __fastcall LoadClick(TObject* Sender);

private:
    TStringList* save;

public:
    __fastcall TfrmFirst(TComponent* Owner) override;
    __fastcall ~TfrmFirst();
};

class TUser
{
public:
    void Clear();
    void Refresh();
    auto LoadGame(const String& AFileName) -> bool;
    auto SaveGame(const String& AFileName, int ACurrentQid) -> bool;

    auto GetMaxStamina() -> int;
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
extern TUser* User;
extern PACKAGE TfrmFirst* frmFirst;

#endif
