#ifndef inventory_viewH
#define inventory_viewH

#include <Vcl.Forms.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Buttons.hpp>

class TfrmInventory : public TForm
{
__published:
    TButton* OKBtn;
    TListBox* SrcList;
    TListBox* DstList;
    TLabel* SrcLabel;
    TLabel* DstLabel;
    TSpeedButton* IncludeBtn;
    TSpeedButton* IncAllBtn;
    TSpeedButton* ExcludeBtn;
    TSpeedButton* ExAllBtn;
    TLabel* Label1;
    TLabel* Label2;

    void __fastcall IncludeBtnClick(TObject* Sender);
    void __fastcall ExcludeBtnClick(TObject* Sender);
    void __fastcall IncAllBtnClick(TObject* Sender);
    void __fastcall ExcAllBtnClick(TObject* Sender);
    void __fastcall FormShow(TObject* Sender);
    void __fastcall OKBtnClick(TObject* Sender);
	void __fastcall FormKeyDown(TObject* Sender, WORD& Key, TShiftState Shift);

private:
    void __fastcall MoveSelected(TCustomListBox* List, TStrings* Items);
    void __fastcall SetItem(TListBox* List, int Index);
    void __fastcall SetButtons();
    void update_weight_display();

    auto __fastcall GetFirstSelection(TCustomListBox* List) -> int;

public:
    explicit __fastcall TfrmInventory(TComponent* AOwner) override;
    virtual __fastcall ~TfrmInventory() = default;
};

#endif
