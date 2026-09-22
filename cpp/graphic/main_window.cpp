#include <vcl.h>
#pragma hdrstop
#include "gr.h"
#include "project_model.h"

#ifdef _WIN64
  #include <xmmintrin.h>
#endif

#define _USE_MATH_DEFINES
#include <cmath>

#pragma package(smart_init)
#pragma resource "*.dfm"

#include <System.JSON.hpp>
#include <filesystem>

TForm1* Form1;

__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
    FPointsCount = 0;
    FErrorFlag = 0;
    FMinX = 0;
    FMaxX = 0;
    FIsFirstGraph = true;
    FIsFullscreen = false;
}

void __fastcall TForm1::UpdateGraphView(TObject* Sender)
{
    // 1. Валидация входных данных (границы, чекбоксы масштабирования)
    if (!ValidateInputAndParams(Sender))
    {
        return;
    }

    // 2. Очистка холста и заливка фона
    PrepareCanvas();

    // 3. Вычисление точек графика (инициализация массивов, вызов f())
    CalculateGraphPoints();

    // Локальные структуры для передачи в отрисовщик (выделение памяти под динамический массив)
    std::vector<TPoint> v(this->FPointsCount + 1);

    // 4. Отрисовка осей, кривых графиков, засечек и подписей текста
    RenderAxesAndCurves(Sender, v.data());
}

bool TForm1::ValidateInputAndParams(TObject* Sender)
{
    if (Sender != nullptr)
    {
        try
        {
            UnicodeString strA = Edit2->Text;
            UnicodeString strB = Edit3->Text;
            strA = System::Sysutils::StringReplace(strA, L",", L".", TReplaceFlags() << rfReplaceAll);
            strB = System::Sysutils::StringReplace(strB, L",", L".", TReplaceFlags() << rfReplaceAll);

            double tempA = strA != "" ? strA.ToDouble() : this->FMinX;
            double tempB = strB != "" ? strB.ToDouble() : this->FMaxX;

            // Если текущие границы в полях ввода не совпадают с сохраненными границами
            if (!FIsFirstGraph && (tempA != FSavedA || tempB != FSavedB))
            {
                CheckBox1->Checked = false;
                FIsFirstGraph = true;
            }
        }
        catch (const EConvertError&)
        {
            // Ошибки конвертации обработает оригинальный try-catch ниже в коде
        }
    }

    if (CheckBox1->Checked && !FIsFirstGraph)
    {
        Edit2->Text = FloatToStr(FSavedA);
        Edit3->Text = FloatToStr(FSavedB);
    }

    if (this->FErrorFlag == 1)
    {
        this->FErrorFlag = 0;
    }

    return true;
}

void TForm1::PrepareCanvas()
{
    if (!CheckBox1->Checked)
    {
        PaintBox1->Repaint();
        PaintBox1->Canvas->Brush->Color = static_cast<TColor>(this->FBgColor);
        PaintBox1->Canvas->Brush->Style = bsSolid;
        PaintBox1->Canvas->FillRect(PaintBox1->ClientRect);

        // Очищаем старое дерево формулы, если оно существовало
        this->p = nullptr;
    }
    else
    {
        this->p = nullptr;
    }
}

void TForm1::CalculateGraphPoints()
{
    this->FPointsCount = PaintBox1->Width - 1;

    int bracketCount = 0;
    this->FFormulaString = AnsiString(ComboBox1->Text);
    // for (int i = 1; i <= this->FFormulaString.Length(); i++)
    // {
    //     if (this->FFormulaString[i] == '(')
    //         bracketCount++;
    //     if (this->FFormulaString[i] == ')')
    //         bracketCount--;
    // }

    // if (bracketCount != 0)
    // {
    //     ShowMessage("Проверьте скобки в вашем выражении!");
    //     return;
    // }

    try
    {
        UnicodeString strA = Edit2->Text;
        UnicodeString strB = Edit3->Text;
        strA = System::Sysutils::StringReplace(strA, L",", L".", TReplaceFlags() << rfReplaceAll);
        strB = System::Sysutils::StringReplace(strB, L",", L".", TReplaceFlags() << rfReplaceAll);

        if (strA != "")
            this->FMinX = strA.ToDouble();
        if (strB != "")
            this->FMaxX = strB.ToDouble();
    }
    catch (const EConvertError&)
    {
        ShowMessage("Введите границы вывода в числовом виде! Допускаются как точки, так и запятые.");
        return;
    }

    // Вызываем функцию из parser.cpp, передавая FErrorFlag по ссылке
    this->FValuesX = ::initMas(this->FMinX, this->FMaxX, this->FPointsCount, this->FErrorFlag);

    if (this->FErrorFlag == 1)
    {
        ShowMessage("Левая граница должна быть строго меньше правой!");
        return;
    }

    this->FValuesY.resize(this->FPointsCount + 1);

    try
    {
        // Вызываем функцию парсера из parser.cpp с явным пробросом параметров
        ::f(this->FFormulaString, this->FValuesY, this->p, this->FPointsCount, this->FErrorFlag, this->FValuesX);

        if (this->FErrorFlag == 1)
        {
            ShowMessage("Ошибка в записи формулы или расстановке скобок!");
        }
    }
    catch (...)
    {
        // В случае критического математического сбоя заполняем массив маркерами разрыва
        for (int i = 0; i <= FPointsCount; i++)
            this->FValuesY[i] = 1e300;
    }
}

