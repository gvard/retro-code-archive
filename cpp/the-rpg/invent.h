#ifndef inventH
#define inventH

#include <Winapi.Windows.hpp>
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Buttons.hpp>

class TDualListDlg : public TForm
{
__published:    // IDE-managed Components
    TButton *OKBtn;
    TListBox *SrcList;
    TListBox *DstList;
    TLabel *SrcLabel;
    TLabel *DstLabel;
    TSpeedButton *IncludeBtn;
    TSpeedButton *IncAllBtn;
    TSpeedButton *ExcludeBtn;
    TSpeedButton *ExAllBtn;
    TLabel *Label1;
    TLabel *Label2;

    void __fastcall IncludeBtnClick(TObject *Sender);
    void __fastcall ExcludeBtnClick(TObject *Sender);
    void __fastcall IncAllBtnClick(TObject *Sender);
    void __fastcall ExcAllBtnClick(TObject *Sender);
    void __fastcall MoveSelected(TCustomListBox *List, TStrings *Items);
    void __fastcall SetItem(TListBox *List, int Index);
    int __fastcall GetFirstSelection(TCustomListBox *List);
    void __fastcall SetButtons();
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OKBtnClick(TObject *Sender);

private:        // User declarations
    void UpdateWeightDisplay();

public:         // User declarations
    __fastcall TDualListDlg(TComponent* AOwner) override;
    __fastcall virtual ~TDualListDlg() {} // Добавлен деструктор для соблюдения единого стиля проекта
};

extern PACKAGE TDualListDlg *DualListDlg;

#endif
