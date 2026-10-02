object frmCharStats: TfrmCharStats
  Left = 456
  Top = 313
  BorderStyle = bsDialog
  Caption = 'The RPG - '#1051#1080#1095#1085#1099#1077' '#1076#1072#1085#1085#1099#1077
  ClientHeight = 265
  ClientWidth = 217
  Color = clBtnFace
  ParentFont = True
  KeyPreview = True
  OldCreateOrder = True
  Position = poScreenCenter
  OnKeyDown = FormKeyDown
  OnShow = frmShow
  PixelsPerInch = 116
  TextHeight = 13
  object Bvl1: TBevel
    Left = 8
    Top = 15
    Width = 201
    Height = 211
    Shape = bsFrame
  end
  object Lbl1: TLabel
    Left = 28
    Top = 29
    Width = 55
    Height = 13
    Caption = #1042#1072#1096#1077' '#1048#1084#1103': '
    Color = clMenu
    ParentColor = False
  end
  object Lbl2: TLabel
    Left = 28
    Top = 48
    Width = 59
    Height = 13
    Caption = #1042#1072#1096#1072' '#1088#1072#1089#1072': '
    Color = clMenu
    ParentColor = False
  end
  object Lbl3: TLabel
    Left = 28
    Top = 67
    Width = 48
    Height = 13
    Caption = #1042#1072#1096' '#1087#1086#1083': '
    Color = clMenu
    ParentColor = False
  end
  object Lbl4: TLabel
    Left = 28
    Top = 86
    Width = 70
    Height = 13
    Caption = #1042#1072#1096' '#1074#1086#1079#1088#1072#1089#1090': '
    Color = clMenu
    ParentColor = False
  end
  object Lbl5: TLabel
    Left = 28
    Top = 120
    Width = 32
    Height = 13
    Caption = #1057#1080#1083#1072': '
    Color = clMenu
    ParentColor = False
  end
  object Lbl6: TLabel
    Left = 28
    Top = 139
    Width = 55
    Height = 13
    Caption = #1051#1086#1074#1082#1086#1089#1090#1100': '
    Color = clMenu
    ParentColor = False
  end
  object Lbl7: TLabel
    Left = 28
    Top = 158
    Width = 38
    Height = 13
    Caption = #1052#1072#1075#1080#1103': '
    Color = clMenu
    ParentColor = False
  end
  object Lbl8: TLabel
    Left = 127
    Top = 120
    Width = 56
    Height = 13
    Caption = #1047#1076#1086#1088#1086#1074#1100#1077': '
  end
  object Lbl9: TLabel
    Left = 127
    Top = 139
    Width = 33
    Height = 13
    Caption = #1052#1072#1085#1072': '
  end
  object lblStamina: TLabel
    Left = 28
    Top = 177
    Width = 79
    Height = 13
    Caption = #1042#1099#1085#1086#1089#1083#1080#1074#1086#1089#1090#1100': '
  end
  object OKBtn: TButton
    Left = 62
    Top = 232
    Width = 93
    Height = 30
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
    OnClick = OKBtnClick
  end
end
