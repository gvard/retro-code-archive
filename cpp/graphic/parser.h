#ifndef parserH
#define parserH

#include <System.Classes.hpp>
#include <memory>
#include <vector>

// Математический узел дерева формулы
struct uzel
{
    std::vector<double> m;
    std::unique_ptr<uzel> l = nullptr;
    std::unique_ptr<uzel> r = nullptr;
};

// Прототипы функций парсера (теперь это свободные функции, не методы TForm1)
std::vector<double> initMas(double a_val, double b_val, int n_val, int &errorFlag);

void f(AnsiString &str,
       std::vector<double> &values,
       std::unique_ptr<uzel> &node,
       int n_val,
       int &errorFlag,
       const std::vector<double> &mas_x);

#endif
