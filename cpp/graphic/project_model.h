#ifndef project_modelH
#define project_modelH

#include <System.Classes.hpp>
#include <Vcl.StdCtrls.hpp>
#include <vector>

struct TFormulaConfig;

void loadFormulasFromJSON(TComboBox* ComboBox, std::vector<TFormulaConfig>& formulaLimits);

void loadProgramSettings(int& graphWidth, int& axisWidth, int& graphColor, int& axisColor, int& bgColor,
                         int& btnTop, int& btnRight, UnicodeString& fontName, int& fontSize, int& fontColor);

#endif
