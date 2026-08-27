#include <vcl.h>
#pragma hdrstop

#include "uinfo.h"
#include "first.h"
#include "utest.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TfrmUInfo *frmUInfo;

__fastcall TfrmUInfo::TfrmUInfo(TComponent* Owner)
    : TForm(Owner)
{
}

void __fastcall TfrmUInfo::Button1Click(TObject *Sender)
{
    try
    {
        User->age = StrToInt(Edit2->Text);
    }
    catch (EConvertError&)
    {
        Application->MessageBox(L"Возраст задается в ДЕСЯТИЧНОЙ системе исчисления!", L"The RPG", MB_OK | MB_ICONWARNING);
        Edit2->SetFocus();
        return;
    }

    if (User->age > 100)
    {
        Application->MessageBox(L"Столько не живут!", L"The RPG", MB_OK | MB_ICONWARNING);
        Edit2->SetFocus();
        return;
    }

    if (User->age < 15)
    {
        Application->MessageBox(L"Такой маленький, а уже ноги чешутся из дому смотаться?!", L"The RPG", MB_OK | MB_ICONWARNING);
        Edit2->SetFocus();
        return;
    }

    if (User->age > 70)
    {
        Application->MessageBox(L"А по дороге не развалишься?!", L"The RPG", MB_OK | MB_ICONWARNING);
        Edit2->SetFocus();
        return;
    }

    // Данные валидны, сохраняем в объект игрока
    User->Name = Edit1->Text;
    User->CrType = ComboBox1->Text;
    User->SexType = ComboBox2->Text;

    // Скрываем текущую форму, чтобы она не мешала переходу
    this->Hide();

    // Безопасное создание и отображение формы теста
    if (frmUTest != nullptr)
    {
        delete frmUTest;
        frmUTest = nullptr;
    }
    Application->CreateForm(__classid(TfrmUTest), &frmUTest);
    frmUTest->Show();

    // Теперь, когда тест открыт, закрываем (и уничтожаем через caFree) окно ввода инфо
    frmUInfo->Close();
}

void __fastcall TfrmUInfo::Edit1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_RETURN)
    {
        ComboBox1->SetFocus();
    }
}

void __fastcall TfrmUInfo::Edit2KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_RETURN)
    {
        Button1->SetFocus();
    }
}

void __fastcall TfrmUInfo::frmUInfoClose(TObject *Sender, TCloseAction &Action)
{
    Action = caFree;

    // Если окно закрыли крестиком (а не перешли к тесту), возвращаем игрока в главное меню
    if (!frmUTest || !frmUTest->Visible)
    {
        frmFirst->Show();
    }
}
void __fastcall TfrmUInfo::FormDestroy(TObject *Sender)
{
    frmUInfo = nullptr;
}
