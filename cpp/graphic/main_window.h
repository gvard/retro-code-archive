#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "parser.h"
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.StdCtrls.hpp>

class TMainWindow : public TForm
{
__published:
    TPanel* Panel1;
    TComboBox* ComboBox1;
    TEdit* EditA;
    TEdit* EditB;
    TLabel* Label1;
    TLabel* Label2;
    TLabel* Label3;
    TCheckBox* CheckBox1;
    TButton* Button1;
    TPaintBox* PaintBox1;

    void __fastcall FormCreate(TObject* Sender);
    void __fastcall UpdateGraphView(TObject* Sender);
    void __fastcall ComboKeyPress(TObject* Sender, char& Key);
    void __fastcall EditAKeyPress(TObject* Sender, char& Key);
    void __fastcall EditBKeyPress(TObject* Sender, char& Key);
    void __fastcall CheckBox1Click(TObject* Sender);
    void __fastcall FormKeyDown(TObject* Sender, WORD& Key, TShiftState Shift);
    void __fastcall ComboBox1Change(TObject* Sender);

private:
    TBorderStyle FOldBorderStyle;
    TWindowState FOldWindowState;
    TColor FCurrentDynamicColor;
    int FOldLeft, FOldTop, FOldWidth, FOldHeight;
    bool FIsFullscreen;
    bool FGraphOverAxes = true;
    struct RenderContext
    {
        int posX = 0;
        int posY = 0;
        int rightEdge = 0;
        int workWidth = 0;
        int workHeight = 0;
        int corr = -1;

        double startX = 0.0;
        double endX = 0.0;
        double stepX = 0.0;
        double closestX = 1e300;
        double bestValX = 0.0;

        double startY = 0.0;
        double endY = 0.0;
        double stepY = 0.0;
        double closestY = 1e300;
        double bestValY = 0.0;

        bool needDrawLabels = true;
    };

    int FPointsCount;
    int FErrorCode;
    double FMinX;
    double FMaxX;
    AnsiString FFormulaString;
    std::vector<double> FValuesX;
    std::vector<double> FValuesY;

    double FSavedMi;
    double FSavedMa;
    double FSavedA;
    double FSavedB;
    bool FIsFirstGraph;

    int FGraphLineWidth;
    int FAxisLineWidth;
    int FGraphColor;
    int FAxisColor;
    int FBgColor;
    int FFullscreenBtnTop;
    int FFullscreenBtnRight;
    UnicodeString FFontName;
    int FFontSize;
    int FFontColor;

    void __fastcall ToggleFullscreen();

    bool ValidateInputAndParams(TObject* Sender);
    void PrepareCanvas();
    void CalculateGraphPoints();
    void RenderAxesAndCurves(TObject* Sender, TPoint* v);
    void DrawGraphCurve(TPoint* v);
    void DrawCoordinateAxes(const RenderContext& ctx);
    void DrawLabelsAndTicksText(const RenderContext& ctx);

public:
    __fastcall TMainWindow(TComponent* Owner);
    std::vector<TFormulaConfig> FFormulaLimits;
};

extern PACKAGE TMainWindow* MainWindow;

#endif
