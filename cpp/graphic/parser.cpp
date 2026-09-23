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
       const std::vector<double> &mas_x)
{
    if (errorFlag == 1) return;

    AnsiString modernStr = str;

    modernStr = System::Sysutils::StringReplace(modernStr, "lg(", "log10(", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
    modernStr = System::Sysutils::StringReplace(modernStr, "arctg(", "atan(", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
    modernStr = System::Sysutils::StringReplace(modernStr, "sh(", "sinh(", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
    modernStr = System::Sysutils::StringReplace(modernStr, "ch(", "cosh(", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
    modernStr = System::Sysutils::StringReplace(modernStr, "th(", "tanh(", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);

    // Автоматическое исправление приоритета степени для унарного минуса: -x^2 -> -(x^2)
    // Ищем паттерн "-x^" и превращаем его в "-(x^..." с закрытием скобки после показателя
    for (int i = 1; i <= modernStr.Length() - 3; i++)
    {
        if (modernStr[i] == '-' && (modernStr[i+1] == 'x' || modernStr[i+1] == 'X') && modernStr[i+2] == '^')
        {
            modernStr.Insert("(", i + 1); // Вставляем открывающую скобку перед 'x'

            // Ищем конец показателя степени (идем вправо по цифрам и точкам)
            int j = i + 4;
            while (j <= modernStr.Length() && (isdigit(modernStr[j]) || modernStr[j] == '.' || modernStr[j] == 'x' || modernStr[j] == 'X'))
            {
                j++;
            }
            modernStr.Insert(")", j);
        }
    }

    // Автоматическая замена модуля |x| на abs(x)
    int pipeCount = 0;
    for (int i = 1; i <= modernStr.Length(); i++)
    {
        if (modernStr[i] == '|') pipeCount++;
    }

    if (pipeCount > 0 && pipeCount % 2 == 0)
    {
        bool isOpen = true;
        for (int i = 1; i <= modernStr.Length(); i++)
        {
            if (modernStr[i] == '|')
            {
                if (isOpen)
                {
                    modernStr.Delete(i, 1);
                    modernStr.Insert("abs(", i);
                    isOpen = false;
                }
                else
                {
                    modernStr.Delete(i, 1);
                    modernStr.Insert(")", i);
                    isOpen = true;
                }
            }
        }
    }

    double x_variable = 0.0;
    te_variable vars[] = { {"x", &x_variable} };
    int compile_error = 0;

    te_expr* expr = te_compile(modernStr.c_str(), vars, 1, &compile_error);

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
