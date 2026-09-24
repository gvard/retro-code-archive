#ifndef parserH
#define parserH

#include <System.Classes.hpp>
#include <memory>
#include <vector>

std::vector<double> init_grid(double a_val, double b_val, int n_val, int& error_code);

void eval_formula(const AnsiString& formula_str,
                  std::vector<double>& values,
                  int n_val,
                  int& error_code,
                  std::vector<double>& x_values);
#endif
