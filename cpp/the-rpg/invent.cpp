#include <vcl.h>
#pragma hdrstop

#include "invent.h"
#include "first.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

extern bool isFirstInventoryLaunch;
TDualListDlg *DualListDlg;

__fastcall TDualListDlg::TDualListDlg(TComponent* AOwner)
    : TForm(AOwner)
{
}

void TDualListDlg::UpdateWeightDisplay()
{
    int totalWeight = 0;

    for (int i = 0; i < SrcList->Items->Count; i++)
    {
        totalWeight += (int)(SrcList->Items->Objects[i]);
    }

    Label1->Caption = L"Вес: " + IntToStr(totalWeight);
    Label2->Caption = L"Из возможных: " + IntToStr(User->maxWeight);

    User->RecalculateStamina(totalWeight);

    if (totalWeight > User->maxWeight)
    {
        Label1->Font->Color = clRed;
    }
    else
    {
        Label1->Font->Color = clWindowText;
    }
    wchar_t weightLog[512];
    swprintf(weightLog, 512,
        L"[ОТЛАДКА ВЕСА] Текущий вес рюкзака: %d | Макс. вес: %d | "
        L"Статус: %s | Выносливость персонажа (User->s): %d (Макс: %d)",
        totalWeight,
        User->maxWeight,
        (totalWeight > User->maxWeight) ? L"ПЕРЕГРУЗ!" : L"Норма",
        User->s,
        User->GetMaxStamina()
    );
    OutputDebugString(weightLog);
}

void __fastcall TDualListDlg::IncludeBtnClick(TObject *Sender)
{
    int Index = GetFirstSelection(SrcList);
    MoveSelected(SrcList, DstList->Items);
    SetItem(SrcList, Index);
    UpdateWeightDisplay();
}

void __fastcall TDualListDlg::ExcludeBtnClick(TObject *Sender)
{
    int Index = GetFirstSelection(DstList);
    MoveSelected(DstList, SrcList->Items);
    SetItem(DstList, Index);
    UpdateWeightDisplay();
}

void __fastcall TDualListDlg::IncAllBtnClick(TObject *Sender)
{
    DstList->Items->AddStrings(SrcList->Items);
    SrcList->Items->Clear();
    SetItem(SrcList, 0);
    UpdateWeightDisplay();
}

void __fastcall TDualListDlg::ExcAllBtnClick(TObject *Sender)
{
    SrcList->Items->AddStrings(DstList->Items);
    DstList->Items->Clear();
    SetItem(DstList, 0);
    UpdateWeightDisplay();
}

void __fastcall TDualListDlg::MoveSelected(TCustomListBox *List, TStrings *Items)
{
    for (int i = List->Items->Count - 1; i >= 0; i--)
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

int __fastcall TDualListDlg::GetFirstSelection(TCustomListBox *List)
{
    for (int i = 0; i < List->Items->Count; i++)
    {
        if (List->Selected[i])
        {
            return i;
        }
    }
    return LB_ERR;
}

void __fastcall TDualListDlg::SetItem(TListBox *List, int Index)
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

void __fastcall TDualListDlg::FormShow(TObject *Sender)
{
    // Очищаем экран перед отрисовкой
    SrcList->Items->Clear();
    DstList->Items->Clear();

    if (User->UserItems->Count == 1 && User->UserItems->Strings[0] == L"__INIT_NEW_GAME__")
    {
        // Маркер найден! Значит это самый первый запуск рюкзака в этой игре.
        // Очищаем маркер, чтобы этот блок больше никогда не сработал
        User->UserItems->Clear();

        TStringList *lInvent = new TStringList;
        String InventPath = ExePath + L"data\\invent.txt";

        if (FileExists(InventPath))
        {
            lInvent->LoadFromFile(InventPath, TEncoding::UTF8);
            for (int i = 0; i < lInvent->Count; i++)
            {
                String currentLine = lInvent->Strings[i].Trim();
                if (currentLine.IsEmpty()) continue;

                int lastSpace = currentLine.LastDelimiter(L" ");
                if (lastSpace > 0)
                {
                    String itemName = currentLine.SubString(1, lastSpace - 1).Trim();
                    String itemWeightStr = currentLine.SubString(lastSpace + 1, currentLine.Length() - lastSpace).Trim();
                    int itemWeight = StrToIntDef(itemWeightStr, 1);

                    SrcList->Items->AddObject(itemName, (TObject*)itemWeight);
                }
            }
        }
        else
        {
            SrcList->Items->AddObject(L"Старый кухонный нож", (TObject*)2);
            SrcList->Items->AddObject(L"Тяжелый стальной щит", (TObject*)15);
        }
        delete lInvent;

        for (int i = 0; i < SrcList->Items->Count; i++)
        {
            int itemWeight = (int)(SrcList->Items->Objects[i]);
            User->UserItems->AddObject(SrcList->Items->Strings[i], (TObject*)itemWeight);
        }
    }
    else
    {
        for (int i = 0; i < User->UserItems->Count; i++)
        {
            SrcList->Items->AddObject(User->UserItems->Strings[i], User->UserItems->Objects[i]);
        }
        for (int i = 0; i < User->GroundItems->Count; i++)
        {
            DstList->Items->AddObject(User->GroundItems->Strings[i], User->GroundItems->Objects[i]);
        }
    }

    UpdateWeightDisplay();
    SetButtons();
}

void __fastcall TDualListDlg::OKBtnClick(TObject *Sender)
{
    User->UserItems->Clear();
    for (int i = 0; i < SrcList->Items->Count; i++)
    {
        int itemWeight = (int)(SrcList->Items->Objects[i]);
        User->UserItems->AddObject(SrcList->Items->Strings[i], (TObject*)itemWeight);
    }

    User->GroundItems->Clear();
    for (int i = 0; i < DstList->Items->Count; i++)
    {
        int itemWeight = (int)(DstList->Items->Objects[i]);
        User->GroundItems->AddObject(DstList->Items->Strings[i], (TObject*)itemWeight);
    }

    ModalResult = mrOk;
}
