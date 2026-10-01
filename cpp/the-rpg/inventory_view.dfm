object DualListDlg: TDualListDlg
  Left = 250
  Top = 108
  BorderStyle = bsDialog
  Caption = 'The RPG - '#1048#1085#1074#1077#1085#1090#1072#1088#1100
  ClientHeight = 314
  ClientWidth = 425
  Color = clBtnFace
  ParentFont = True
  KeyPreview = True
  OldCreateOrder = True
  Position = poScreenCenter
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 116
  TextHeight = 13
  object SrcLabel: TLabel
    Left = 60
    Top = 10
    Width = 71
    Height = 20
    AutoSize = False
    Caption = #1042' '#1088#1102#1082#1079#1072#1082#1077':'
  end
  object DstLabel: TLabel
    Left = 291
    Top = 10
    Width = 70
    Height = 20
    AutoSize = False
    Caption = #1053#1072' '#1079#1077#1084#1083#1077':'
  end
  object IncludeBtn: TSpeedButton
    Left = 197
    Top = 39
    Width = 29
    Height = 30
    Caption = '>'
    OnClick = IncludeBtnClick
  end
  object IncAllBtn: TSpeedButton
    Left = 197
    Top = 79
    Width = 29
    Height = 29
    Caption = '>>'
    OnClick = IncAllBtnClick
  end
  object ExcludeBtn: TSpeedButton
    Left = 197
    Top = 118
    Width = 29
    Height = 30
    Caption = '<'
    Enabled = False
    OnClick = ExcludeBtnClick
  end
  object ExAllBtn: TSpeedButton
    Left = 197
    Top = 158
    Width = 29
    Height = 29
    Caption = '<<'
    Enabled = False
    OnClick = ExcAllBtnClick
  end
  object Label1: TLabel
    Left = 8
    Top = 275
    Width = 32
    Height = 16
    Caption = #1042#1077#1089':'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 156
    Top = 275
    Width = 111
    Height = 16
    Caption = #1048#1079' '#1074#1086#1079#1084#1086#1078#1085#1099#1093':'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object OKBtn: TButton
    Left = 321
    Top = 275
    Width = 93
    Height = 31
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 2
    OnClick = OKBtnClick
  end
  object SrcList: TListBox
    Left = 10
    Top = 30
    Width = 177
    Height = 227
    ItemHeight = 13
    Items.Strings = (
      #1073#1091#1090#1099#1083#1082#1072' '#1087#1080#1074#1072
      #1085#1086#1078)
    MultiSelect = True
    TabOrder = 0
  end
  object DstList: TListBox
    Left = 236
    Top = 30
    Width = 178
    Height = 227
    ItemHeight = 13
    MultiSelect = True
    TabOrder = 1
  end
end
