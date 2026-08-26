#ifndef fightH
#define fightH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ComCtrls.hpp>

class TfrmFight : public TForm
{
__published:    // IDE-managed Components
    TStringGrid *grEnemy;
    TStringGrid *grWeapon;
    TStatusBar *sbBar;
    TButton *btnAttack;

    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall btnAttackClick(TObject *Sender);
    void __fastcall grEnemySelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
    void __fastcall grWeaponSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
    void __fastcall grEnemyKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall grWeaponKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);

private:        // User declarations
    int apt;
    int jump;
    int selWeapon;
    int selEnemy;
    int countWeapon;
    int countEnemy;

    void updateApt();
    void update();
    void opponentAttack();
    bool check();

public:         // User declarations
    int qptr;

    __fastcall TfrmFight(TComponent* Owner);
    __fastcall virtual ~TfrmFight() {} // Добавлен пустой виртуальный деструктор по стандарту C++
};

extern PACKAGE TfrmFight *frmFight;

#endif
