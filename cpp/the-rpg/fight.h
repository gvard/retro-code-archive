# pragma once

#include <Vcl.ComCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>

#include <vector>
#include <string>


struct EnemyInitData {
    int race_type_idx;
    int base_hit;
    String custom_name;
};

class TfrmFight : public TForm
{
__published:
    TStringGrid* grEnemy;
    TStringGrid* grWeapon;
    TStatusBar* sbBar;
    TButton* btnAttack;

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
    int apt = 0;
    int selWeapon = 0;
    int selEnemy = 0;

    void updateApt();
    void update_display();
    void opponentAttack();
    auto check_battle_state() -> bool;

public:
    String battle_caption;
    std::vector<EnemyInitData> raw_enemies;
    std::vector<std::wstring> raw_race_types;

    explicit __fastcall TfrmFight(TComponent* Owner) override;
    __fastcall ~TfrmFight() = default;

protected:
    void __fastcall CreateParams(TCreateParams& Params) override;

};

extern PACKAGE TfrmFight* frmFight;
