#include <charconv>
#include <string>
#include <string_view>
#include <system_error>

#include <Winapi.Windows.hpp>

#include "storage_manager.h"
#include "chargen_view.h"
#include "first.h"

#pragma resource "*.dfm"

__fastcall TfrmCharGen::TfrmCharGen(TComponent* Owner)
    : TForm(Owner)
{
}

void __fastcall TfrmCharGen::Button1Click(TObject* /*Sender*/)
{
    using namespace std::string_view_literals;
    constexpr auto title = L"The RPG"sv;

    const String user_name = Edit1->Text.Trim();
    if (user_name.IsEmpty())
    {
        constexpr auto msg = L"Имя персонажа не может быть пустым!"sv;
        ::MessageBoxW(this->Handle, msg.data(), title.data(), MB_OK | MB_ICONWARNING);
        Edit1->SetFocus();
        return;
    }

    if (user_name.Length() > 25)
    {
        constexpr auto msg = L"Имя персонажа не может быть длиннее 25 символов!"sv;
        ::MessageBoxW(this->Handle, msg.data(), title.data(), MB_OK | MB_ICONWARNING);
        Edit1->SetFocus();
        return;
    }

    std::wstring_view edit_text_view = Edit2->Text.Trim().c_str();

    std::string age_str;
    age_str.reserve(edit_text_view.size());
    for (wchar_t ch : edit_text_view)
    {
        age_str.push_back(static_cast<char>(ch));
    }

    int parsed_age = 0;
    auto [ptr, ec] = std::from_chars(age_str.data(), age_str.data() + age_str.size(), parsed_age);

    // Проверка на корректность числа (исключаем буквы, знаки препинания, пустоту)
    if (ec != std::errc{} || ptr != age_str.data() + age_str.size())
    {
        constexpr auto msg = L"Возраст задается в ДЕСЯТИЧНОЙ системе исчисления!"sv;
        ::MessageBoxW(this->Handle, msg.data(), title.data(), MB_OK | MB_ICONWARNING);
        Edit2->SetFocus();
        return;
    }

    if (parsed_age > 100)
    {
        constexpr auto msg = L"Столько не живут!"sv;
        ::MessageBoxW(this->Handle, msg.data(), title.data(), MB_OK | MB_ICONWARNING);
        Edit2->SetFocus();
        return;
    }
    if (parsed_age < 15)
    {
        constexpr auto msg = L"Такой маленький, а уже ноги чешутся из дому смотаться?!"sv;
        ::MessageBoxW(this->Handle, msg.data(), title.data(), MB_OK | MB_ICONWARNING);
        Edit2->SetFocus();
        return;
    }
    if (parsed_age > 70)
    {
        constexpr auto msg = L"А по дороге не развалишься?"sv;
        ::MessageBoxW(this->Handle, msg.data(), title.data(), MB_OK | MB_ICONWARNING);
        Edit2->SetFocus();
        return;
    }

    User->age = parsed_age;
    User->Name = user_name;
    User->CrType = cbRace->Text;
    User->SexType = cbGender->Text;

    // Чтение модификаторов расы из хранилища ресурсов
    const auto& races = storage_system::get().get_races();

    for (const auto& race : races)
    {
        // Превращаем std::wstring из файла в VCL String и сравниваем без учета регистра и пробелов
        if (AnsiSameText(String(race.name.c_str()).Trim(), User->CrType.Trim()))
        {
            User->str += race.modifier;
            break;
        }
    }

    this->ModalResult = mrOk;
}

void __fastcall TfrmCharGen::Edit1KeyDown(TObject* /*Sender*/, WORD& Key, TShiftState /*Shift*/)
{
    if (Key == VK_RETURN)
    {
        cbRace->SetFocus();
    }
}

void __fastcall TfrmCharGen::Edit2KeyDown(TObject* /*Sender*/, WORD& Key, TShiftState /*Shift*/)
{
    if (Key == VK_RETURN)
    {
        Button1->SetFocus();
    }
}

void __fastcall TfrmCharGen::FormShow(TObject* /*Sender*/)
{
    cbRace->Items->Clear();

    // Загрузка через лаконичный и безопасный storage_manager
    const auto& races = storage_system::get().get_races();
    for (const auto& race : races)
    {
        cbRace->Items->Add(race.name.c_str());
    }

    // Дефолтный фоллбек, если файл пуст или отсутствует
    if (cbRace->Items->Count == 0)
    {
        cbRace->Items->Add(L"Человек");
        cbRace->Items->Add(L"Эльф");
    }

    cbRace->ItemIndex = 0;
}