void TForm1::RenderAxesAndCurves(TObject* Sender, TPoint* v)
{
    // Оставшийся код отрисовки:
    // 1. Поиск mi, ma и проверка на валидность точек (firstValid)
    // 2. Расчет пропорций экрана (workHeight, workWidth)
    // 3. Цикл перевода координат: v[i] = Point(...)
    // 4. Отрисовка кривой графика (MoveTo/LineTo)
    // 5. Отрисовка осей X и Y (Polyline/Polygon)
    // 6. Циклы разметки засечек оси X и оси Y
    // 7. Вывод подписей текста
    TPoint o1[2];
    TPoint o2[2];

    int posY = 0;
    int posX = 0;
    int rightEdge = 0;
    int corr = -1;
    double mi = 0, ma = 0;
    bool needDrawLabels = (!CheckBox1->Checked || FIsFirstGraph || Sender == nullptr);

    // 1. Поиск mi, ma и проверка на валидность точек (firstValid)
    bool firstValid = false;
    for (int i = 0; i <= FPointsCount; i++)
    {
        // Проверяем, что точка валидна, не бесконечна и не NaN
        if (this->FValuesY[i] < 1e299 && !std::isinf(this->FValuesY[i]) && !std::isnan(this->FValuesY[i]))
        {
            if (!firstValid)
            {
                mi = this->FValuesY[i];
                ma = this->FValuesY[i];
                firstValid = true;
            }
            else
            {
                if (mi > this->FValuesY[i])
                    mi = this->FValuesY[i];
                if (ma < this->FValuesY[i])
                    ma = this->FValuesY[i];
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

    if (Sender != nullptr && (!CheckBox1->Checked || FIsFirstGraph))
    {
        FSavedMi = mi;
        FSavedMa = ma;
        FSavedA = this->FMinX;
        FSavedB = this->FMaxX;
        FIsFirstGraph = false;
    }
    else if (Sender == nullptr || CheckBox1->Checked)
    {
        // Восстанавливаем точные пропорции при изменении экрана или наложении
        mi = FSavedMi;
        ma = FSavedMa;
        this->FMinX = FSavedA;
        this->FMaxX = FSavedB;
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

    // 2. Цикл перевода координат в экранные
    for (int i = 0; i <= FPointsCount; i++)
    {
        int screenX = padLeft + floor((double)i * workWidth / this->FPointsCount);
        int screenY = padTop + floor(workHeight * (ma - this->FValuesY[i]) / (ma - mi)) + (mi * ma == 0 ? corr : 0);
        v[i] = Point(screenX, screenY);
    }

    if (CheckBox1->Checked)
    {
        // Генерирует случайный цвет, исключая слишком светлые
        PaintBox1->Canvas->Pen->Color = static_cast<TColor>(RGB(rand() % 200, rand() % 200, rand() % 200));
    }
    else
    {
        PaintBox1->Canvas->Pen->Color = static_cast<TColor>(this->FGraphColor);
    }
    PaintBox1->Canvas->Pen->Width = this->FGraphLineWidth;

    // 3. Отрисовка кривой графика (MoveTo/LineTo)
    bool drawing = false;

    for (int i = 0; i <= FPointsCount; i++)
    {
        // Проверяем точку на валидность
        if (this->FValuesY[i] < 1e299 && !std::isinf(this->FValuesY[i]) && !std::isnan(this->FValuesY[i]))
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

    PaintBox1->Canvas->Pen->Color = static_cast<TColor>(this->FAxisColor);
    PaintBox1->Canvas->Brush->Color = static_cast<TColor>(this->FAxisColor);
    PaintBox1->Canvas->Pen->Width = this->FAxisLineWidth;

    // 4. Отрисовка оси X
    posY = padTop + floor(workHeight * ma / (ma - mi) + (mi * ma == 0 ? corr : 0));
    if (posY < padTop)
        posY = padTop;
    if (posY > PaintBox1->Height - padBottom)
        posY = PaintBox1->Height - padBottom;

    rightEdge = PaintBox1->Width - padRight;
    o1[0] = Point(padLeft, posY);
    o1[1] = Point(rightEdge, posY);
    PaintBox1->Canvas->Polyline(o1, 1);

    TPoint arrowX[3];
    arrowX[0] = Point(rightEdge, posY);
    arrowX[1] = Point(rightEdge - 8, posY - 4);
    arrowX[2] = Point(rightEdge - 8, posY + 4);
    PaintBox1->Canvas->Polygon(arrowX, 2);

    // 5. Отрисовка оси Y
    if (this->FMinX <= 0 && this->FMaxX >= 0)
    {
        posX = padLeft + floor(workWidth * (0 - this->FMinX) / (this->FMaxX - this->FMinX));
    }
    else if (this->FMinX > 0)
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

    // 6. Разметка засечек оси X
    const int MAX_MARKS_X = 20;
    double deltaX = this->FMaxX - this->FMinX;
    double stepX = 0.5;

    // Если количество рисок превышает лимит,
    // пересчитываем шаг адаптивно
    if ((deltaX / stepX) > MAX_MARKS_X)
    {
        // Выбираем более крупный шаг
        stepX = ceil(deltaX / (double)MAX_MARKS_X);
        if (stepX < 1.0)
            stepX = 1.0;
    }

    // Вычисляем стартовую и конечную точки цикла с учетом нового шага
    double startX = ceil(this->FMinX / stepX) * stepX;
    double endX = floor(this->FMaxX / stepX) * stepX;
    int iterationsCountX = 0;

    // Поиск ближайшей к нулю риски
    double closestX = 1e300; // Хранит минимальное расстояние до нуля
    double bestValX = 0.0;   // Значение координаты наилучшей риски

    // Находим, какая риска из реально прошедших фильтр ближе всего к 0
    int testCountX = 0;
    for (double valX = startX; valX <= endX && testCountX < 50; valX += stepX)
    {
        testCountX++;
        if (std::abs(valX) < 1e-9)
            continue; // Пропускаем сам ноль

        int markX = padLeft + floor(workWidth * (valX - this->FMinX) / (this->FMaxX - this->FMinX));
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
        if (std::abs(valX) < 1e-9)
            continue;

        int markX = padLeft + floor(workWidth * (valX - this->FMinX) / (this->FMaxX - this->FMinX));

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
                        PaintBox1->Canvas->Font->Name = this->FFontName;
                        PaintBox1->Canvas->Font->Size = this->FFontSize;
                        PaintBox1->Canvas->Font->Color = static_cast<TColor>(this->FFontColor);

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

                        int labelY = (posY + 6 + 14 > (PaintBox1->Height - padBottom)) ? (posY - 32) : (posY + 6);
                        int textWidthHalf = PaintBox1->Canvas->TextWidth(txtX) / 2;
                        PaintBox1->Canvas->TextOut(markX - textWidthHalf, labelY, txtX);
                    }
                }
            }
        }
    }

    // 7. Разметка засечек оси Y
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
        if (std::abs(valY) < 1e-9)
            continue;

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
                    PaintBox1->Canvas->Font->Name = this->FFontName;
                    PaintBox1->Canvas->Font->Size = this->FFontSize;
                    PaintBox1->Canvas->Font->Color = static_cast<TColor>(this->FFontColor);

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

    // 8. Вывод подписей текста (букв X и Y на краях осей)
    if (needDrawLabels)
    {
        PaintBox1->Canvas->Font->Name = this->FFontName;
        PaintBox1->Canvas->Font->Size = this->FFontSize;
        PaintBox1->Canvas->Font->Color = static_cast<TColor>(this->FFontColor);
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
}

void __fastcall TForm1::FormCreate(TObject* Sender)
{
    Set8087CW(0x133F);

    #ifdef _WIN64
        // Маскирование исключений для Win64 (SSE/SSE2)
        // Компилируется и выполняется только в 64-битном режиме
        _mm_setcsr(0x1F80);
    #endif

    System::Sysutils::FormatSettings.DecimalSeparator = '.';

    ::loadProgramSettings(this->FGraphLineWidth, this->FAxisLineWidth, this->FGraphColor, this->FAxisColor, this->FBgColor,
                          this->FFullscreenBtnTop, this->FFullscreenBtnRight, this->FFontName, this->FFontSize, this->FFontColor);

    ::loadFormulasFromJSON(ComboBox1, this->FFormulaLimits);

    ComboBox1->DropDownCount = ComboBox1->Items->Count;
    ComboBox1->ItemIndex = 0;

    if (!FFormulaLimits.empty())
    {
        this->FMinX = FFormulaLimits[0].a;
        this->FMaxX = FFormulaLimits[0].b;
        Edit2->Text = FloatToStr(this->FMinX);
        Edit3->Text = FloatToStr(this->FMaxX);
    }

    ComboBox1->Focused();
}

void __fastcall TForm1::ComboKeyPress(TObject* Sender, char& Key)
{
    if (Key == VK_RETURN)
    {
        Button1->SetFocus();
        UpdateGraphView(Button1);
        Edit2->SetFocus();
        Key = 0;
    }
}

void __fastcall TForm1::E2KeyPress(TObject* Sender, char& Key)
{
    if (Key == VK_RETURN)
    {
        Button1->SetFocus();
        UpdateGraphView(Button1);
        Edit3->SetFocus();
        Key = 0;
    }
}

void __fastcall TForm1::E3KeyPress(TObject* Sender, char& Key)
{
    if (Key == VK_RETURN)
    {
        Button1->SetFocus();
        UpdateGraphView(Button1);
        CheckBox1->SetFocus();
        Key = 0;
    }
}

void __fastcall TForm1::CheckBox1Click(TObject* Sender)
{
    if (!CheckBox1->Checked)
    {
        FIsFirstGraph = true;
    }
    else
    {
        if (!this->FValuesY.empty())
        {
            FIsFirstGraph = false;
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
        FOldLeft = this->Left;
        FOldTop = this->Top;
        FOldWidth = this->Width;
        FOldHeight = this->Height;

        this->BorderStyle = bsNone;
        this->WindowState = wsMaximized;
        FIsFullscreen = true;

        // Выносим кнопку из панели на саму форму
        Button1->Parent = this;
        Button1->Align = alNone;
        Button1->BringToFront();

        marginTop = this->FFullscreenBtnTop;
        marginRight = this->FFullscreenBtnRight;
    }
    else
    {
        // Возвращаем обычный режим формы
        this->WindowState = wsNormal;
        this->BorderStyle = FOldBorderStyle;
        this->Left = FOldLeft;
        this->Top = FOldTop;
        this->Width = FOldWidth;
        this->Height = FOldHeight;
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

    static bool isProcessing = false;

    if (isProcessing)
    {
        return;
    }

    isProcessing = true;
    try
    {
        if (FIsFirstGraph)
        {
            PaintBox1->Repaint();
        }
        else
        {
            UpdateGraphView(Button1);
        }
    }
    __finally
    {
        isProcessing = false;
    }
}

void __fastcall TForm1::FormKeyDown(TObject* Sender, WORD& Key, TShiftState Shift)
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

void __fastcall TForm1::ComboBox1Change(TObject* Sender)
{
    int index = ComboBox1->ItemIndex;

    if (index >= 0 && index < static_cast<int>(FFormulaLimits.size()))
    {
        if (!CheckBox1->Checked)
        {
            this->FMinX = FFormulaLimits[index].a;
            this->FMaxX = FFormulaLimits[index].b;

            Edit2->Text = FloatToStr(this->FMinX);
            Edit3->Text = FloatToStr(this->FMaxX);

            FIsFirstGraph = true;
        }
        else
        {
            FIsFirstGraph = false;
        }
        UpdateGraphView(Button1);
    }
}
