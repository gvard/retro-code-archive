#include <vcl.h>
#pragma hdrstop
#include "gr.h"

#define _USE_MATH_DEFINES
#include <cmath>

#pragma package(smart_init)
#pragma resource "*.dfm"

#include <System.JSON.hpp>
#include <filesystem>
#include <vector>

TForm1 *Form1;

struct TFormulaConfig
{
    double a;
    double b;
};

std::vector<TFormulaConfig> FormulaLimits;

__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
    n = 0;
    er = 0;
    a = 0;
    b = 0;
    is_first_graph = true;
    FIsFullscreen = false;
    Mas = nullptr;
    Res = nullptr;
    p = nullptr;
}

double* TForm1::initMas(double a_val, double b_val, int n_val)
{
    double h = (b_val - a_val) / n_val;
    double *p_mas = new double[n_val + 1];

    if (b_val <= a_val)
    {
        ShowMessage("Левая граница должна быть строго меньше правой!");
        this->er = 1;
        return p_mas;
    }

    *p_mas = a_val;
    for (int i = 1; i <= n_val; i++)
    {
        *(p_mas + i) = *(p_mas + i - 1) + h;
    }

    return p_mas;
}

// Передача AnsiString по константной ссылке исключает лишние копирования
void TForm1::f(AnsiString &str, double *values, uzel *&node)
{
    int bracketCount = 0;
    int functionType = 0;
    int isOperatorFound = 0;
    int sign = 1;

    if (this->er == 1)
    {
        return;
    }

    node = new uzel;
    node->m = new double[this->n + 1];
    node->l = nullptr;
    node->r = nullptr;

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
        // знаки модуля закрывают именно друг друга,
        // а не являются независимыми модулями
        while (barCount != 0 && i < str.Length())
        {
            i++;
            // В отличие от скобок, знак модуля одинаковый с обеих сторон.
            // Считаем её за признак изменения вложенности.
            if (str[i] == '|')
            {
                barCount = (barCount == 1 ? 0 : 1);
            }
        }

        // Если нашли парный закрывающий модуль на самом конце строки
        if (i == str.Length() && barCount == 0)
        {
            // Удаляем крайние символы '|'
            str.Delete(1, 1);
            str.Delete(str.Length(), 1);

            // Рекурсивно вычисляем то, что было внутри модуля
            f(str, values, node->l);

            // Применяем математическую функцию модуля ко всем вычисленным точкам
            for (int j = 0; j <= this->n; j++)
            {
                // Проверяем на маркер ошибки, чтобы не сломать логику разрывов
                if (values[j] < 1e299 && !std::isnan(values[j]) && !std::isinf(values[j]))
                {
                    values[j] = std::abs(values[j]);
                }
            }
            // Так как модуль обработан и строка полностью разобрана, выходим из функции
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
                    for (int j = 0; j <= this->n; j++)
                    {
                        values[j] = 0;
                    }
                }
                else
                {
                    AnsiString sub1 = str.SubString(1, i - 1);
                    f(sub1, values, node->l);
                }

                AnsiString sub2 = str.SubString(i + 1, str.Length() - i);
                f(sub2, node->m, node->r);

                for (int j = 0; j <= this->n; j++)
                {
                    values[j] = values[j] + sign * (*((node->m) + j));
                }

                delete[] node->m;
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
                f(sub1, values, node->l);

                AnsiString sub2 = str.SubString(i + 1, str.Length() - i);
                f(sub2, node->m, node->r);

                for (int j = 0; j <= this->n; j++)
            {
                // Если левый или правый операнд - это маркер ошибки
                if (values[j] >= 1e299 || *(node->m + j) >= 1e299)
                {
                    values[j] = 1e300; // Пробрасываем ошибку дальше
                }
                else
                {
                    values[j] = values[j] * (*((node->m) + j));
                }
            }
                delete[] node->m;
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
                f(sub1, values, node->l);

                AnsiString sub2 = str.SubString(i + 1, str.Length() - i);
                f(sub2, node->m, node->r);

                for (int j = 0; j <= this->n; j++)
                {
                    if (std::abs(*(node->m + j)) <= 1e-10 || std::isnan(*(node->m + j)))
                    {
                        values[j] = 1e300;
                    }
                    else
                    {
                        values[j] = values[j] / (*((node->m) + j));
                    }
                }
                delete[] node->m;
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
                f(sub1, values, node->l);

                AnsiString sub2 = str.SubString(i + 1, str.Length() - i);
                f(sub2, node->m, node->r);

                for (int j = 0; j <= this->n; j++)
                {
                    values[j] = pow(values[j], *(node->m + j));
                }

                delete[] node->m;
                break;
            }
        }
    }

    // Математические функции: sin, cos, ctg, tg, ln
    if (isOperatorFound == 1)
        return;

    // Приведение строки к нижнему регистру
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

                // Исправлено: точное выделение подстроки без концевой скобки
                AnsiString sub = str.SubString(i + offset, str.Length() - (i + offset));
                f(sub, node->m, node->l);

                for (int j = 0; j <= this->n; j++)
                {
                    double arg = *(node->m + j);
                    switch (functionType)
                    {
                        case 1: values[j] = sin(arg); break;
                        case 2: values[j] = cos(arg); break;
                        case 3: values[j] = (std::abs(sin(arg)) <= 1e-10 ? 1e300 : 1.0 / tan(arg)); break;
                        case 4: values[j] = (std::abs(cos(arg)) <= 1e-10 ? 1e300 : tan(arg)); break;
                        case 5:
                            if (arg <= 1e-10 || std::isnan(arg) || std::isinf(arg))
                            {
                                values[j] = 1e300;
                            }
                            else
                            {
                                if (arg > 0.0)
                                {
                                    values[j] = log(arg);
                                }
                                else
                                {
                                    values[j] = 1e300;
                                }
                            }
                            break;
                    }
                }

                delete[] node->m;
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
        ShowMessage("Проверьте скобки в вашем выражении!");
        this->er = 1;
        return;
    }

    AnsiString checkStr = str.LowerCase();

    if (checkStr == "x")
    {
        for (int j = 0; j <= this->n; j++)
        {
            values[j] = this->Mas[j];
        }
    }
    else if (checkStr == "pi")
    {
        for (int j = 0; j <= this->n; j++)
        {
            values[j] = M_PI;
        }
    }
    else if (checkStr == "e")
    {
        for (int j = 0; j <= this->n; j++)
        {
            values[j] = M_E;
        }
    }
    else if (str != "")
    {
        try
        {
            double val = str.ToDouble();
            for (int j = 0; j <= this->n; j++)
            {
                values[j] = val;
            }
        }
        catch (const EConvertError&)
        {
            ShowMessage("Ошибка в записи формулы на элементе: " + str);
            this->er = 1;
            return;
        }
    }
    else
    {
        ShowMessage("Проверьте скобки в вашем выражении!");
        this->er = 1;
    }
}

