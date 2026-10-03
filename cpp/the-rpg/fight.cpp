#include <string>
#include <string_view>
#include <system_error>
#include <algorithm>

#include <Winapi.MMSystem.hpp>
#include <Winapi.Messages.hpp>

#include "fight.h"
#include "first.h"
#include "storage_manager.h"

#pragma resource "*.dfm"

TfrmFight* frmFight;

__fastcall TfrmFight::TfrmFight(TComponent* Owner)
    : TForm(Owner)
{
}

void __fastcall TfrmFight::CreateParams(TCreateParams& Params)
{
    // Базовая настройка VCL формы
    TForm::CreateParams(Params);

    // Расширенный стиль Windows API: сделать окно видимым на панели задач
    Params.ExStyle |= WS_EX_APPWINDOW;

    // Связываем окно с рабочим столом Windows
    Params.WndParent = ::GetDesktopWindow();
}

void __fastcall TfrmFight::FormCreate(TObject* /*Sender*/)
{
    grEnemy->Cells[1][0] = "Вид противника";
    grEnemy->Cells[2][0] = "Имя";
    grEnemy->Cells[3][0] = "Жизнь";
    grEnemy->Cells[4][0] = "Хиты";
    grWeapon->Cells[1][0] = "Оружие";
    grWeapon->Cells[2][0] = "Хиты";
    grWeapon->Cells[3][0] = "Мана";
    grWeapon->Cells[4][0] = "ActPts";

    const auto& weapons = storage_system::get().get_weapons();

    if (!weapons.empty())
    {
        // Устанавливаем количество строк в таблице оружия (+1 для заголовка)
        grWeapon->RowCount = static_cast<int>(weapons.size()) + 1;

        int row_idx = 1;
        for (const auto& weapon : weapons)
        {
            grWeapon->Cells[1][row_idx] = UTF8String(weapon.name.c_str());
            grWeapon->Cells[2][row_idx] = IntToStr(weapon.hp);
            grWeapon->Cells[3][row_idx] = IntToStr(weapon.mana);
            grWeapon->Cells[4][row_idx] = IntToStr(weapon.ap);
            row_idx++;
        }
    }
}

void __fastcall TfrmFight::FormShow(TObject* /*Sender*/)
{
    selEnemy = 0;
    selWeapon = 0;
    updateApt();
    this->update_display();

    this->Caption = battle_caption;

    int count = 0;
    for (const auto& enemy : raw_enemies)
    {
        grEnemy->Cells[0][count + 1] = " ";

        const auto& parsed_races = storage_system::get().get_races();
        const int target_idx = enemy.race_type_idx - 1;

        if (target_idx >= 0 && target_idx < static_cast<int>(parsed_races.size()))
        {
            // Выводим только имя расы, так как парсер уже отрезал модификатор
            grEnemy->Cells[1][count + 1] = parsed_races[target_idx].name.c_str();
        }
        else
        {
            grEnemy->Cells[1][count + 1] = L"Неизвестно";
        }

        grEnemy->Cells[2][count + 1] = enemy.custom_name;
        grEnemy->Cells[3][count + 1] = L"100";
        grEnemy->Cells[4][count + 1] = IntToStr(enemy.base_hit);
        count++;
    }

    grEnemy->RowCount = count + 1;
    for (int i = 1; i < grWeapon->RowCount; ++i)
    {
        grWeapon->Cells[0][i] = " ";
    }

    grEnemy->Cells[0][1] = ">";
    grWeapon->Cells[0][1] = ">";
}

void TfrmFight::updateApt()
{
    apt = (User->dex * User->stamina) / 100;
    if (apt < 0)
    {
        apt = 0;
    }
}

void TfrmFight::update_display()
{
    sbBar->Panels->Items[0]->Text = L"Health: " + IntToStr(User->hlth);
    sbBar->Panels->Items[1]->Text = L"Мана: " + IntToStr(User->mana);
    sbBar->Panels->Items[2]->Text = L"Stam: " + IntToStr(User->stamina);
    sbBar->Panels->Items[3]->Text = L"Action points: " + IntToStr(apt);
}

