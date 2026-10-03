#include "first.h"
#include "charstats_view.h"

#pragma resource "*.dfm"

__fastcall TfrmCharStats::TfrmCharStats(TComponent* AOwner)
    : TForm(AOwner)
{
}

void __fastcall TfrmCharStats::frmShow(TObject* /*Sender*/)
{
    Lbl1->Caption = L"Имя: " + User->Name;
    Lbl2->Caption = L"Раса: " + User->CrType;
    Lbl3->Caption = L"Пол: " + User->SexType;
    Lbl4->Caption = L"Возраст: " + IntToStr(User->age);
    Lbl5->Caption = L"Сила: " + IntToStr(User->strength);
    Lbl6->Caption = L"Ловкость: " + IntToStr(User->dex);
    Lbl7->Caption = L"Магия: " + IntToStr(User->mag);
    Lbl8->Caption = L"Здоровье: " + IntToStr(User->hlth);
    Lbl9->Caption = L"Мана: " + IntToStr(User->mana);

    this->update_stamina_display();
}

void __fastcall TfrmCharStats::OKBtnClick(TObject* /*Sender*/)
{
    this->ModalResult = mrOk;
}

void TfrmCharStats::update_stamina_display()
{
    const int max_stamina = User->GetMaxStamina();
    lblStamina->Caption = L"Выносливость: " + IntToStr(User->stamina) + L" / " + IntToStr(max_stamina);

    if (User->stamina < max_stamina)
    {
        lblStamina->Font->Color = clRed;
    }
    else
    {
        lblStamina->Font->Color = clWindowText;
    }
}
void __fastcall TfrmCharStats::FormKeyDown(TObject* /*Sender*/, WORD& Key, TShiftState Shift)
{
    if (Shift == TShiftState{})
    {
        if (Key == 'C' || Key == VK_ESCAPE)
        {
            Key = 0;
            this->ModalResult = mrOk;
        }
    }
}