void __fastcall TForm1::Button1Click(TObject *Sender)
{
    int corr = -1;
    int bracketCount = 0;
    double mi = 0, ma = 0;
    bool needDrawLabels = (!CheckBox1->Checked || is_first_graph || Sender == nullptr);

    if (Sender != nullptr)
    {
        try
        {
            UnicodeString strA = Edit2->Text;
            UnicodeString strB = Edit3->Text;
            strA = System::Sysutils::StringReplace(strA, L",", L".", TReplaceFlags() << rfReplaceAll);
            strB = System::Sysutils::StringReplace(strB, L",", L".", TReplaceFlags() << rfReplaceAll);

            double temp_a = strA != "" ? strA.ToDouble() : this->a;
            double temp_b = strB != "" ? strB.ToDouble() : this->b;

            // Если текущие границы в полях ввода не совпадают с сохраненными границами
            if (!is_first_graph && (temp_a != saved_a || temp_b != saved_b))
            {
                CheckBox1->Checked = false;
                is_first_graph = true;
            }
        }
        catch (const EConvertError&)
        {
            // Ошибки конвертации обработает оригинальный try-catch ниже в коде
        }
    }

    if (CheckBox1->Checked && !is_first_graph)
    {
        Edit2->Text = FloatToStr(saved_a);
        Edit3->Text = FloatToStr(saved_b);
    }

    if (this->er == 1)
    {
        this->er = 0;
    }

    if (!CheckBox1->Checked)
    {
        PaintBox1->Repaint();

        // Очищаем старое дерево формулы, если оно существовало
        if (this->p != nullptr)
        {
            this->p = nullptr;
        }
    }
    else
    {
        this->p = nullptr;
    }

    this->n = PaintBox1->Width - 1;

    TPoint *v = new TPoint[this->n + 1];
    TPoint *o1 = new TPoint[2];
    TPoint *o2 = new TPoint[2];

    this->s = AnsiString(ComboBox1->Text);
    for (int i = 1; i <= this->s.Length(); i++)
    {
        if (this->s[i] == '(')
            bracketCount++;
        if (this->s[i] == ')')
            bracketCount--;
    }

    if (bracketCount != 0)
    {
        ShowMessage("Проверьте скобки в вашем выражении!");
        delete[] v;
        delete[] o1;
        delete[] o2;
        return;
    }

    try
    {
        UnicodeString strA = Edit2->Text;
        UnicodeString strB = Edit3->Text;

        strA = System::Sysutils::StringReplace(strA, L",", L".", TReplaceFlags() << rfReplaceAll);
        strB = System::Sysutils::StringReplace(strB, L",", L".", TReplaceFlags() << rfReplaceAll);

        if (strA != "") this->a = strA.ToDouble();
        if (strB != "") this->b = strB.ToDouble();
    }
    catch (const EConvertError&)
    {
        ShowMessage("Введите границы вывода в числовом виде! Допускаются как точки, так и запятые.");
        delete[] v;
        delete[] o1;
        delete[] o2;
        return;
    }

    delete[] Mas;
    this->Mas = initMas(this->a, this->b, this->n);
    this->Res = new double[this->n + 1];

    // Безопасный вызов парсера f() с отловом аппаратных исключений
    try
    {
        f(this->s, this->Res, this->p);
    }
    catch (...)
    {
        // В случае критического математического сбоя заполняем массив маркерами разрыва
        for (int i = 0; i <= this->n; i++) this->Res[i] = 1e300;
    }

    bool firstValid = false;
    for (int i = 0; i <= this->n; i++)
    {
        // Проверяем, что точка валидна, не бесконечна и не NaN
        if (this->Res[i] < 1e299 && !std::isinf(this->Res[i]) && !std::isnan(this->Res[i]))
        {
            if (!firstValid)
            {
                mi = this->Res[i];
                ma = this->Res[i];
                firstValid = true;
            }
            else
            {
                if (mi > this->Res[i]) mi = this->Res[i];
                if (ma < this->Res[i]) ma = this->Res[i];
            }
        }
    }

    // Если вообще все точки оказались ошибочными (например, ln(-5))
    if (!firstValid)
    {
        mi = -1.0;
        ma = 1.0;
    }
    mi = (mi > 0 ? 0 : mi);
    ma = (ma < 0 ? 0 : ma);

    if (Sender != nullptr && (!CheckBox1->Checked || is_first_graph))
    {
        saved_mi = mi;
        saved_ma = ma;
        saved_a = this->a;
        saved_b = this->b;
        is_first_graph = false;
    }
    else if (Sender == nullptr || CheckBox1->Checked)
    {
        // Восстанавливаем точные пропорции при изменении экрана или наложении
        mi = saved_mi;
        ma = saved_ma;
        this->a = saved_a;
        this->b = saved_b;
    }

    if (ma == 0)
    {
        corr = 1;
    }

    int padTop = 20;
    int padBottom = 14;
    int padLeft = 20;
    int padRight = 25;

    int workHeight = PaintBox1->Height - padTop - padBottom;
    int workWidth = PaintBox1->Width - padLeft - padRight;

    for (int i = 0; i <= this->n; i++)
    {
        int screenX = padLeft + floor((double)i * workWidth / this->n);
        int screenY = padTop + floor(workHeight * (ma - this->Res[i]) / (ma - mi)) + (mi * ma == 0 ? corr : 0);
        v[i] = Point(screenX, screenY);
    }

    if (CheckBox1->Checked)
    {
        // Генерирует случайный цвет, исключая слишком светлые
        PaintBox1->Canvas->Pen->Color = (TColor)RGB(rand()%200, rand()%200, rand()%200);
    }
    else
    {
        PaintBox1->Canvas->Pen->Color = clBlack;
        PaintBox1->Canvas->Pen->Width = 1;
    }
    bool drawing = false; // Флаг: ведем ли мы сейчас линию

    for (int i = 0; i <= this->n; i++)
    {
        // Проверяем точку на валидность
        if (this->Res[i] < 1e299 && !std::isinf(this->Res[i]) && !std::isnan(this->Res[i]))
        {
            if (!drawing)
            {
                // Начинаем новую линию с этой корректной точки
                PaintBox1->Canvas->MoveTo(v[i].X, v[i].Y);
                drawing = true;
            }
            else
            {
                // Продолжаем существующую линию
                PaintBox1->Canvas->LineTo(v[i].X, v[i].Y);
            }
        }
        else
        {
            drawing = false;
        }
    }

    PaintBox1->Canvas->Pen->Color = clRed;
    PaintBox1->Canvas->Brush->Color = clRed;
    PaintBox1->Canvas->Pen->Width = 2;

    // Ось X
    int posY = padTop + floor(workHeight * ma / (ma - mi) + (mi * ma == 0 ? corr : 0));
    if (posY < padTop) posY = padTop;
    if (posY > PaintBox1->Height - padBottom) posY = PaintBox1->Height - padBottom;

    int rightEdge = PaintBox1->Width - padRight;
    o1[0] = Point(padLeft, posY);
    o1[1] = Point(rightEdge, posY);
    PaintBox1->Canvas->Polyline(o1, 1);

    TPoint arrowX[3];
    arrowX[0] = Point(rightEdge, posY);
    arrowX[1] = Point(rightEdge - 8, posY - 4);
    arrowX[2] = Point(rightEdge - 8, posY + 4);
    PaintBox1->Canvas->Polygon(arrowX, 2);

    // Ось Y
    int posX = 0;
    if (this->a <= 0 && this->b >= 0)
    {
        posX = padLeft + floor(workWidth * (0 - this->a) / (this->b - this->a));
    }
    else if (this->a > 0)
    {
        posX = padLeft;
    }
    else
    {
        posX = PaintBox1->Width - padRight;
    }

    o2[0] = Point(posX, PaintBox1->Height - padBottom);
    o2[1] = Point(posX, 5);
    PaintBox1->Canvas->Polyline(o2, 1);

    TPoint arrowY[3];
    arrowY[0] = Point(posX, 5);
    arrowY[1] = Point(posX - 4, 13);
    arrowY[2] = Point(posX + 4, 13);
    PaintBox1->Canvas->Polygon(arrowY, 2);

    // Засечки оси X
    const int MAX_MARKS_X = 20;
    double deltaX = this->b - this->a;
    double stepX = 0.5;

    // Если количество рисок превышает лимит,
    // пересчитываем шаг адаптивно
    if ((deltaX / stepX) > MAX_MARKS_X)
    {
        // Выбираем более крупный шаг
        stepX = ceil(deltaX / (double)MAX_MARKS_X);
        if (stepX < 1.0) stepX = 1.0;
    }

    // Вычисляем стартовую и конечную точки цикла с учетом нового шага
    double startX = ceil(this->a / stepX) * stepX;
    double endX = floor(this->b / stepX) * stepX;
    int iterationsCountX = 0;

    // Поиск ближайшей к нулю риски
    double closestX = 1e300; // Хранит минимальное расстояние до нуля
    double bestValX = 0.0;   // Значение координаты наилучшей риски

    // Находим, какая риска из реально прошедших фильтр ближе всего к 0
    int testCountX = 0;
    for (double valX = startX; valX <= endX && testCountX < 50; valX += stepX)
    {
        testCountX++;
        if (std::abs(valX) < 1e-9) continue; // Пропускаем сам ноль

        int markX = padLeft + floor(workWidth * (valX - this->a) / (this->b - this->a));
        if (markX >= padLeft && markX <= (rightEdge - 15))
        {
            double dist = std::abs(valX); // Расстояние до начала координат

            // Берем риску, если она ближе к нулю, чем предыдущая найденная,
            // или она находится на таком же расстоянии, но при этом она положительная
            if (dist < (closestX - 1e-5) || (std::abs(dist - closestX) < 1e-5 && valX > 0.0))
            {
                closestX = dist;
                bestValX = valX;
            }
        }
    }

    // Второй проход: отрисовка рисок и вывод текста для bestValX
    for (double valX = startX; valX <= endX && iterationsCountX < 50; valX += stepX)
    {
        iterationsCountX++;
        if (std::abs(valX) < 1e-9) continue; // Пропускаем ноль (там стоит ось Y)

        int markX = padLeft + floor(workWidth * (valX - this->a) / (this->b - this->a));

        // Не рисуем риску, если она находится в пределах 15 пикселей от острия
        if (markX >= padLeft && markX <= (rightEdge - 15))
        {
            PaintBox1->Canvas->MoveTo(markX, posY - 3);
            PaintBox1->Canvas->LineTo(markX, posY + 3);

            // Подписываем риску, которую алгоритм определил как ближайшую к нулю
            if (closestX < 1e299 && std::abs(valX - bestValX) < 1e-9)
            {
                if (!CheckBox1->Checked || (CheckBox1->Checked && PaintBox1->Tag == 0))
                {
                    if (needDrawLabels && std::abs(valX) < 100000.0)
                    {
                        PaintBox1->Canvas->Brush->Style = bsClear;
                        PaintBox1->Canvas->Font->Color = clBlack;

                        AnsiString txtX = FloatToStrF(valX, ffGeneral, 4, 2);

                        if (txtX.AnsiPos(".") > 0)
                        {
                            while (txtX.Length() > 0 && txtX[txtX.Length()] == '0')
                            {
                                txtX.Delete(txtX.Length(), 1);
                            }
                            if (txtX.Length() > 0 && txtX[txtX.Length()] == '.')
                            {
                                txtX.Delete(txtX.Length(), 1);
                            }
                        }

                        int labelY = (posY + 6 + 14 > (PaintBox1->Height - padBottom)) ? (posY - 28) : (posY + 6);
                        int textWidthHalf = PaintBox1->Canvas->TextWidth(txtX) / 2;
                        PaintBox1->Canvas->TextOut(markX - textWidthHalf, labelY, txtX);
                    }
                }
            }
        }
    }

    // Динамические засечки оси Y
    double deltaY = ma - mi;
    double stepY = 0.5;

    if (deltaY > 15.0)
    {
        stepY = ceil(deltaY / 15.0);
    }
    else if (deltaY < 1.0 && deltaY > 0)
    {
        stepY = 0.1;
    }

    double startY = ceil(mi / stepY) * stepY;
    double endY = floor(ma / stepY) * stepY;
    int iterationsCount = 0;

    double closestY = 1e300;
    double bestValY = 0.0;

    // Ищем риску Y, ближайшую к нулю
    int testCountY = 0;
    for (double valY = startY; valY <= endY && testCountY < 50; valY += stepY)
    {
        testCountY++;
        if (std::abs(valY) < 1e-9) continue;

        int markY = padTop + floor(workHeight * (ma - valY) / (ma - mi)) + (mi * ma == 0 ? corr : 0);
        if (markY >= 15 && markY <= PaintBox1->Height - padBottom)
        {
            double dist = std::abs(valY);

            if (dist < (closestY - 1e-5) || (std::abs(dist - closestY) < 1e-5 && valY > 0.0))
            {
                closestY = dist;
                bestValY = valY;
            }
        }
    }

    // Второй проход: отрисовка рисок Y и вывод текста
    for (double valY = startY; valY <= endY && iterationsCount < 50; valY += stepY)
    {
        iterationsCount++;
        if (std::abs(valY) < 1e-9)
            continue;

        int markY = padTop + floor(workHeight * (ma - valY) / (ma - mi)) + (mi * ma == 0 ? corr : 0);
        if (markY >= 15 && markY <= PaintBox1->Height - padBottom)
        {
            PaintBox1->Canvas->MoveTo(posX - 3, markY);
            PaintBox1->Canvas->LineTo(posX + 3, markY);

            if (closestY < 1e299 && std::abs(valY - bestValY) < 1e-9)
            {
                if (needDrawLabels && std::abs(valY) < 100000.0)
                {
                    PaintBox1->Canvas->Brush->Style = bsClear;
                    PaintBox1->Canvas->Font->Color = clBlack;

                    AnsiString txtY = FloatToStrF(valY, ffFixed, 7, 2);

                    if (txtY.AnsiPos(".") > 0)
                    {
                        while (txtY.Length() > 0 && txtY[txtY.Length()] == '0')
                        {
                            txtY.Delete(txtY.Length(), 1);
                        }
                        if (txtY.Length() > 0 && txtY[txtY.Length()] == '.')
                        {
                            txtY.Delete(txtY.Length(), 1);
                        }
                    }

                    // Адаптивное позиционирование подписи оси Y
                    // Узнаем точную физическую ширину строки в пикселях
                    int realTextWidth = PaintBox1->Canvas->TextWidth(txtY);
                    int textX;

                    // Проверяем положение оси Y: если слева от оси слишком мало места
                    if (posX - realTextWidth - 8 < padLeft)
                    {
                        // Ось слева -> рисуем подпись справа от оси с зазором в 8 пикселей
                        textX = posX + 8;
                    }
                    else
                    {
                        // Иначе рисуем подпись слева от оси.
                        // Отнимаем ширину текста и добавляем зазор в 6 пикселей от линии
                        textX = posX - realTextWidth - 6;
                    }

                    PaintBox1->Canvas->TextOut(textX, markY - 7, txtY);
                }
            }
        }
    }

    // Подписи
    if (needDrawLabels)
    {
        PaintBox1->Canvas->Font->Name = "Arial";
        PaintBox1->Canvas->Font->Size = 10;
        PaintBox1->Canvas->Font->Color = clBlack;
        PaintBox1->Canvas->Font->Style = TFontStyles() << fsBold;
        PaintBox1->Canvas->Brush->Style = bsClear;

        int textY = posY + 6;
        if (textY + 18 > (PaintBox1->Height - padBottom))
        {
            textY = posY - 32;
        }

        rightEdge = PaintBox1->Width - padRight;
        PaintBox1->Canvas->TextOut(rightEdge - 15, textY, "X");
        PaintBox1->Canvas->TextOut(posX + 8, 4, "Y");
    }

    delete[] this->Res;
    delete[] v;
    delete[] o1;
    delete[] o2;
}

