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
    TEdit *Edit2;
    TEdit *Edit3;
    TButton *Button1;
    TPaintBox *PaintBox1;
    TComboBox *ComboBox1;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ComboKeyPress(TObject *Sender, char &Key);
    void __fastcall E2KeyPress(TObject *Sender, char &Key);
    void __fastcall E3KeyPress(TObject *Sender, char &Key);

private:

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

    void f(AnsiString &str, double *values, uzel *&node);
    double* initMas(double a_val, double b_val, int n_val);
};

extern PACKAGE TForm1 *Form1;

#endif

