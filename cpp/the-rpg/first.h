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

const String APP_VERSION = L"0.1.4";

class TfrmFirst : public TForm
{
__published:    // IDE-managed Components
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

    void __fastcall NewClick(TObject *Sender);
    void __fastcall ExitClick(TObject *Sender);
    void __fastcall AboutClick(TObject *Sender);
    void __fastcall frmFirstCreate(TObject *Sender);
    void __fastcall LoadClick(TObject *Sender);

private:        // User declarations
    TStringList* save;

public:         // User declarations
    __fastcall TfrmFirst(TComponent* Owner);
    __fastcall virtual ~TfrmFirst(); // Добавлен деструктор для очистки памяти
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
    TStringList* GroundItems;

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
