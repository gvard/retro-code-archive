#include <vcl.h>
#pragma hdrstop
#include "gr.h"
#include <math.h>
#include <cmath>
#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;

__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
    n = 0;
    er = 0;
    a = 0;
    b = 0;
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
            if (str[i] == '(')  bracketCount++;
            if (str[i] == ')')  bracketCount--;
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

    // Операторы: + и -
    for (int i = str.Length(); i >= 1; i--)
    {
        if (str[i] == '(')  bracketCount++;
        if (str[i] == ')')  bracketCount--;

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
    if (isOperatorFound == 1) return;

    for (int i = str.Length(); i >= 1; i--)
    {
        if (str[i] == '(')  bracketCount++;
        if (str[i] == ')')  bracketCount--;

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
                    values[j] = values[j] * (*((node->m) + j));
                }
                delete[] node->m;
                break;
            }
        }
    }

    // Оператор: /
    if (isOperatorFound == 1) return;

    for (int i = str.Length(); i >= 1; i--)
    {
        if (str[i] == '(')  bracketCount++;
        if (str[i] == ')')  bracketCount--;

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
    if (isOperatorFound == 1) return;

    for (int i = str.Length(); i >= 1; i--)
    {
        if (str[i] == '(')  bracketCount++;
        if (str[i] == ')')  bracketCount--;

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

    // Математические функции: sin, cos, ctg, tg, In
    if (isOperatorFound == 1) return;

    // Приведение строки к нижнему регистру для нечувствительности разбора (In(x) vs ln(x))
    AnsiString lowerStr = str.LowerCase();

    for (int i = 1; i <= lowerStr.Length(); i++)
    {
        if (lowerStr[i] == '(')  bracketCount++;
        if (lowerStr[i] == ')')  bracketCount--;

        if (bracketCount == 0)
        {
            functionType = 0;
            int offset = 0;

            if (lowerStr.SubString(i, 4) == "sin(")       { functionType = 1; offset = 4; }
            else if (lowerStr.SubString(i, 4) == "cos(")  { functionType = 2; offset = 4; }
            else if (lowerStr.SubString(i, 4) == "ctg(")  { functionType = 3; offset = 4; }
            else if (lowerStr.SubString(i, 3) == "tg(")   { functionType = 4; offset = 3; }
            else if (lowerStr.SubString(i, 3) == "in(")   { functionType = 5; offset = 3; }
            else if (lowerStr.SubString(i, 3) == "ln(")   { functionType = 5; offset = 3; }

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
                                values[j] = log(arg);
                            }
                            break;
                    }
                }

                delete[] node->m;
                break;
            }
        }
    }

    if (isOperatorFound == 1) return;

    // Раскрытие скобок в конце
    while (str.Length() > 0 && str[1] == '(' && str[str.Length()] == ')')
    {
        bracketCount = 1;
        int i = 1;
        while (bracketCount != 0 && i < str.Length())
        {
            i++;
            if (str[i] == '(')  bracketCount++;
            if (str[i] == ')')  bracketCount--;
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

    if (str == "x")
    {
        for (int j = 0; j <= this->n; j++)
        {
            values[j] = this->Mas[j];
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

    if (this->er == 1)
    {
        this->er = 0;
    }

    PaintBox1->Repaint();
    this->n = PaintBox1->Width - 1;

    TPoint *v = new TPoint[this->n + 1];
    TPoint *o1 = new TPoint[2];
    TPoint *o2 = new TPoint[2];

    this->s = AnsiString(ComboBox1->Text);
    for (int i = 1; i <= this->s.Length(); i++)
    {
        if (this->s[i] == '(')  bracketCount++;
        if (this->s[i] == ')')  bracketCount--;
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

    if (ma == 0)
    {
        corr = 1;
    }

    int padTop = 25;
    int padBottom = 30;
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

    PaintBox1->Canvas->Pen->Color = clBlack;
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

    // Ось X
    int posY = padTop + floor(workHeight * ma / (ma - mi) + (mi * ma == 0 ? corr : 0));
    if (posY < padTop) posY = padTop;
    if (posY > PaintBox1->Height - padBottom) posY = PaintBox1->Height - padBottom;

    o1[0] = Point(padLeft, posY);
    o1[1] = Point(PaintBox1->Width - 5, posY);
    PaintBox1->Canvas->Polyline(o1, 1);

    TPoint arrowX[3];
    arrowX[0] = Point(PaintBox1->Width - 5, posY);
    arrowX[1] = Point(PaintBox1->Width - 13, posY - 4);
    arrowX[2] = Point(PaintBox1->Width - 13, posY + 4);
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
    double startX = ceil(this->a * 2.0) / 2.0;
    double endX = floor(this->b * 2.0) / 2.0;

    for (double valX = startX; valX <= endX; valX += 0.5)
    {
        if (std::abs(valX) < 1e-9) continue;

        int markX = padLeft + floor(workWidth * (valX - this->a) / (this->b - this->a));
        if (markX >= padLeft && markX <= PaintBox1->Width - 15)
        {
            PaintBox1->Canvas->MoveTo(markX, posY - 3);
            PaintBox1->Canvas->LineTo(markX, posY + 3);
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

    for (double valY = startY; valY <= endY && iterationsCount < 50; valY += stepY)
    {
        iterationsCount++;
        if (std::abs(valY) < 1e-9) continue;

        int markY = padTop + floor(workHeight * (ma - valY) / (ma - mi)) + (mi * ma == 0 ? corr : 0);
        if (markY >= 15 && markY <= PaintBox1->Height - padBottom)
        {
            PaintBox1->Canvas->MoveTo(posX - 3, markY);
            PaintBox1->Canvas->LineTo(posX + 3, markY);
        }
    }

    // Подписи
    PaintBox1->Canvas->Font->Name = "Arial";
    PaintBox1->Canvas->Font->Size = 10;
    PaintBox1->Canvas->Font->Color = clBlack;
    PaintBox1->Canvas->Font->Style = TFontStyles() << fsBold;
    PaintBox1->Canvas->Brush->Style = bsClear;

    int textY = posY + 6;
    if (textY + 15 > PaintBox1->Height)
    {
        textY = posY - 18;
    }
    PaintBox1->Canvas->TextOut(PaintBox1->Width - 18, textY, "X");
    PaintBox1->Canvas->TextOut(posX + 8, 4, "Y");

    delete[] this->Res;
    delete[] v;
    delete[] o1;
    delete[] o2;
}

void __fastcall TForm1::FormCreate(TObject *Sender)
{
    System::Sysutils::FormatSettings.DecimalSeparator = '.';

    ComboBox1->Items->Clear();
    ComboBox1->Items->Add("x^2+2*x+1");
    ComboBox1->Items->Add("x^3-x");
    ComboBox1->Items->Add("0.1*x^3-x^2+x");
    ComboBox1->Items->Add("x^4-4*x^2");
    ComboBox1->Items->Add("x*sin(1/x)");
    ComboBox1->Items->Add("1/x");
    ComboBox1->Items->Add("1/x^2");
    ComboBox1->Items->Add("1/(1+x^2)");
    ComboBox1->Items->Add("1/(1+25*x^2)");
    ComboBox1->Items->Add("sin(x)");
    ComboBox1->Items->Add("cos(x)");
    ComboBox1->Items->Add("sin(x)*cos(x)");
    ComboBox1->Items->Add("tg(x)*ctg(x)");
    ComboBox1->Items->Add("sin(x^2)");

    ComboBox1->DropDownCount = ComboBox1->Items->Count;
    ComboBox1->ItemIndex = 0;
    ComboBox1->Focused();
}

void __fastcall TForm1::ComboKeyPress(TObject *Sender, char &Key)
{
    if (Key == VK_RETURN)  Edit2->SetFocus();
}

void __fastcall TForm1::E2KeyPress(TObject *Sender, char &Key)
{
    if (Key == VK_RETURN)  Edit3->SetFocus();
}

void __fastcall TForm1::E3KeyPress(TObject *Sender, char &Key)
{
    if (Key == VK_RETURN)  Button1->SetFocus();
}
