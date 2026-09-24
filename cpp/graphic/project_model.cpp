#include "project_model.h"
#include <System.JSON.hpp>
#include <filesystem>

void load_formulas_from_json(std::vector<TFormulaItem>& formula_items)
{
    std::filesystem::path exe_dir = std::filesystem::path(ParamStr(0).c_str()).parent_path();
    std::filesystem::path json_path = exe_dir / "functions.json";

    String file_path = json_path.c_str();
    TStringList* file_content = new TStringList();
    TJSONArray* json_array = nullptr;

    formula_items.clear();

    if (!std::filesystem::exists(json_path))
    {
        TJSONArray* base_array = new TJSONArray();

        UnicodeString default_formulas[] = {"abs(x) * sin(5*x)", "1 / (1 + x^2)", "ln(x) / sqrt(x)", "(x > 0) * sin(x) + (x <= 0) * cos(x)"};
        double default_a[] = {-5.0, -4.0, 0.01, -6.28};
        double default_b[] = {5.0,  4.0,  20.0, 6.28};

        for (int i = 0; i < 4; i++)
        {
            TJSONObject* item = new TJSONObject();
            item->AddPair("formula", default_formulas[i]);
            item->AddPair("a", default_a[i]);
            item->AddPair("b", default_b[i]);
            base_array->AddElement(item);
        }

        file_content->Text = base_array->ToString();
        file_content->SaveToFile(file_path, TEncoding::UTF8);
        delete base_array;
    }

    try
    {
        file_content->LoadFromFile(file_path, TEncoding::UTF8);

        TJSONValue* parsed_value = TJSONObject::ParseJSONValue(file_content->Text);
        if (parsed_value != nullptr)
        {
            json_array = static_cast<TJSONArray*>(parsed_value);

            TFormatSettings json_locale = TFormatSettings::Create("en-US");
            json_locale.DecimalSeparator = '.';

            for (int i = 0; i < json_array->Count; i++)
            {
                TJSONObject* item = static_cast<TJSONObject*>(json_array->Items[i]);

                TFormulaItem formula_data;
                formula_data.formula_str = item->Values["formula"]->Value();
                formula_data.config.a = StrToFloat(item->Values["a"]->Value(), json_locale);
                formula_data.config.b = StrToFloat(item->Values["b"]->Value(), json_locale);

                formula_items.push_back(formula_data);
            }
        }
    }
    __finally
    {
        delete file_content;
        if (json_array != nullptr)
        {
            delete json_array;
        }
    }
}

void load_program_settings(TProgramSettings& settings)
{
    std::filesystem::path exe_dir = std::filesystem::path(ParamStr(0).c_str()).parent_path();
    std::filesystem::path config_path = exe_dir / "settings.json";

    String file_path = config_path.c_str();
    TStringList* file_content = new TStringList();
    TJSONObject* json_settings = nullptr;

    // Заполнение дефолтных значений структуры
    settings.graph_width = 2;
    settings.axis_width = 2;
    settings.graph_color = clBlack;
    settings.axis_color = clRed;
    settings.bg_color = clBtnFace;
    settings.btn_top = 15;
    settings.btn_right = 15;
    settings.font_name = "Arial";
    settings.font_size = 10;
    settings.font_color = clBlack;

    if (!std::filesystem::exists(config_path))
    {
        TJSONObject* default_settings = new TJSONObject();
        default_settings->AddPair("graph_line_width", settings.graph_width);
        default_settings->AddPair("axis_line_width", settings.axis_width);
        default_settings->AddPair("graph_color", "0x" + IntToHex(settings.graph_color, 6));
        default_settings->AddPair("axis_color", "0x" + IntToHex(settings.axis_color, 6));
        default_settings->AddPair("fs_btn_top", settings.btn_top);
        default_settings->AddPair("fs_btn_right", settings.btn_right);
        default_settings->AddPair("font_name", settings.font_name);
        default_settings->AddPair("font_size", settings.font_size);
        default_settings->AddPair("font_color", "0x" + IntToHex(settings.font_color, 6));
        default_settings->AddPair("bg_color", "0x" + IntToHex(settings.bg_color, 6));

        file_content->Text = default_settings->ToString();
        file_content->SaveToFile(file_path, TEncoding::UTF8);
        delete default_settings;
    }

    try
    {
        file_content->LoadFromFile(file_path, TEncoding::UTF8);

        // ИСПРАВЛЕНО: Защищенный вызов парсера
        TJSONValue* parsed_value = TJSONObject::ParseJSONValue(file_content->Text);
        if (parsed_value != nullptr)
        {
            json_settings = static_cast<TJSONObject*>(parsed_value);

            if (json_settings->Values["graph_line_width"] != nullptr)
            {
                int raw = json_settings->Values["graph_line_width"]->Value().ToInt();
                settings.graph_width = (raw > 50 || raw < 1) ? 2 : raw;
            }
            if (json_settings->Values["axis_line_width"] != nullptr)
            {
                int raw = json_settings->Values["axis_line_width"]->Value().ToInt();
                settings.axis_width = (raw > 10 || raw < 1) ? 2 : raw;
            }

            if (json_settings->Values["graph_color"] != nullptr)
                settings.graph_color = static_cast<int>(StrToInt64(json_settings->Values["graph_color"]->Value()));

            if (json_settings->Values["axis_color"] != nullptr)
                settings.axis_color = static_cast<int>(StrToInt64(json_settings->Values["axis_color"]->Value()));

            if (json_settings->Values["bg_color"] != nullptr)
                settings.bg_color = static_cast<int>(StrToInt64(json_settings->Values["bg_color"]->Value()));

            if (json_settings->Values["fs_btn_top"] != nullptr)
                settings.btn_top = json_settings->Values["fs_btn_top"]->Value().ToInt();

            if (json_settings->Values["fs_btn_right"] != nullptr)
                settings.btn_right = json_settings->Values["fs_btn_right"]->Value().ToInt();

            if (json_settings->Values["font_name"] != nullptr)
                settings.font_name = json_settings->Values["font_name"]->Value();

            if (json_settings->Values["font_size"] != nullptr)
                settings.font_size = json_settings->Values["font_size"]->Value().ToInt();

            if (json_settings->Values["font_color"] != nullptr)
                settings.font_color = static_cast<int>(StrToInt64(json_settings->Values["font_color"]->Value()));
        }
    }
    __finally
    {
        delete file_content;
        if (json_settings != nullptr)
        {
            delete json_settings;
        }
    }
}