void TfrmFight::opponentAttack()
{
    if (check_battle_state())
        return;

    int total_hit = 0;
    for (int i = 1; i < grEnemy->RowCount; ++i)
    {
        if (grEnemy->Cells[3][i].ToInt() > 0)
        {
            total_hit += grEnemy->Cells[4][i].ToInt();
        }
    }

    User->hlth -= total_hit;
    User->stamina -= total_hit;
    User->Refresh();

    updateApt();
    this->update_display();
    this->Repaint();

    if (total_hit > 0)
    {
        const String msg = L"Противник нанес вам удар: " + IntToStr(total_hit);
        ::MessageBoxW(this->Handle, msg.c_str(), this->Caption.c_str(), MB_OK | MB_ICONWARNING);
    }

    if (check_battle_state())
        return;

    if (apt <= 0)
    {
        opponentAttack();
    }
}

auto TfrmFight::check_battle_state() -> bool
{
    using namespace std::string_view_literals;

    if (grEnemy->Cells[3][selEnemy + 1].ToInt() < 0)
    {
        grEnemy->Cells[3][selEnemy + 1] = "0";
    }

    btnAttack->Enabled = !(grEnemy->Cells[3][selEnemy + 1].ToInt() <= 0 ||
                           grWeapon->Cells[3][selWeapon + 1].ToInt() > User->mana ||
                           grWeapon->Cells[4][selWeapon + 1].ToInt() > apt);

    bool all_enemies_dead = true;
    for (int i = 1; i < grEnemy->RowCount; ++i)
    {
        if (grEnemy->Cells[3][i].ToInt() > 0)
        {
            all_enemies_dead = false;
            break;
        }
    }

    if (User->hlth <= 0)
    {
        ::MessageBoxW(this->Handle, L"Вы потерпели поражение в бою!", this->Caption.c_str(), MB_OK | MB_ICONHAND);
        PlaySound(storage_system::get().get_sound_path("death.wav").c_str(), nullptr, SND_ASYNC);

        this->ModalResult = mrCancel;
        return true;
    }

    if (all_enemies_dead)
    {
        ::MessageBoxW(this->Handle, L"Вы их сделали!", this->Caption.c_str(), MB_OK | MB_ICONINFORMATION);
        this->ModalResult = mrOk;
        return true;
    }

    return false;
}

void __fastcall TfrmFight::btnAttackClick(TObject* /*Sender*/)
{
    grEnemy->Cells[3][selEnemy + 1] = IntToStr(grEnemy->Cells[3][selEnemy + 1].ToInt() - grWeapon->Cells[2][selWeapon + 1].ToInt());
    User->mana -= grWeapon->Cells[3][selWeapon + 1].ToInt();
    apt -= grWeapon->Cells[4][selWeapon + 1].ToInt();

    this->update_display();
    this->Repaint();

    if (apt <= 0)
    {
        opponentAttack();
    }
    else
    {
        check_battle_state();
    }

    if (User->hlth > 0 && grEnemy->Cells[3][selEnemy + 1].ToInt() > 0)
    {
        grEnemy->SetFocus();
    }
}

void __fastcall TfrmFight::grEnemySelectCell(TObject* /*Sender*/, int /*ACol*/, int Row, bool& /*CanSelect*/)
{
    for (int i = 0; i < grEnemy->RowCount; ++i)
        grEnemy->Cells[0][i] = "";
    grEnemy->Cells[0][Row] = ">";
    selEnemy = Row - 1;
    check_battle_state();
}

void __fastcall TfrmFight::grWeaponSelectCell(TObject* /*Sender*/, int /*ACol*/, int Row, bool& /*CanSelect*/)
{
    for (int i = 0; i < grWeapon->RowCount; ++i)
        grWeapon->Cells[0][i] = "";
    grWeapon->Cells[0][Row] = ">";
    selWeapon = Row - 1;
    check_battle_state();
}

void __fastcall TfrmFight::grEnemyKeyDown(TObject* /*Sender*/, WORD& Key, TShiftState /*Shift*/)
{
    if (Key == VK_RETURN)
        grWeapon->SetFocus();
}

void __fastcall TfrmFight::grWeaponKeyDown(TObject* /*Sender*/, WORD& Key, TShiftState /*Shift*/)
{
    if (Key == VK_RETURN)
        btnAttack->SetFocus();
}

void __fastcall TfrmFight::FormResize(TObject* /*Sender*/)
{
    btnAttack->Left = (ClientWidth - btnAttack->Width) / 2;
}

void __fastcall TfrmFight::FormKeyDown(TObject* /*Sender*/, WORD& Key, TShiftState Shift)
{
    // Закрытие по Esc расценивается как побег (поражение)
    if (Shift == TShiftState{} && Key == VK_ESCAPE)
    {
        Key = 0;
        this->ModalResult = mrCancel;
    }
}
