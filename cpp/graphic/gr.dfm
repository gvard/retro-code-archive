object Form1: TForm1
  Left = 200
  Top = 104
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = #1055#1086#1089#1090#1088#1086#1080#1090#1100' '#1075#1088#1072#1092#1080#1082' y=f(x) '#1086#1090' a '#1076#1086' b'
  ClientHeight = 459
  ClientWidth = 633
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object PaintBox1: TPaintBox
    Left = 8
    Top = 30
    Width = 617
    Height = 394
    OnClick = CheckBox1Click
  end
  object Label1: TLabel
    Left = 17
    Top = 5
    Width = 19
    Height = 20
    Caption = 'y='
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 403
    Top = 5
    Width = 21
    Height = 20
    Caption = 'a='
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label3: TLabel
    Left = 513
    Top = 5
    Width = 21
    Height = 20
    Caption = 'b='
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Edit2: TEdit
    Left = 430
    Top = 4
    Width = 65
    Height = 21
    TabOrder = 1
    Text = '-1'
    OnKeyPress = E2KeyPress
  end
  object Edit3: TEdit
    Left = 540
    Top = 4
    Width = 65
    Height = 21
    TabOrder = 2
    Text = '1'
    OnKeyPress = E3KeyPress
  end
  object Button1: TButton
    Left = 268
    Top = 430
    Width = 97
    Height = 25
    Caption = #1055#1086#1089#1090#1088#1086#1080#1090#1100
    TabOrder = 3
    OnClick = Button1Click
  end
  object ComboBox1: TComboBox
    Left = 42
    Top = 4
    Width = 184
    Height = 21
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    Text = 'f(x)'
    OnKeyPress = ComboKeyPress
    Items.Strings = (
      'x^2+2*x+1'
      'x^3-x'
      'cos(x)')
  end
  object CheckBox1: TCheckBox
    Left = 248
    Top = 7
    Width = 117
    Height = 15
    Caption = #1053#1077' '#1089#1090#1080#1088#1072#1090#1100
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
  end
end