void LoadFormulasFromJSON(TComboBox *ComboBox)
{
    std::filesystem::path exeDir = std::filesystem::path(ParamStr(0).c_str()).parent_path();
    std::filesystem::path jsonPath = exeDir / "functions.json";

    String filePath = jsonPath.c_str();
    TStringList *fileContent = new TStringList();

    FormulaLimits.clear();
    ComboBox->Items->Clear();

    if (!std::filesystem::exists(jsonPath))
    {
        TJSONArray *baseArray = new TJSONArray();

        UnicodeString defaultFormulas[] = {"x^2+2*x+1", "1/x", "sin(x)",
                                           "ln(x)"};
        double defaultA[] = {-2, -0.6, -3.16, -0.02};
        double defaultB[] = {1, 0.6, -3.16, 10};

        for (int i = 0; i < 4; i++)
        {
            TJSONObject *item = new TJSONObject();
            item->AddPair("formula", defaultFormulas[i]);
            item->AddPair("a", defaultA[i]);
            item->AddPair("b", defaultB[i]);
            baseArray->AddElement(item);
        }

        fileContent->Text = baseArray->ToString();
        fileContent->SaveToFile(filePath, TEncoding::UTF8);
        delete baseArray;
    }

    TJSONArray *jsonArray = nullptr;
    try
    {
        fileContent->LoadFromFile(filePath, TEncoding::UTF8);
        jsonArray = (TJSONArray*)TJSONObject::ParseJSONValue(fileContent->Text);

        if (jsonArray != nullptr)
        {
            for (int i = 0; i < jsonArray->Count; i++)
            {
                TJSONObject *item = (TJSONObject*)jsonArray->Items[i];

                UnicodeString formula = item->Values["formula"]->Value();
                double valA = item->Values["a"]->Value().ToDouble();
                double valB = item->Values["b"]->Value().ToDouble();

                ComboBox->Items->Add(formula);

                TFormulaConfig cfg = {valA, valB};
                FormulaLimits.push_back(cfg);
            }
        }
    }
    __finally
    {
        delete fileContent;
        if (jsonArray != nullptr)
        delete jsonArray;
    }
}

