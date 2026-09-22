#include "parser.h"
#define _USE_MATH_DEFINES
#include <cmath>

#ifdef USE_TINYEXPR
  #include "tinyexpr.h"
#endif

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
       std::unique_ptr<uzel> &node,
       int n_val,
       int &errorFlag,
       const std::vector<double> &mas_x)
{
    if (errorFlag == 1) return;

#ifdef USE_TINYEXPR
    // Препроцессор строки для TinyExpr
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

    // Сборка и вычисление через TinyExpr
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

#else
    node = std::make_unique<uzel>();
    node->m.resize(n_val + 1);

    // Раскрытие скобок в начале
    while (str.Length() > 0 && str[1] == '(' && str[str.Length()] == ')')
    {
        bracketCount = 1;
        int i = 1;
        while (bracketCount != 0 && i < str.Length())
        {
            i++;
            if (str[i] == '(')
                bracketCount++;
            if (str[i] == ')')
                bracketCount--;
        }

        if (i == str.Length() && bracketCount == 0)
        {
            str.Delete(1, 1);
            str.Delete(str.Length(), 1);
        }
        else
        {
            break;
        }
    }

    // Модуль
    while (str.Length() > 0 && str[1] == '|' && str[str.Length()] == '|')
    {
        int barCount = 1;
        int i = 1;
        while (barCount != 0 && i < str.Length())
        {
            i++;
            if (str[i] == '|')
            {
                barCount = (barCount == 1 ? 0 : 1);
            }
        }

        if (i == str.Length() && barCount == 0)
        {
            str.Delete(1, 1);
            str.Delete(str.Length(), 1);

            f(str, values, node->l, n_val, errorFlag, mas_x);

            for (int j = 0; j <= n_val; j++)
            {
                if (values[j] < 1e299 && !std::isnan(values[j]) && !std::isinf(values[j]))
                {
                    values[j] = std::abs(values[j]);
                }
            }
            return;
        }
        else
        {
            break;
        }
    }

    // Операторы: + и -
    for (int i = str.Length(); i >= 1; i--)
    {
        if (str[i] == '(')
            bracketCount++;
        if (str[i] == ')')
            bracketCount--;

        if (bracketCount == 0 && isOperatorFound == 0)
        {
            if (str[i] == '+' || str[i] == '-')
            {
                sign = (str[i] == '+' ? 1 : -1);
                isOperatorFound = 1;

                if (sign == -1 && i == 1)
                {
                    for (int j = 0; j <= n_val; j++)
                    {
                        values[j] = 0;
                    }
                }
                else
                {
                    AnsiString sub1 = str.SubString(1, i - 1);
                    f(sub1, values, node->l, n_val, errorFlag, mas_x);
                }

                AnsiString sub2 = str.SubString(i + 1, str.Length() - i);
                f(sub2, node->m, node->r, n_val, errorFlag, mas_x);

                for (int j = 0; j <= n_val; j++)
                {
                    values[j] = values[j] + sign * node->m[j];
                }

                break;
            }
        }
    }

    // Оператор: *
    if (isOperatorFound == 1)
        return;

    for (int i = str.Length(); i >= 1; i--)
    {
        if (str[i] == '(')
            bracketCount++;
        if (str[i] == ')')
            bracketCount--;

        if (bracketCount == 0 && isOperatorFound == 0)
        {
            if (str[i] == '*')
            {
                isOperatorFound = 1;
                AnsiString sub1 = str.SubString(1, i - 1);
                f(sub1, values, node->l, n_val, errorFlag, mas_x);

                AnsiString sub2 = str.SubString(i + 1, str.Length() - i);
                f(sub2, node->m, node->r, n_val, errorFlag, mas_x);

                for (int j = 0; j <= n_val; j++)
                {
                    if (values[j] >= 1e299 || node->m[j] >= 1e299)
                    {
                        values[j] = 1e300;
                    }
                    else
                    {
                        values[j] = values[j] * node->m[j];
                    }
                }
                break;
            }
        }
    }

    // Оператор: /
    if (isOperatorFound == 1)
        return;

    for (int i = str.Length(); i >= 1; i--)
    {
        if (str[i] == '(')
            bracketCount++;
        if (str[i] == ')')
            bracketCount--;

        if (bracketCount == 0 && isOperatorFound == 0)
        {
            if (str[i] == '/')
            {
                isOperatorFound = 1;
                AnsiString sub1 = str.SubString(1, i - 1);
                f(sub1, values, node->l, n_val, errorFlag, mas_x);

                AnsiString sub2 = str.SubString(i + 1, str.Length() - i);
                f(sub2, node->m, node->r, n_val, errorFlag, mas_x);

                for (int j = 0; j <= n_val; j++)
                {
                    if (std::abs(node->m[j]) <= 1e-10 || std::isnan(node->m[j]))
                    {
                        values[j] = 1e300;
                    }
                    else
                    {
                        values[j] = values[j] / node->m[j];
                    }
                }
                break;
            }
        }
    }

    // Оператор: ^
    if (isOperatorFound == 1)
        return;

    for (int i = str.Length(); i >= 1; i--)
    {
        if (str[i] == '(')
            bracketCount++;
        if (str[i] == ')')
            bracketCount--;

        if (bracketCount == 0 && isOperatorFound == 0)
        {
            if (str[i] == '^')
            {
                isOperatorFound = 1;
                AnsiString sub1 = str.SubString(1, i - 1);
                f(sub1, values, node->l, n_val, errorFlag, mas_x);

                AnsiString sub2 = str.SubString(i + 1, str.Length() - i);
                f(sub2, node->m, node->r, n_val, errorFlag, mas_x);

                for (int j = 0; j <= n_val; j++)
                {
                    values[j] = pow(values[j], node->m[j]);
                }

                break;
            }
        }
    }

    // Функции: sin, cos, ctg, tg, ln
    if (isOperatorFound == 1)
        return;

    AnsiString lowerStr = str.LowerCase();

    for (int i = 1; i <= lowerStr.Length(); i++)
    {
        if (lowerStr[i] == '(')
            bracketCount++;
        if (lowerStr[i] == ')')
            bracketCount--;

        if (bracketCount == 0)
        {
            functionType = 0;
            int offset = 0;

            if (lowerStr.SubString(i, 4) == "sin(")
            {
                functionType = 1;
                offset = 4;
            }
            else if (lowerStr.SubString(i, 4) == "cos(")
            {
                functionType = 2;
                offset = 4;
            }
            else if (lowerStr.SubString(i, 4) == "ctg(")
            {
                functionType = 3;
                offset = 4;
            }
            else if (lowerStr.SubString(i, 3) == "tg(")
            {
                functionType = 4;
                offset = 3;
            }
            else if (lowerStr.SubString(i, 3) == "in(")
            {
                functionType = 5;
                offset = 3;
            }
            else if (lowerStr.SubString(i, 3) == "ln(")
            {
                functionType = 5;
                offset = 3;
            }

            if (functionType >= 1 && functionType <= 5)
            {
                isOperatorFound = 1;

                AnsiString sub = str.SubString(i + offset, str.Length() - (i + offset));
                f(sub, node->m, node->l, n_val, errorFlag, mas_x);

                for (int j = 0; j <= n_val; j++)
                {
                    double arg = node->m[j];
                    switch (functionType)
                    {
                        case 1:
                            values[j] = sin(arg);
                            break;
                        case 2:
                            values[j] = cos(arg);
                            break;
                        case 3:
                            values[j] = (std::abs(sin(arg)) <= 1e-10 ? 1e300 : 1.0 / tan(arg));
                            break;
                        case 4:
                            values[j] = (std::abs(cos(arg)) <= 1e-10 ? 1e300 : tan(arg));
                            break;
                        case 5:
                            if (arg <= 1e-10 || std::isnan(arg) || std::isinf(arg))
                            {
                                values[j] = 1e300;
                            }
                            else
                            {
                                values[j] = log(arg);
                            }
                            break;
                    }
                }
                break;
            }
        }
    }

    if (isOperatorFound == 1)
        return;

    // Раскрытие скобок в конце
    while (str.Length() > 0 && str[1] == '(' && str[str.Length()] == ')')
    {
        bracketCount = 1;
        int i = 1;
        while (bracketCount != 0 && i < str.Length())
        {
            i++;
            if (str[i] == '(')
                bracketCount++;
            if (str[i] == ')')
                bracketCount--;
        }
        if (i == str.Length() && bracketCount == 0)
        {
            str.Delete(1, 1);
            str.Delete(str.Length(), 1);
        }
        else
        {
            break;
        }
    }

    if (str.AnsiPos(")") + str.AnsiPos("("))
    {
        errorFlag = 1;
        return;
    }

    AnsiString checkStr = str.LowerCase();

    if (checkStr == "x")
    {
        for (int j = 0; j <= n_val; j++)
        {
            values[j] = mas_x[j]; // Берем X из переданного локального массива параметров
        }
    }
    else if (checkStr == "pi")
    {
        for (int j = 0; j <= n_val; j++)
        {
            values[j] = M_PI;
        }
    }
    else if (checkStr == "e")
    {
        for (int j = 0; j <= n_val; j++)
        {
            values[j] = M_E;
        }
    }
    else if (str != "")
    {
        try
        {
            double val = str.ToDouble();
            for (int j = 0; j <= n_val; j++)
            {
                values[j] = val;
            }
        }
        catch (...)
        {
            errorFlag = 1;
            return;
        }
    }
    else
    {
        errorFlag = 1;
    }
}
#endif
}
