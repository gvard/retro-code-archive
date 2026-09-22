#include "project_model.h"
#include "main_window.h"
#include <System.JSON.hpp>
#include <filesystem>

void loadFormulasFromJSON(TComboBox* ComboBox, std::vector<TFormulaConfig>& formulaLimits)
{
    std::filesystem::path exeDir = std::filesystem::path(ParamStr(0).c_str()).parent_path();
    std::filesystem::path jsonPath = exeDir / "functions.json";

    String filePath = jsonPath.c_str();
    TStringList* fileContent = new TStringList();

    formulaLimits.clear();
    ComboBox->Items->Clear();

    if (!std::filesystem::exists(jsonPath))
    {
        TJSONArray* baseArray = new TJSONArray();

        UnicodeString defaultFormulas[] = {"abs(x) * sin(5*x)", "1 / (1 + x^2)", "ln(x) / sqrt(x)", "(x > 0) * sin(x) + (x <= 0) * cos(x)"};
        double defaultA[] = {-5.0, -4.0, 0.01, -6.28};
        double defaultB[] = {5.0,  4.0,  20.0, 6.28};

        for (int i = 0; i < 4; i++)
        {
            TJSONObject* item = new TJSONObject();
            item->AddPair("formula", defaultFormulas[i]);
            item->AddPair("a", defaultA[i]);
            item->AddPair("b", defaultB[i]);
            baseArray->AddElement(item);
        }

        fileContent->Text = baseArray->ToString();
        fileContent->SaveToFile(filePath, TEncoding::UTF8);
        delete baseArray;
    }

    TJSONArray* jsonArray = nullptr;
    try
    {
        fileContent->LoadFromFile(filePath, TEncoding::UTF8);
        jsonArray = (TJSONArray*)TJSONObject::ParseJSONValue(fileContent->Text);

        if (jsonArray != nullptr)
        {
            TFormatSettings jsonLocale = TFormatSettings::Create("en-US");
            jsonLocale.DecimalSeparator = '.';

            for (int i = 0; i < jsonArray->Count; i++)
            {
                TJSONObject* item = (TJSONObject*)jsonArray->Items[i];

                UnicodeString formula = item->Values["formula"]->Value();

                double valA = StrToFloat(item->Values["a"]->Value(), jsonLocale);
                double valB = StrToFloat(item->Values["b"]->Value(), jsonLocale);

                ComboBox->Items->Add(formula);

                TFormulaConfig cfg = {valA, valB};
                formulaLimits.push_back(cfg); // Наполнение вектора формы
            }
        }
    }
    __finally
    {
        delete fileContent;
        if (jsonArray != nullptr)
            delete jsonArray;
    }
}

void loadProgramSettings(int& graphWidth, int& axisWidth, int& graphColor, int& axisColor, int& bgColor,
                         int& btnTop, int& btnRight, UnicodeString& fontName, int& fontSize, int& fontColor)
{
    std::filesystem::path exeDir = std::filesystem::path(ParamStr(0).c_str()).parent_path();
    std::filesystem::path configPath = exeDir / "settings.json";

    String filePath = configPath.c_str();
    TStringList* fileContent = new TStringList();

    graphWidth = 2;
    axisWidth = 2;
    graphColor = clBlack;
    axisColor = clRed;
    bgColor = clBtnFace;
    btnTop = 15;
    btnRight = 15;
    fontName = "Arial";
    fontSize = 10;
    fontColor = clBlack;

    if (!std::filesystem::exists(configPath))
    {
        TJSONObject* defaultSettings = new TJSONObject();
        defaultSettings->AddPair("graph_line_width", graphWidth);
        defaultSettings->AddPair("axis_line_width", axisWidth);
        defaultSettings->AddPair("graph_color", "0x" + IntToHex(graphColor, 6));
        defaultSettings->AddPair("axis_color", "0x" + IntToHex(axisColor, 6));
        defaultSettings->AddPair("fs_btn_top", btnTop);
        defaultSettings->AddPair("fs_btn_right", btnRight);
        defaultSettings->AddPair("font_name", fontName);
        defaultSettings->AddPair("font_size", fontSize);
        defaultSettings->AddPair("font_color", "0x" + IntToHex(fontColor, 6));
        defaultSettings->AddPair("bg_color", "0x" + IntToHex(bgColor, 6));

        fileContent->Text = defaultSettings->ToString();
        fileContent->SaveToFile(filePath, TEncoding::UTF8);
        delete defaultSettings;
    }

    TJSONObject* jsonSettings = nullptr;
    try
    {
        fileContent->LoadFromFile(filePath, TEncoding::UTF8);
        jsonSettings = (TJSONObject*)TJSONObject::ParseJSONValue(fileContent->Text);

        if (jsonSettings != nullptr)
        {
            if (jsonSettings->Values["graph_line_width"] != nullptr)
            {
                int raw = jsonSettings->Values["graph_line_width"]->Value().ToInt();
                graphWidth = (raw > 50 || raw < 1) ? 2 : raw;
            }
            if (jsonSettings->Values["axis_line_width"] != nullptr)
            {
                int raw = jsonSettings->Values["axis_line_width"]->Value().ToInt();
                axisWidth = (raw > 10 || raw < 1) ? 2 : raw;
            }

            if (jsonSettings->Values["graph_color"] != nullptr)
                graphColor = static_cast<int>(StrToInt64(jsonSettings->Values["graph_color"]->Value()));

            if (jsonSettings->Values["axis_color"] != nullptr)
                axisColor = static_cast<int>(StrToInt64(jsonSettings->Values["axis_color"]->Value()));

            if (jsonSettings->Values["bg_color"] != nullptr)
                bgColor = static_cast<int>(StrToInt64(jsonSettings->Values["bg_color"]->Value()));

            if (jsonSettings->Values["fs_btn_top"] != nullptr)
                btnTop = jsonSettings->Values["fs_btn_top"]->Value().ToInt();

            if (jsonSettings->Values["fs_btn_right"] != nullptr)
                btnRight = jsonSettings->Values["fs_btn_right"]->Value().ToInt();

            if (jsonSettings->Values["font_name"] != nullptr)
                fontName = jsonSettings->Values["font_name"]->Value();

            if (jsonSettings->Values["font_size"] != nullptr)
                fontSize = jsonSettings->Values["font_size"]->Value().ToInt();

            if (jsonSettings->Values["font_color"] != nullptr)
                fontColor = static_cast<int>(StrToInt64(jsonSettings->Values["font_color"]->Value()));
        }
    }
    __finally
    {
        delete fileContent;
        if (jsonSettings != nullptr)
            delete jsonSettings;
    }
}
