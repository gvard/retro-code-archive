#ifndef grH
#define grH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <ExtCtrls.hpp>
#include <iostream>
#include <math.h>

struct uzel
{
    double *m;
    uzel *l;
    uzel *r;
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
    void __fastcall UpdateGraphView(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ComboKeyPress(TObject *Sender, char &Key);
    void __fastcall E2KeyPress(TObject *Sender, char &Key);
    void __fastcall E3KeyPress(TObject *Sender, char &Key);
    void __fastcall CheckBox1Click(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall ToggleFullscreen();
	void __fastcall ComboBox1Change(TObject *Sender);

private:
    TBorderStyle FOldBorderStyle;
    TWindowState FOldWindowState;
    int FOldLeft, FOldTop, FOldWidth, FOldHeight;
    bool FIsFullscreen;
    TColor current_graph_color;

public:
    __fastcall TForm1(TComponent* Owner);

    int n;
    int er;
    double a;
    double b;
    AnsiString s;
    double *Mas;
    double *Res;
    uzel *p;

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

    void f(AnsiString &str, double *values, uzel *&node);
    double* initMas(double a_val, double b_val, int n_val);
};

extern PACKAGE TForm1 *Form1;

#endif

