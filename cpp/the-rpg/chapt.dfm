object frmChapt: TfrmChapt
  Left = 230
  Top = 285
  Caption = 'The RPG - Chapter'
  ClientHeight = 250
  ClientWidth = 434
  Color = clBtnFace
  Constraints.MinHeight = 260
  Constraints.MinWidth = 390
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poScreenCenter
  OnCloseQuery = frmChaptCloseQuery
  OnCreate = FormCreate
  OnResize = FormResize
  DesignSize = (
    434
    250)
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 6
    Top = 0
    Width = 423
    Height = 136
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    TabStop = False
    Align = alCustom
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    TabOrder = 2
    WantReturns = False
  end
  object ListBox1: TListBox
    Left = 6
    Top = 142
    Width = 423
    Height = 74
    Align = alCustom
    Anchors = [akLeft, akRight, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    OnDblClick = ListBox1DblClick
    OnKeyDown = ListBox1KeyDown
  end
  object Button1: TButton
    Left = 170
    Top = 222
    Width = 97
    Height = 25
    Anchors = [akBottom]
    Caption = 'OK'
    TabOrder = 1
    OnClick = Button1Click
  end
  object MainMenu1: TMainMenu
    Left = 8
    object File: TMenuItem
      Caption = '&'#1060#1072#1081#1083
      object Exit: TMenuItem
        Caption = '&'#1042#1099#1093#1086#1076
        OnClick = ExitClick
      end
    end
    object Game: TMenuItem
      Caption = '&'#1048#1075#1088#1072
      object Save: TMenuItem
        Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100
        OnClick = SaveClick
      end
      object Load: TMenuItem
        Caption = #1047#1072#1075#1088#1091#1079#1080#1090#1100
        OnClick = LoadClick
      end
      object ustype1: TMenuItem
        Caption = #1051#1080#1095#1085#1099#1077' '#1076#1072#1085#1085#1099#1077
        OnClick = ustype1Click
      end
      object N1: TMenuItem
        Caption = #1048#1085#1074#1077#1085#1090#1072#1088#1100
        OnClick = InvClick
      end
    end
    object Help: TMenuItem
      Caption = '&?'
      object Help1: TMenuItem
        Caption = #1055#1086#1084#1086#1097#1100
        OnClick = Help1Click
      end
      object About: TMenuItem
        Caption = #1054' '#1087#1088#1086#1075#1088#1072#1084#1084#1077
        OnClick = AboutClick
      end
    end
  end
  object SaveDialog1: TSaveDialog
    FileName = '*.sav'
    Filter = #1042#1072#1096#1080' '#1089#1086#1093#1088#1072#1085#1077#1085#1080#1103'|*.sav'
    Options = [ofHideReadOnly, ofNoChangeDir]
    Left = 360
    Top = 220
  end
  object OpenDialog1: TOpenDialog
    FileName = '*.sav'
    Filter = #1042#1072#1096#1080' '#1089#1086#1093#1088#1072#1085#1077#1085#1080#1103'|*.sav'
    Options = [ofHideReadOnly, ofNoChangeDir]
    Left = 60
    Top = 220
  end
end
