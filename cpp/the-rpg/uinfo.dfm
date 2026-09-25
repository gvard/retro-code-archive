object frmUInfo: TfrmUInfo
  Left = 312
  Top = 196
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'The RPG - new game'
  ClientHeight = 174
  ClientWidth = 316
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = frmUInfoClose
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 116
  TextHeight = 16
  object Label1: TLabel
    Left = 49
    Top = 15
    Width = 66
    Height = 16
    Caption = #1042#1072#1096#1077' '#1048#1084#1103':'
  end
  object Label2: TLabel
    Left = 44
    Top = 44
    Width = 71
    Height = 16
    Caption = #1042#1072#1096#1072' '#1088#1072#1089#1072':'
  end
  object Label3: TLabel
    Left = 59
    Top = 74
    Width = 56
    Height = 16
    Caption = #1042#1072#1096' '#1087#1086#1083':'
  end
  object Label4: TLabel
    Left = 29
    Top = 98
    Width = 86
    Height = 16
    Caption = #1042#1072#1096' '#1074#1086#1079#1088#1072#1089#1090':'
  end
  object Button1: TButton
    Left = 128
    Top = 138
    Width = 90
    Height = 31
    Caption = 'OK'
    ModalResult = 1
    TabOrder = 4
    OnClick = Button1Click
  end
  object Edit1: TEdit
    Left = 128
    Top = 10
    Width = 169
    Height = 24
    TabOrder = 0
    Text = #1040#1088#1093#1077#1085
    OnKeyDown = Edit1KeyDown
  end
  object ComboBox2: TComboBox
    Left = 128
    Top = 69
    Width = 169
    Height = 24
    TabOrder = 2
    Text = #1084#1091#1078#1089#1082#1086#1081
    Items.Strings = (
      #1084#1091#1078#1089#1082#1086#1081
      #1078#1077#1085#1089#1082#1080#1081
      #1089#1088#1077#1076#1085#1080#1081
      #1075#1077#1088#1084#1072#1092#1088#1086#1076#1080#1090
      #1073#1077#1089#1087#1086#1083#1099#1081
      #1082#1072#1092#1077#1083#1100#1085#1099#1081)
  end
  object Edit2: TEdit
    Left = 128
    Top = 98
    Width = 169
    Height = 24
    TabOrder = 3
    Text = '23'
    OnKeyDown = Edit2KeyDown
  end
  object ComboBox1: TComboBox
    Left = 128
    Top = 39
    Width = 169
    Height = 24
    TabOrder = 1
    Text = #1095#1077#1083#1086#1074#1077#1082
    Items.Strings = (
      #1095#1077#1083#1086#1074#1077#1082
      #1101#1083#1100#1092
      #1075#1085#1086#1084
      #1093#1086#1073#1073#1080#1090
      #1086#1088#1082)
  end
end
