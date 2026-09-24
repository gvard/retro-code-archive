#include "parser.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <cctype>

#include "tinyexpr.h"

std::vector<double> init_grid(double a_val, double b_val, int n_val, int& error_code)
{
    double h = (b_val - a_val) / n_val;
    std::vector<double> grid_values(n_val + 1);

    if (b_val <= a_val)
    {
        error_code = 1;
        return grid_values;
    }

    grid_values[0] = a_val;
    for (int i = 1; i <= n_val; i++)
    {
        grid_values[i] = grid_values[i - 1] + h;
    }

    return grid_values;
}

static void preprocess_formula_string(AnsiString& s)
{
    s = System::Sysutils::StringReplace(s, "lg(", "log10(", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
    s = System::Sysutils::StringReplace(s, "arctg(", "atan(", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
    s = System::Sysutils::StringReplace(s, "sh(", "sinh(", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
    s = System::Sysutils::StringReplace(s, "ch(", "cosh(", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);
    s = System::Sysutils::StringReplace(s, "th(", "tanh(", TReplaceFlags() << rfReplaceAll << rfIgnoreCase);

    // Автоматическое исправление приоритета степени для унарного минуса: -x^2 -> -(x^2)
    for (int i = 1; i <= s.Length() - 3; i++)
    {
        if (s[i] == '-' && (s[i + 1] == 'x' || s[i + 1] == 'X' || s[i + 1] == 't' || s[i + 1] == 'T') && s[i + 2] == '^')
        {
            s.Insert("(", i + 1);
            int j = i + 4;
            while (j <= s.Length() && (std::isdigit(static_cast<unsigned char>(s[j])) || s[j] == '.' || s[j] == 'x' || s[j] == 'X' || s[j] == 't' || s[j] == 'T'))
            {
                j++;
            }
            s.Insert(")", j);
        }
    }

    // Автоматическая замена модуля |x| на abs(x)
    int pipe_count = 0;
    for (int i = 1; i <= s.Length(); i++)
    {
        if (s[i] == '|')
            pipe_count++;
    }

    if (pipe_count > 0 && pipe_count % 2 == 0)
    {
        bool is_open = true;
        for (int i = 1; i <= s.Length(); i++)
        {
            if (s[i] == '|')
            {
                if (is_open)
                {
                    s.Delete(i, 1);
                    s.Insert("abs(", i);
                    is_open = false;
                }
                else
                {
                    s.Delete(i, 1);
                    s.Insert(")", i);
                    is_open = true;
                }
            }
        }
    }
}

static void tabulate_grid(te_expr* expr_x, te_expr* expr_y, double& t_var,
                          int n_val, std::vector<double>& y_values, std::vector<double>& x_values)
{
    if (expr_x == nullptr)
    {
        for (int j = 0; j <= n_val; j++)
        {
            t_var = x_values[j];
            double y_val = te_eval(expr_y);

            if (std::isnan(y_val) || std::isinf(y_val))
                y_values[j] = 1e300;
            else
                y_values[j] = y_val;
        }
    }
    // Если заданы оба выражения - это параметрический режим x(t); y(t)
    else
    {
        std::vector<double> t_values = x_values; // Сохраняем копию сетки параметра t
        for (int j = 0; j <= n_val; j++)
        {
            t_var = t_values[j];

            double x_val = te_eval(expr_x);
            if (std::isnan(x_val) || std::isinf(x_val))
                x_values[j] = 1e300;
            else
                x_values[j] = x_val;

            double y_val = te_eval(expr_y);
            if (std::isnan(y_val) || std::isinf(y_val))
                y_values[j] = 1e300;
            else
                y_values[j] = y_val;
        }
    }
}

void eval_formula(const AnsiString& formula_str,
                  std::vector<double>& y_values,
                  int n_val,
                  int& error_code,
                  std::vector<double>& x_values)
{
    if (error_code == 1)
        return;

    AnsiString modern_str = formula_str;
    int semi_colon_pos = modern_str.Pos(";");

    double t_variable = 0.0;
    te_variable vars[] = {{"x", &t_variable}, {"t", &t_variable}};

    if (semi_colon_pos > 0)
    {
        AnsiString str_x = modern_str.SubString(1, semi_colon_pos - 1);
        AnsiString str_y = modern_str.SubString(semi_colon_pos + 1, modern_str.Length() - semi_colon_pos);

        preprocess_formula_string(str_x);
        preprocess_formula_string(str_y);

        int err_x = 0, err_y = 0;
        te_expr* expr_x = te_compile(str_x.c_str(), vars, 2, &err_x);
        te_expr* expr_y = te_compile(str_y.c_str(), vars, 2, &err_y);

        if (!expr_x || !expr_y)
        {
            if (expr_x)
                te_free(expr_x);
            if (expr_y)
                te_free(expr_y);
            error_code = 1;
            return;
        }

        tabulate_grid(expr_x, expr_y, t_variable, n_val, y_values, x_values);

        te_free(expr_x);
        te_free(expr_y);
    }
    else
    {
        preprocess_formula_string(modern_str);

        int err = 0;
        te_expr* expr = te_compile(modern_str.c_str(), vars, 2, &err);

        if (!expr)
        {
            error_code = 1;
            return;
        }

        tabulate_grid(nullptr, expr, t_variable, n_val, y_values, x_values);

        te_free(expr);
    }
}
