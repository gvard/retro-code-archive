#include <cstdio>
#include <Winapi.Windows.hpp>

#include "invent.h"
#include "first.h"

#pragma resource "*.dfm"

extern bool isFirstInventoryLaunch;

__fastcall TDualListDlg::TDualListDlg(TComponent* AOwner)
    : TForm(AOwner)
{
}

void TDualListDlg::update_weight_display()
{
    int total_weight = 0;

    for (int i = 0; i < SrcList->Items->Count; ++i)
    {
        total_weight += static_cast<int>(reinterpret_cast<intptr_t>(SrcList->Items->Objects[i]));
    }

    Label1->Caption = L"Вес: " + IntToStr(total_weight);
    Label2->Caption = L"Из возможных: " + IntToStr(User->maxWeight);

    User->RecalculateStamina(total_weight);

    const bool is_overloaded = total_weight > User->maxWeight;
    Label1->Font->Color = is_overloaded ? clRed : clWindowText;

    wchar_t weight_log[512];
    std::swprintf(weight_log, 512,
        L"[ОТЛАДКА ВЕСА] Текущий вес рюкзака: %d | Макс. вес: %d | "
        L"Статус: %s | Выносливость персонажа (User->s): %d (Макс: %d)",
        total_weight,
        User->maxWeight,
        is_overloaded ? L"ПЕРЕГРУЗ!" : L"Норма",
        User->s,
        User->GetMaxStamina()
    );
    OutputDebugString(weight_log);
}

void __fastcall TDualListDlg::IncludeBtnClick(TObject* /*Sender*/)
{
    const int index = GetFirstSelection(SrcList);
    MoveSelected(SrcList, DstList->Items);
    SetItem(SrcList, index);

    this->update_weight_display();
}

void __fastcall TDualListDlg::ExcludeBtnClick(TObject* /*Sender*/)
{
    int Index = GetFirstSelection(DstList);
    MoveSelected(DstList, SrcList->Items);
    SetItem(DstList, Index);
    update_weight_display();
}

void __fastcall TDualListDlg::IncAllBtnClick(TObject* /*Sender*/)
{
    DstList->Items->AddStrings(SrcList->Items);
    SrcList->Items->Clear();
    SetItem(SrcList, 0);
    update_weight_display();
}

void __fastcall TDualListDlg::ExcAllBtnClick(TObject* /*Sender*/)
{
    SrcList->Items->AddStrings(DstList->Items);
    DstList->Items->Clear();
    SetItem(DstList, 0);
    update_weight_display();
}

void __fastcall TDualListDlg::MoveSelected(TCustomListBox* List, TStrings* Items)
{
    for (int i = List->Items->Count - 1; i >= 0; --i)
    {
        if (List->Selected[i])
        {
            Items->AddObject(List->Items->Strings[i], List->Items->Objects[i]);
            List->Items->Delete(i);
        }
    }
}

void __fastcall TDualListDlg::SetButtons()
{
    bool SrcEmpty = (SrcList->Items->Count == 0);
    bool DstEmpty = (DstList->Items->Count == 0);

    IncludeBtn->Enabled = (!SrcEmpty);
    IncAllBtn->Enabled = (!SrcEmpty);
    ExcludeBtn->Enabled = (!DstEmpty);
    ExAllBtn->Enabled = (!DstEmpty);
}

auto __fastcall TDualListDlg::GetFirstSelection(TCustomListBox* List) -> int
{
    for (int i = 0; i < List->Items->Count; ++i)
    {
        if (List->Selected[i])
        {
            return i;
        }
    }
    return LB_ERR;
}

void __fastcall TDualListDlg::SetItem(TListBox* List, int Index)
{
    int MaxIndex = List->Items->Count - 1;
    List->SetFocus();

    if (Index == LB_ERR)
    {
        Index = 0;
    }
    else if (Index > MaxIndex)
    {
        Index = MaxIndex;
    }

    if (List->Items->Count > 0)
    {
        List->Selected[Index] = true;
    }
    SetButtons();
}

void __fastcall TDualListDlg::FormShow(TObject* /*Sender*/)
{
    SrcList->Items->Clear();
    DstList->Items->Clear();

    if (User != nullptr)
    {
        if (User->UserItems != nullptr)
        {
            for (int i = 0; i < User->UserItems->Count; ++i)
            {
                SrcList->Items->AddObject(User->UserItems->Strings[i], User->UserItems->Objects[i]);
            }
        }

        if (User->EnvironmentItems != nullptr)
        {
            for (int i = 0; i < User->EnvironmentItems->Count; ++i)
            {
                DstList->Items->AddObject(User->EnvironmentItems->Strings[i], User->EnvironmentItems->Objects[i]);
            }
        }
    }

    update_weight_display();
    SetButtons();
}

void __fastcall TDualListDlg::OKBtnClick(TObject* /*Sender*/)
{
    User->UserItems->Clear();
    for (int i = 0; i < SrcList->Items->Count; ++i)
    {
        User->UserItems->AddObject(SrcList->Items->Strings[i], SrcList->Items->Objects[i]);
    }

    User->EnvironmentItems->Clear();
    for (int i = 0; i < DstList->Items->Count; ++i)
    {
        User->EnvironmentItems->AddObject(DstList->Items->Strings[i], DstList->Items->Objects[i]);
    }

    ModalResult = mrOk;
}

void __fastcall TDualListDlg::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Shift == TShiftState{})
    {
		if (Key == 'I' || Key == VK_ESCAPE)
        {
            Key = 0;
            this->ModalResult = mrOk;
        }
	}
}

