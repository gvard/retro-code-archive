#ifndef parserH
#define parserH

#include <System.Classes.hpp>
#include <memory>
#include <vector>

// Переключатель парсеров
// Оставьте активным для TinyExpr. Закомментируйте для возврата к старому коду.
#define USE_TINYEXPR

#ifndef USE_TINYEXPR
// Оригинальный математический узел дерева формулы (нужен для отката)
struct uzel
{
    std::vector<double> m;
    std::unique_ptr<uzel> l = nullptr;
    std::unique_ptr<uzel> r = nullptr;
};
#else
// Заглушка для совместимости с полем в TForm1
struct uzel
{
};
#endif


// Прототипы функций парсера (остаются сквозными и неизменными для обоих движков)
std::vector<double> initMas(double a_val, double b_val, int n_val, int &errorFlag);

void f(AnsiString &str,
       std::vector<double> &values,
       std::unique_ptr<uzel> &node,
       int n_val,
       int &errorFlag,
       const std::vector<double> &mas_x);

#endif
