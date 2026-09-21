#ifndef grH
#define grH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <memory>
#include <vector>

struct TFormulaConfig
{
    double a = 0.0;
    double b = 0.0;
};

struct uzel
{
    std::vector<double> m;
    std::unique_ptr<uzel> l = nullptr;
    std::unique_ptr<uzel> r = nullptr;
};

class TForm1 : public TForm
{
__published:
	TPanel *Panel1;
	TComboBox *ComboBox1;
	TEdit *Edit2;
	TEdit *Edit3;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TCheckBox *CheckBox1;
    TButton *Button1;
    TPaintBox *PaintBox1;

    void __fastcall FormCreate(TObject *Sender);
    void __fastcall UpdateGraphView(TObject *Sender);
    void __fastcall ComboKeyPress(TObject *Sender, char &Key);
    void __fastcall E2KeyPress(TObject *Sender, char &Key);
    void __fastcall E3KeyPress(TObject *Sender, char &Key);
    void __fastcall CheckBox1Click(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall ComboBox1Change(TObject *Sender);

private:
    TBorderStyle FOldBorderStyle;
    TWindowState FOldWindowState;
    int FOldLeft, FOldTop, FOldWidth, FOldHeight;
    bool FIsFullscreen;

    std::vector<TFormulaConfig> FFormulaLimits;

    int n;
    int er;
    double a;
    double b;
    AnsiString s;
    std::vector<double> Mas;
    std::vector<double> Res;
    std::unique_ptr<uzel> p = nullptr;

    double saved_mi;
    double saved_ma;
    double saved_a;
    double saved_b;
    bool is_first_graph;

    int GraphLineWidth;
    int AxisLineWidth;
    int GraphColor;
    int AxisColor;
    int BgColor;
    int FullscreenBtnTop;
    int FullscreenBtnLeft;
    UnicodeString FontName;
    int FontSize;
    int FontColor;

    void __fastcall ToggleFullscreen();
    void __fastcall LoadFormulasFromJSON(TComboBox *ComboBox);

    void f(AnsiString &str, std::vector<double> &values, std::unique_ptr<uzel> &node);
    std::vector<double> initMas(double a_val, double b_val, int n_val);

    bool ValidateInputAndParams(TObject* Sender);
    void PrepareCanvas();
    void CalculateGraphPoints();
    void RenderAxesAndCurves(TObject *Sender, TPoint *v);

public:
    __fastcall TForm1(TComponent* Owner);
};

extern PACKAGE TForm1 *Form1;

#endif
