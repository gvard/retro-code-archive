#include <charconv>
#include <string>
#include <string_view>
#include <system_error>

#include <Winapi.Windows.hpp>

#include "uinfo.h"
#include "first.h"
#include "storage_manager.h"

#pragma resource "*.dfm"

static auto parse_race_line(const String& ALine) -> race_data
{
    race_data data{.name = L"", .modifier = 0};

    String currentLine = ALine.Trim();
    if (currentLine.IsEmpty())
        return data;

    int lastSpace = currentLine.LastDelimiter(L" ");
    if (lastSpace > 0)
    {
        data.name = currentLine.SubString(1, lastSpace - 1).Trim();
        String modStr = currentLine.SubString(lastSpace + 1, currentLine.Length() - lastSpace).Trim();

        if (modStr.Pos(L"+") == 1)
        {
            modStr = modStr.SubString(2, modStr.Length() - 1);
        }
        data.modifier = StrToIntDef(modStr, 0);
    }
    else
    {
        data.name = currentLine;
    }
    return data;
}

__fastcall TfrmUInfo::TfrmUInfo(TComponent* Owner)
    : TForm(Owner)
{
}

void __fastcall TfrmUInfo::Button1Click(TObject* /*Sender*/)
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
    User->CrType = ComboBox1->Text;
    User->SexType = ComboBox2->Text;

    // Чтение модификаторов расы из хранилища ресурсов
    const auto res = storage_system::get().load_race_file();
    if (res.is_loaded)
    {
        const String user_race_lower = User->CrType.LowerCase();

        for (const auto& raw_line : res.lines)
        {
            const String file_line = String(raw_line.c_str());

            if (file_line.LowerCase().Pos(user_race_lower) > 0)
            {
                const race_data race = parse_race_line(file_line);
                User->str += race.modifier;

                const String log_msg = L"[ОТЛАДКА uinfo] Раса: " + User->CrType +
                                       L" | Модификатор: " + IntToStr(race.modifier) +
                                       L" | Итоговая сила: " + IntToStr(User->str);
                OutputDebugString(log_msg.c_str());
                break;
            }
        }
    }

    // Проверки пройдены успешно — только ТЕПЕРЬ закрываем модальное окно
    this->ModalResult = mrOk;
}

void __fastcall TfrmUInfo::Edit1KeyDown(TObject* /*Sender*/, WORD& Key, TShiftState /*Shift*/)
{
    if (Key == VK_RETURN)
    {
        ComboBox1->SetFocus();
    }
}

void __fastcall TfrmUInfo::Edit2KeyDown(TObject* /*Sender*/, WORD& Key, TShiftState /*Shift*/)
{
    if (Key == VK_RETURN)
    {
        Button1->SetFocus();
    }
}

void __fastcall TfrmUInfo::FormShow(TObject* /*Sender*/)
{
    ComboBox1->Items->Clear();

    // Загрузка через лаконичный и безопасный storage_manager
    const auto res = storage_system::get().load_race_file();

    if (res.is_loaded)
    {
        for (const auto& raw_line : res.lines)
        {
            const race_data race = parse_race_line(String(raw_line.c_str()));
            if (!race.name.IsEmpty())
            {
                ComboBox1->Items->Add(race.name);
            }
        }
    }

    // Дефолтный фоллбек, если файл пуст или отсутствует
    if (ComboBox1->Items->Count == 0)
    {
        ComboBox1->Items->Add(L"Человек");
        ComboBox1->Items->Add(L"Эльф");
    }

    ComboBox1->ItemIndex = 0;
}
