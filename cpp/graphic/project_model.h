#ifndef PROJECT_MODEL_H
#define PROJECT_MODEL_H

#include "parser.h"
#include <vector>
#include <vcl.h>

struct TFormulaItem
{
    UnicodeString formula_str;
    TFormulaConfig config;
};

struct TProgramSettings
{
    int graph_width = 2;
    int axis_width = 2;
    int graph_color = 0;
    int axis_color = 0;
    int bg_color = 0;
    int btn_top = 15;
    int btn_right = 15;
    UnicodeString font_name = "Arial";
    int font_size = 10;
    int font_color = 0;
};

void load_formulas_from_json(std::vector<TFormulaItem>& formula_items);
void load_program_settings(TProgramSettings& settings);

#endif