void __fastcall TForm1::FormCreate(TObject *Sender)
{
    _control87(MCW_EM, MCW_EM);

    System::Sysutils::FormatSettings.DecimalSeparator = '.';

    LoadFormulasFromJSON(ComboBox1);

    ComboBox1->DropDownCount = ComboBox1->Items->Count;
    ComboBox1->ItemIndex = 0;

    if (!FormulaLimits.empty())
    {
        this->a = FormulaLimits[0].a;
        this->b = FormulaLimits[0].b;
        Edit2->Text = FloatToStr(this->a);
        Edit3->Text = FloatToStr(this->b);
    }

    ComboBox1->Focused();
}

void __fastcall TForm1::ComboKeyPress(TObject *Sender, char &Key)
{
    if (Key == VK_RETURN)
    {
        Button1->SetFocus();
        Button1Click(Button1);
        Edit2->SetFocus();
        Key = 0;
    }
}

void __fastcall TForm1::E2KeyPress(TObject *Sender, char &Key)
{
    if (Key == VK_RETURN)
    {
        Button1->SetFocus();
        Button1Click(Button1);
        Edit3->SetFocus();
        Key = 0;
    }
}

void __fastcall TForm1::E3KeyPress(TObject *Sender, char &Key)
{
    if (Key == VK_RETURN)
    {
        Button1->SetFocus();
        Button1Click(Button1);
        CheckBox1->SetFocus();
        Key = 0;
    }
}

