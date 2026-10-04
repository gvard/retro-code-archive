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

class TfrmMainMenu : public TForm
{
__published:
    TMainMenu* MainMenu1;
    TMenuItem* File;
    TMenuItem* Help;
    TMenuItem* About;
    TMenuItem* New;
    TMenuItem* Load;
    TMenuItem* Exit;
	TLabel *lblGreeting;
	TLabel *lblVersion;
    TImage* imgLogoFull;
    TOpenDialog* OpenDialog1;
    TActionList* ActionList1;
    TAction* actNewGame;
    TAction* actSaveGame;
    TAction* actLoadGame;

    void __fastcall NewClick(TObject* Sender);
    void __fastcall ExitClick(TObject* Sender);
    void __fastcall AboutClick(TObject* Sender);
    void __fastcall frmMainMenuCreate(TObject* Sender);
    void __fastcall LoadClick(TObject* Sender);

private:
    TStringList* save;

public:
    __fastcall TfrmMainMenu(TComponent* Owner) override;
    __fastcall ~TfrmMainMenu();
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
    int strength;
    int dex;
    int mag;
    int hlth;
    int mana;
    int stamina;
    int maxWeight;
    int gold;
};

extern String ExePath;
extern TUser* User;
extern PACKAGE TfrmMainMenu* frmMainMenu;

#endif
