#ifndef parserH
#define parserH

#include <System.Classes.hpp>
#include <memory>
#include <vector>

std::vector<double> initMas(double a_val, double b_val, int n_val, int &errorFlag);

void f(AnsiString &str,
       std::vector<double> &values,
       int n_val,
       int &errorFlag,
       std::vector<double> &mas_x);
#endif
