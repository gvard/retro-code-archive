#ifndef main_windowH
#define main_windowH
#include "parser.h"

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>

struct TFormulaConfig
{
    double a = 0.0;
    double b = 0.0;
};

class TMainWindow : public TForm
{
__published:
    TPanel* Panel1;
    TComboBox* ComboBox1;
    TEdit* Edit2;
    TEdit* Edit3;
    TLabel* Label1;
    TLabel* Label2;
    TLabel* Label3;
    TCheckBox* CheckBox1;
    TButton* Button1;
    TPaintBox* PaintBox1;

    void __fastcall FormCreate(TObject* Sender);
    void __fastcall UpdateGraphView(TObject* Sender);
    void __fastcall ComboKeyPress(TObject* Sender, char& Key);
    void __fastcall E2KeyPress(TObject* Sender, char& Key);
    void __fastcall E3KeyPress(TObject* Sender, char& Key);
    void __fastcall CheckBox1Click(TObject* Sender);
    void __fastcall FormKeyDown(TObject* Sender, WORD& Key, TShiftState Shift);
    void __fastcall ComboBox1Change(TObject* Sender);

private:
    TBorderStyle FOldBorderStyle;
    TWindowState FOldWindowState;
    int FOldLeft, FOldTop, FOldWidth, FOldHeight;
    bool FIsFullscreen;

    int FPointsCount;
    int FErrorFlag;
    double FMinX;
    double FMaxX;
    AnsiString FFormulaString;
    std::vector<double> FValuesX;
    std::vector<double> FValuesY;
    std::unique_ptr<uzel> p = nullptr;

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

public:
    __fastcall TMainWindow(TComponent* Owner);
    std::vector<TFormulaConfig> FFormulaLimits;
};

extern PACKAGE TMainWindow* MainWindow;

#endif
