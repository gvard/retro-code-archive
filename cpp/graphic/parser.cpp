#include "parser.h"
#define _USE_MATH_DEFINES
#include <cmath>

#include "tinyexpr.h"

std::vector<double> initMas(double a_val, double b_val, int n_val, int &errorFlag)
{
    double h = (b_val - a_val) / n_val;
    std::vector<double> p_mas(n_val + 1);

    if (b_val <= a_val)
    {
        errorFlag = 1; // Устанавливаем флаг ошибки вместо вызова ShowMessage
        return p_mas;
    }

    p_mas[0] = a_val;
    for (int i = 1; i <= n_val; i++)
    {
        p_mas[i] = p_mas[i - 1] + h;
    }

    return p_mas;
}

void f(AnsiString &str,
       std::vector<double> &values,
       int n_val,
       int &errorFlag,
       std::vector<double> &mas_x)
{
    if (errorFlag == 1) return;

    AnsiString modernStr = str;

    // Вспомогательная лямбда-функция для предобработки строк
    auto preprocessStr = [](AnsiString &s) {
        s = System::Sysutils::StringReplace(s, "lg(", "log10(", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
        s = System::Sysutils::StringReplace(s, "arctg(", "atan(", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
        s = System::Sysutils::StringReplace(s, "sh(", "sinh(", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
        s = System::Sysutils::StringReplace(s, "ch(", "cosh(", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
        s = System::Sysutils::StringReplace(s, "th(", "tanh(", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);

        for (int i = 1; i <= s.Length() - 3; i++)
        {
            if (s[i] == '-' && (s[i+1] == 'x' || s[i+1] == 'X' || s[i+1] == 't' || s[i+1] == 'T') && s[i+2] == '^')
            {
                s.Insert("(", i + 1);
                int j = i + 4;
                while (j <= s.Length() && (isdigit(s[j]) || s[j] == '.' || s[j] == 'x' || s[j] == 'X' || s[j] == 't' || s[j] == 'T'))
                {
                    j++;
                }
                s.Insert(")", j);
            }
        }

        int pipeCount = 0;
        for (int i = 1; i <= s.Length(); i++) { if (s[i] == '|') pipeCount++; }

        if (pipeCount > 0 && pipeCount % 2 == 0)
        {
            bool isOpen = true;
            for (int i = 1; i <= s.Length(); i++)
            {
                if (s[i] == '|')
                {
                    if (isOpen) { s.Delete(i, 1); s.Insert("abs(", i); isOpen = false; }
                    else { s.Delete(i, 1); s.Insert(")", i); isOpen = true; }
                }
            }
        }
    };

    // Проверяем, является ли уравнение параметрическим (наличие ';')
    int semiColonPos = modernStr.Pos(";");

    if (semiColonPos > 0)
    {
        AnsiString strX = modernStr.SubString(1, semiColonPos - 1);
        AnsiString strY = modernStr.SubString(semiColonPos + 1, modernStr.Length() - semiColonPos);

        preprocessStr(strX);
        preprocessStr(strY);

        // Исходный mas_x сейчас хранит сетку шагов для параметра t, сохраняем её копию
        std::vector<double> mas_t = mas_x;

        double t_variable = 0.0;
        // Регистрируем и x, и t, чтобы пользователь мог писать в уравнениях любую из этих букв
        te_variable vars[] = { {"x", &t_variable}, {"t", &t_variable} };
        int compile_errorX = 0;
        int compile_errorY = 0;

        te_expr* exprX = te_compile(strX.c_str(), vars, 2, &compile_errorX);
        te_expr* exprY = te_compile(strY.c_str(), vars, 2, &compile_errorY);

        if (!exprX || !exprY)
        {
            if (exprX) te_free(exprX);
            if (exprY) te_free(exprY);
            errorFlag = 1;
            return;
        }

        for (int j = 0; j <= n_val; j++)
        {
            t_variable = mas_t[j]; // Привязываем текущее значение t

            // Считаем X(t)
            double x_val = te_eval(exprX);
            if (std::isnan(x_val) || std::isinf(x_val)) mas_x[j] = 1e300;
            else mas_x[j] = x_val;

            // Считаем Y(t)
            double y_val = te_eval(exprY);
            if (std::isnan(y_val) || std::isinf(y_val)) values[j] = 1e300;
            else values[j] = y_val;
        }

        te_free(exprX);
        te_free(exprY);
    }
    else
    {
        // Режим 2: обычное уравнение y = f(x)
        preprocessStr(modernStr);

        double x_variable = 0.0;
        te_variable vars[] = { {"x", &x_variable}, {"t", &x_variable} };
        int compile_error = 0;

        te_expr* expr = te_compile(modernStr.c_str(), vars, 2, &compile_error); // Передаем count = 2

        if (!expr)
        {
            errorFlag = 1;
            return;
        }

        for (int j = 0; j <= n_val; j++)
        {
            x_variable = mas_x[j];
            double y_val = te_eval(expr);

            if (std::isnan(y_val) || std::isinf(y_val))
            {
                values[j] = 1e300;
            }
            else
            {
                values[j] = y_val;
            }
        }

        te_free(expr);
    }
}
