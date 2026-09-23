#include <vcl.h>
#pragma hdrstop
#include "main_window.h"
#include "project_model.h"

#ifdef _WIN64
  #include <xmmintrin.h>
#endif

#define _USE_MATH_DEFINES
#include <cmath>

#pragma package(smart_init)
#pragma resource "*.dfm"

TMainWindow* MainWindow;

__fastcall TMainWindow::TMainWindow(TComponent* Owner)
    : TForm(Owner)
{
    FPointsCount = 0;
    FErrorFlag = 0;
    FMinX = 0;
    FMaxX = 0;
    FIsFirstGraph = true;
    FIsFullscreen = false;
}

void __fastcall TMainWindow::UpdateGraphView(TObject* Sender)
{
    if (Sender != Button1)
    {
        FIsFirstGraph = true;
    }

    // 1. Валидация входных данных (границы, чекбоксы масштабирования)
    if (!ValidateInputAndParams(Sender))
    {
        return;
    }

    // 2. Очистка холста и заливка фона
    PrepareCanvas();

    // 3. Вычисление точек графика (инициализация массивов, вызов f())
    CalculateGraphPoints();

    if (CheckBox1->Checked)
    {
        // Генерируем новый цвет только при клике на кнопку.
        // При изменении размеров окна (Sender != Button1) используем старый сохраненный цвет.
        if (Sender == Button1)
        {
            FCurrentDynamicColor = static_cast<TColor>(RGB(rand() % 200, rand() % 200, rand() % 200));
        }
    }

    // Локальные структуры для передачи в отрисовщик (выделение памяти под динамический массив)
    std::vector<TPoint> v(this->FPointsCount + 1);

    // 4. Отрисовка осей, кривых графиков, засечек и подписей текста
    RenderAxesAndCurves(Sender, v.data());
}

