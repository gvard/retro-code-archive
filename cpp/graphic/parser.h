#ifndef PARSER_H
#define PARSER_H

#include <vcl.h>
#include <vector>

struct TFormulaConfig
{
    double a = 0.0;
    double b = 0.0;
};

std::vector<double> init_grid(double a_val, double b_val, int n_val, int& error_code);

void eval_formula(const AnsiString& formula_str,
                  std::vector<double>& y_values,
                  int n_val,
                  int& error_code,
                  std::vector<double>& x_values);

#endif
