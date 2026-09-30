#ifndef fightH
#define fightH

#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ComCtrls.hpp>

class TfrmFight : public TForm
{
__published:
    TStringGrid* grEnemy;
    TStringGrid* grWeapon;
    TStatusBar* sbBar;
    TButton* btnAttack;

    void __fastcall FormClose(TObject* Sender, TCloseAction& Action);
    void __fastcall FormCreate(TObject* Sender);
    void __fastcall FormShow(TObject* Sender);
    void __fastcall btnAttackClick(TObject* Sender);
    void __fastcall grEnemySelectCell(TObject* Sender, int ACol, int ARow, bool& CanSelect);
    void __fastcall grWeaponSelectCell(TObject* Sender, int ACol, int ARow, bool& CanSelect);
    void __fastcall grEnemyKeyDown(TObject* Sender, WORD& Key, TShiftState Shift);
    void __fastcall grWeaponKeyDown(TObject* Sender, WORD& Key, TShiftState Shift);
    void __fastcall FormResize(TObject* Sender);
    void __fastcall FormKeyDown(TObject* Sender, WORD& Key, TShiftState Shift);

private:
    int apt;
    int jump;
    int selWeapon;
    int selEnemy;

    void updateApt();
    void update();
    void opponentAttack();
    auto check() -> bool;

public:
    int qptr;

    __fastcall TfrmFight(TComponent* Owner) override;
    __fastcall ~TfrmFight() = default;
};

extern PACKAGE TfrmFight* frmFight;

#endif