bool TMainWindow::ValidateInputAndParams(TObject* Sender)
{
    if (Sender != nullptr)
    {
        try
        {
            UnicodeString strA = EditA->Text;
            UnicodeString strB = EditB->Text;
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
        EditA->Text = FloatToStr(FSavedA);
        EditB->Text = FloatToStr(FSavedB);
    }

    if (this->FErrorFlag == 1)
    {
        this->FErrorFlag = 0;
    }

    return true;
}

void TMainWindow::PrepareCanvas()
{
    if (!CheckBox1->Checked || FIsFirstGraph)
    {
        PaintBox1->Repaint();
        PaintBox1->Canvas->Brush->Color = static_cast<TColor>(this->FBgColor);
        PaintBox1->Canvas->Brush->Style = bsSolid;
        PaintBox1->Canvas->FillRect(PaintBox1->ClientRect);

    }
}

void TMainWindow::CalculateGraphPoints()
{
    this->FPointsCount = PaintBox1->Width - 1;
    this->FFormulaString = AnsiString(ComboBox1->Text);

    try
    {
        UnicodeString strA = EditA->Text;
        UnicodeString strB = EditB->Text;
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

    // Если включен режим наложения и это не первый график,
    // восстанавливаем исходные расчетные границы, чтобы initMas не ломался от прошлых перезаписей X
    if (CheckBox1->Checked && !FIsFirstGraph)
    {
        this->FMinX = FSavedA;
        this->FMaxX = FSavedB;
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
        ::f(this->FFormulaString, this->FValuesY, this->FPointsCount, this->FErrorFlag, this->FValuesX);

        if (this->FErrorFlag == 1)
        {
            ShowMessage("Ошибка в записи формулы или расстановке скобок!");
            this->FValuesY.assign(this->FPointsCount + 1, 1e300);
            return;
        }
    }
    catch (...)
    {
        // В случае критического математического сбоя заполняем массив маркерами разрыва
        for (int i = 0; i <= FPointsCount; i++)
            this->FValuesY[i] = 1e300;
    }
}

void TMainWindow::RenderAxesAndCurves(TObject* Sender, TPoint* v)
{
    TPoint o1[2];
    TPoint o2[2];

    int posY = 0;
    int posX = 0;
    int rightEdge = 0;
    int corr = -1;
    double mi = 0, ma = 0;      // Минимум и максимум по Y
    double minX = 0, maxX = 0;  // Реальные экранные границы по X
    bool needDrawLabels = (!CheckBox1->Checked || FIsFirstGraph || Sender == nullptr);

    // 1. Поиск экстремумов по Y (mi, ma) и по X (minX, maxX)
    bool firstValid = false;
    bool isParametric = (this->FFormulaString.AnsiPos(";") > 0);

    for (int i = 0; i <= FPointsCount; i++)
    {
        if (this->FValuesY[i] < 1e299 && !std::isinf(this->FValuesY[i]) && !std::isnan(this->FValuesY[i]) &&
            this->FValuesX[i] < 1e299 && !std::isinf(this->FValuesX[i]) && !std::isnan(this->FValuesX[i]))
        {
            if (!firstValid)
            {
                mi = this->FValuesY[i];
                ma = this->FValuesY[i];
                minX = this->FValuesX[i];
                maxX = this->FValuesX[i];
                firstValid = true;
            }
            else
            {
                if (mi > this->FValuesY[i])
                    mi = this->FValuesY[i];
                if (ma < this->FValuesY[i])
                    ma = this->FValuesY[i];
                if (minX > this->FValuesX[i])
                    minX = this->FValuesX[i];
                if (maxX < this->FValuesX[i])
                    maxX = this->FValuesX[i];
            }
        }
    }

    if (!firstValid)
    {
        mi = -1.0;
        ma = 1.0;
        minX = this->FMinX;
        maxX = this->FMaxX;
    }

    // Для обычного графика X-границы экрана жестко привязаны к вводу
    if (!isParametric)
    {
        minX = this->FMinX;
        maxX = this->FMaxX;
    }

    mi = (mi > 0 ? 0 : mi);
    ma = (ma < 0 ? 0 : ma);

    // Если это первый график или строим по нажатию кнопки без режима наложения графиков,
    // мы должны зафиксировать новые реальные масштабы фигуры.
    if (FIsFirstGraph || (Sender == Button1 && !CheckBox1->Checked))
    {
        FSavedMi = mi;
        FSavedMa = ma;

        if (isParametric) {
            FSavedA = minX;
            FSavedB = maxX;
        } else {
            FSavedA = this->FMinX;
            FSavedB = this->FMaxX;
        }
        FIsFirstGraph = false;
    }
    else
    {
        // Во всех остальных случаях (перерисовка окна, наложение графиков CheckBox1)
        // восстанавливаем сохраненные физические границы экрана.
        mi = FSavedMi;
        ma = FSavedMa;
        minX = FSavedA;
        maxX = FSavedB;
    }


    if (ma == 0) corr = 1;

    int padTop = 20;
    int padBottom = 14;
    int padLeft = 20;
    int padRight = 25;

    int workHeight = PaintBox1->Height - padTop - padBottom;
    int workWidth = PaintBox1->Width - padLeft - padRight;

    // Защита от деления на 0 при сборке пустых/вертикальных графиков
    if (std::abs(maxX - minX) < 1e-9) maxX = minX + 1.0;

    // 2. Цикл перевода координат с поддержкой параметрического массива X
    for (int i = 0; i <= FPointsCount; i++)
    {
        int screenX = padLeft + static_cast<int>(floor((this->FValuesX[i] - minX) * workWidth / (maxX - minX)));
        int screenY = padTop + static_cast<int>(floor(static_cast<double>(workHeight) * (ma - this->FValuesY[i]) / (ma - mi))) + (mi * ma == 0 ? corr : 0);
        v[i] = Point(screenX, screenY);
    }

    // Временно подменяем FMinX и FMaxX на физические экранные значения.
    double originalMinX = this->FMinX;
    double originalMaxX = this->FMaxX;
    this->FMinX = minX;
    this->FMaxX = maxX;

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

    RenderContext ctx;
    ctx.posX = posX;
    ctx.posY = posY;
    ctx.rightEdge = rightEdge;
    ctx.workWidth = workWidth;
    ctx.workHeight = workHeight;
    ctx.corr = corr;

    ctx.startX = startX;
    ctx.endX = endX;
    ctx.stepX = stepX;
    ctx.closestX = closestX;
    ctx.bestValX = bestValX;

    ctx.startY = startY;
    ctx.endY = endY;
    ctx.stepY = stepY;
    ctx.closestY = closestY;
    ctx.bestValY = bestValY;

    ctx.needDrawLabels = needDrawLabels;

    if (this->FGraphOverAxes)
    {
        this->DrawCoordinateAxes(ctx);
        this->DrawGraphCurve(v);
    }
    else
    {
        this->DrawGraphCurve(v);
        this->DrawCoordinateAxes(ctx);
    }

    this->DrawLabelsAndTicksText(ctx);

    this->FMinX = originalMinX;
    this->FMaxX = originalMaxX;
}

void TMainWindow::DrawGraphCurve(TPoint* v)
{
    if (CheckBox1->Checked)
    {
        PaintBox1->Canvas->Pen->Color = this->FCurrentDynamicColor;
    }
    else
    {
        PaintBox1->Canvas->Pen->Color = static_cast<TColor>(this->FGraphColor);
    }
    PaintBox1->Canvas->Pen->Width = this->FGraphLineWidth;

    bool drawing = false;
    for (int i = 0; i <= FPointsCount; i++)
    {
        if (this->FValuesY[i] < 1e299 && !std::isinf(this->FValuesY[i]) && !std::isnan(this->FValuesY[i]))
        {
            if (!drawing)
            {
                PaintBox1->Canvas->MoveTo(v[i].X, v[i].Y);
                drawing = true;
            }
            else
            {
                PaintBox1->Canvas->LineTo(v[i].X, v[i].Y);
            }
        }
        else
        {
            drawing = false;
        }
    }
}

void TMainWindow::DrawCoordinateAxes(const RenderContext& ctx)
{
    TPoint o1[2];
    TPoint o2[2];

    PaintBox1->Canvas->Pen->Color = static_cast<TColor>(this->FAxisColor);
    PaintBox1->Canvas->Brush->Color = static_cast<TColor>(this->FAxisColor);
    PaintBox1->Canvas->Pen->Width = this->FAxisLineWidth;

    // Ось X
    o1[0] = Point(20, ctx.posY); // padLeft = 20
    o1[1] = Point(ctx.rightEdge, ctx.posY);
    PaintBox1->Canvas->Polyline(o1, 1);

    TPoint arrowX[3];
    arrowX[0] = Point(ctx.rightEdge, ctx.posY);
    arrowX[1] = Point(ctx.rightEdge - 8, ctx.posY - 4);
    arrowX[2] = Point(ctx.rightEdge - 8, ctx.posY + 4);
    PaintBox1->Canvas->Polygon(arrowX, 2);

    // Ось Y
    o2[0] = Point(ctx.posX, PaintBox1->Height - 14); // padBottom = 14
    o2[1] = Point(ctx.posX, 5);
    PaintBox1->Canvas->Polyline(o2, 1);

    TPoint arrowY[3];
    arrowY[0] = Point(ctx.posX, 5);
    arrowY[1] = Point(ctx.posX - 4, 13);
    arrowY[2] = Point(ctx.posX + 4, 13);
    PaintBox1->Canvas->Polygon(arrowY, 2);

    // Риски оси X
    int iterationsCountX = 0;
    for (double valX = ctx.startX; valX <= ctx.endX && iterationsCountX < 50; valX += ctx.stepX)
    {
        iterationsCountX++;
        if (std::abs(valX) < 1e-9) continue;

        int markX = 20 + static_cast<int>(floor(static_cast<double>(ctx.workWidth) * (valX - this->FMinX) / (this->FMaxX - this->FMinX)));
        if (markX >= 20 && markX <= (ctx.rightEdge - 15))
        {
            PaintBox1->Canvas->MoveTo(markX, ctx.posY - 3);
            PaintBox1->Canvas->LineTo(markX, ctx.posY + 3);
        }
    }

    // Риски оси Y
    int iterationsCountY = 0;
    for (double valY = ctx.startY; valY <= ctx.endY && iterationsCountY < 50; valY += ctx.stepY)
    {
        iterationsCountY++;
        if (std::abs(valY) < 1e-9) continue;

        int markY = 20 + static_cast<int>(floor(static_cast<double>(ctx.workHeight) * (ctx.endY - valY) / (ctx.endY - ctx.startY))) + (ctx.startX * ctx.endX == 0 ? ctx.corr : 0);
        if (markY >= 15 && markY <= PaintBox1->Height - 14)
        {
            PaintBox1->Canvas->MoveTo(ctx.posX - 3, markY);
            PaintBox1->Canvas->LineTo(ctx.posX + 3, markY);
        }
    }
}

void TMainWindow::DrawLabelsAndTicksText(const RenderContext& ctx)
{
    if (!ctx.needDrawLabels) return;

    // Числовые подписи оси X
    int iterationsCountX = 0;
    for (double valX = ctx.startX; valX <= ctx.endX && iterationsCountX < 50; valX += ctx.stepX)
    {
        iterationsCountX++;
        if (std::abs(valX) < 1e-9) continue;

        int markX = 20 + static_cast<int>(floor(static_cast<double>(ctx.workWidth) * (valX - this->FMinX) / (this->FMaxX - this->FMinX)));

        if (markX >= 20 && markX <= (ctx.rightEdge - 15))
        {
            if (ctx.closestX < 1e299 && std::abs(valX - ctx.bestValX) < 1e-9)
            {
                if (!CheckBox1->Checked || (CheckBox1->Checked && PaintBox1->Tag == 0))
                {
                    PaintBox1->Canvas->Brush->Style = bsClear;
                    PaintBox1->Canvas->Font->Name = this->FFontName;
                    PaintBox1->Canvas->Font->Size = this->FFontSize;
                    PaintBox1->Canvas->Font->Color = static_cast<TColor>(this->FFontColor);

                    AnsiString txtX = FloatToStrF(valX, ffGeneral, 4, 2);
                    if (txtX.AnsiPos(".") > 0)
                    {
                        while (txtX.Length() > 0 && txtX[txtX.Length()] == '0') txtX.Delete(txtX.Length(), 1);
                        if (txtX.Length() > 0 && txtX[txtX.Length()] == '.') txtX.Delete(txtX.Length(), 1);
                    }

                    int labelY = (ctx.posY + 6 + 14 > (PaintBox1->Height - 14)) ? (ctx.posY - 32) : (ctx.posY + 6);
                    int textWidthHalf = PaintBox1->Canvas->TextWidth(txtX) / 2;
                    PaintBox1->Canvas->TextOut(markX - textWidthHalf, labelY, txtX);
                }
            }
        }
    }

    // Числовые подписи оси Y
    int iterationsCountY = 0;
    for (double valY = ctx.startY; valY <= ctx.endY && iterationsCountY < 50; valY += ctx.stepY)
    {
        iterationsCountY++;
        if (std::abs(valY) < 1e-9) continue;

        int markY = 20 + static_cast<int>(floor(static_cast<double>(ctx.workHeight) * (ctx.endY - valY) / (ctx.endY - ctx.startY))) + (ctx.startX * ctx.endX == 0 ? ctx.corr : 0);
        if (markY >= 15 && markY <= PaintBox1->Height - 14)
        {
            if (ctx.closestY < 1e299 && std::abs(valY - ctx.bestValY) < 1e-9)
            {
                PaintBox1->Canvas->Brush->Style = bsClear;
                PaintBox1->Canvas->Font->Name = this->FFontName;
                PaintBox1->Canvas->Font->Size = this->FFontSize;
                PaintBox1->Canvas->Font->Color = static_cast<TColor>(this->FFontColor);

                AnsiString txtY = FloatToStrF(valY, ffFixed, 7, 2);
                if (txtY.AnsiPos(".") > 0)
                {
                    while (txtY.Length() > 0 && txtY[txtY.Length()] == '0') txtY.Delete(txtY.Length(), 1);
                    if (txtY.Length() > 0 && txtY[txtY.Length()] == '.') txtY.Delete(txtY.Length(), 1);
                }

                int realTextWidth = PaintBox1->Canvas->TextWidth(txtY);
                int textX = (ctx.posX - realTextWidth - 8 < 20) ? (ctx.posX + 8) : (ctx.posX - realTextWidth - 6);
                PaintBox1->Canvas->TextOut(textX, markY - 7, txtY);
            }
        }
    }

    // Буквенные подписи X и Y
    PaintBox1->Canvas->Font->Name = this->FFontName;
    PaintBox1->Canvas->Font->Size = this->FFontSize;
    PaintBox1->Canvas->Font->Color = static_cast<TColor>(this->FFontColor);
    PaintBox1->Canvas->Font->Style = TFontStyles() << fsBold;
    PaintBox1->Canvas->Brush->Style = bsClear;

    int textY = (ctx.posY + 6 + 18 > (PaintBox1->Height - 14)) ? (ctx.posY - 32) : (ctx.posY + 6);
    PaintBox1->Canvas->TextOut(ctx.rightEdge - 15, textY, "X");
    PaintBox1->Canvas->TextOut(ctx.posX + 8, 4, "Y");
}

void __fastcall TMainWindow::FormCreate(TObject* Sender)
{
    Set8087CW(0x133F);

    #ifdef _WIN64
        // Маскирование исключений для Win64 (SSE/SSE2)
        // Компилируется и выполняется только в 64-битном режиме
        _mm_setcsr(0x1F80);
    #endif

    // this->Caption = "Ширина холста: " + IntToStr(PaintBox1->Width) + " Высота: " + IntToStr(PaintBox1->Height);
    System::Sysutils::FormatSettings.DecimalSeparator = '.';

    ::loadProgramSettings(this->FGraphLineWidth, this->FAxisLineWidth, this->FGraphColor, this->FAxisColor, this->FBgColor,
                          this->FFullscreenBtnTop, this->FFullscreenBtnRight, this->FFontName, this->FFontSize, this->FFontColor);

    ::loadFormulasFromJSON(ComboBox1, this->FFormulaLimits);

    ComboBox1->DropDownCount = ComboBox1->Items->Count;
    ComboBox1->ItemIndex = 0;

    if (!FFormulaLimits.empty())
    {
        // TODO: Обновить до minX/maxX
        this->FMinX = FFormulaLimits[0].a;
        this->FMaxX = FFormulaLimits[0].b;
        EditA->Text = FloatToStr(this->FMinX);
        EditB->Text = FloatToStr(this->FMaxX);
    }

    this->DoubleBuffered = true;
    this->FCurrentDynamicColor = static_cast<TColor>(this->FGraphColor);
    ComboBox1->Focused();
}

void __fastcall TMainWindow::ComboKeyPress(TObject* Sender, char& Key)
{
    if (Key == VK_RETURN)
    {
        Button1->SetFocus();
        UpdateGraphView(Button1);
        EditA->SetFocus();
        Key = 0;
    }
}

void __fastcall TMainWindow::EditAKeyPress(TObject* Sender, char& Key)
{
    if (Key == VK_RETURN)
    {
        Button1->SetFocus();
        UpdateGraphView(Button1);
        EditB->SetFocus();
        Key = 0;
    }
}

void __fastcall TMainWindow::EditBKeyPress(TObject* Sender, char& Key)
{
    if (Key == VK_RETURN)
    {
        Button1->SetFocus();
        UpdateGraphView(Button1);
        CheckBox1->SetFocus();
        Key = 0;
    }
}

void __fastcall TMainWindow::CheckBox1Click(TObject* Sender)
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

void __fastcall TMainWindow::ToggleFullscreen()
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
        Button1->Align = alRight;

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
        // Принудительно очищаем и красим фон
        PaintBox1->Repaint();
        PaintBox1->Canvas->Brush->Color = static_cast<TColor>(this->FBgColor);
        PaintBox1->Canvas->Brush->Style = bsSolid;
        PaintBox1->Canvas->FillRect(PaintBox1->ClientRect);

        this->FPointsCount = PaintBox1->Width - 1;

        std::vector<TPoint> v(this->FPointsCount + 1);
        RenderAxesAndCurves(nullptr, v.data());
    }
    __finally
    {
        isProcessing = false;
    }
}

void __fastcall TMainWindow::FormKeyDown(TObject* Sender, WORD& Key, TShiftState Shift)
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

void __fastcall TMainWindow::ComboBox1Change(TObject* Sender)
{
    int index = ComboBox1->ItemIndex;

    if (index >= 0 && index < static_cast<int>(FFormulaLimits.size()))
    {
        if (!CheckBox1->Checked)
        {
            this->FMinX = FFormulaLimits[index].a;
            this->FMaxX = FFormulaLimits[index].b;

            EditA->Text = FloatToStr(this->FMinX);
            EditB->Text = FloatToStr(this->FMaxX);

            FIsFirstGraph = true;
        }
        else
        {
            FIsFirstGraph = false;
        }
        UpdateGraphView(Button1);
    }
}