void __fastcall TForm1::CheckBox1Click(TObject *Sender)
{
    if (!CheckBox1->Checked)
    {
        is_first_graph = true;
    }
    else
    {
        if (this->Res != nullptr)
        {
            is_first_graph = false;
        }
    }
}

void __fastcall TForm1::ToggleFullscreen()
{
    int marginTop, marginRight;

    if (!FIsFullscreen)
    {
        FOldBorderStyle = this->BorderStyle;
        FOldWindowState = this->WindowState;
        FOldLeft        = this->Left;
        FOldTop         = this->Top;
        FOldWidth       = this->Width;
        FOldHeight      = this->Height;

        this->BorderStyle = bsNone;
        this->WindowState = wsMaximized;
        FIsFullscreen = true;

        // Выносим кнопку из панели на саму форму
        Button1->Parent = this;
        Button1->Align = alNone;
        Button1->BringToFront();

        marginTop = 15;
        marginRight = 15;
    }
    else
    {
        // Возвращаем обычный режим формы
        this->WindowState = wsNormal;
        this->BorderStyle = FOldBorderStyle;
        this->Left        = FOldLeft;
        this->Top         = FOldTop;
        this->Width       = FOldWidth;
        this->Height      = FOldHeight;
        this->WindowState = FOldWindowState;

        FIsFullscreen = false;

        // Возвращаем кнопку обратно внутрь панели
        Button1->Parent = Panel1;

        marginTop = 5;
        marginRight = 5;
    }

    // Рассчитываем координаты кнопки.
    // В полноэкранном режиме ширина берется от формы (ClientWidth),
    // а в обычном - от панели (Panel1->ClientWidth).
    Button1->Top = marginTop;
    Button1->Left = (FIsFullscreen ? this->ClientWidth : Panel1->ClientWidth) - Button1->Width - marginRight;

    Application->ProcessMessages();

    if (is_first_graph)
    {
        PaintBox1->Repaint();
    }
    else
    {
        Button1Click(nullptr);
    }
}

void __fastcall TForm1::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_F11)
    {
        ToggleFullscreen();
        Key = 0;
    }
    // Выход из полноэкранного режима по клавише Esc
    else if (Key == VK_ESCAPE && FIsFullscreen)
    {
        ToggleFullscreen();
        Key = 0;
    }
}

void __fastcall TForm1::ComboBox1Change(TObject *Sender)
{
    int index = ComboBox1->ItemIndex;

    if (index >= 0 && index < (int)FormulaLimits.size())
    {
        if (!CheckBox1->Checked)
        {
            this->a = FormulaLimits[index].a;
            this->b = FormulaLimits[index].b;

            Edit2->Text = FloatToStr(this->a);
            Edit3->Text = FloatToStr(this->b);

            is_first_graph = true;
        }
        else
        {
            is_first_graph = false;
        }
        Button1Click(Button1);
    }
}
