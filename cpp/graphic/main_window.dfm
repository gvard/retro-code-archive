object MainWindow: TMainWindow
  Left = 200
  Top = 104
  Caption = #1055#1086#1089#1090#1088#1086#1077#1085#1080#1077' '#1082#1088#1080#1074#1099#1093' f(x) '#1080' x(t); y(t) '#1074' '#1076#1080#1072#1087#1072#1079#1086#1085#1077' [a, b]'
  ClientHeight = 480
  ClientWidth = 640
  Color = clBtnFace
  Constraints.MinHeight = 180
  Constraints.MinWidth = 120
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  OnResize = UpdateGraphView
  PixelsPerInch = 96
  TextHeight = 13
  object PaintBox1: TPaintBox
    Left = 0
    Top = 28
    Width = 640
    Height = 452
    Align = alClient
    OnClick = CheckBox1Click
    ExplicitLeft = 8
    ExplicitTop = 30
    ExplicitWidth = 617
    ExplicitHeight = 394
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 640
    Height = 28
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object Label1: TLabel
      Left = 5
      Top = 4
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
      Left = 254
      Top = 4
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
      Left = 340
      Top = 4
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
    object ComboBox1: TComboBox
      Left = 29
      Top = 3
      Width = 215
      Height = 21
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      Text = 'f(x)'
      OnChange = ComboBox1Change
      OnKeyPress = ComboKeyPress
      Items.Strings = (
        'x^2+2*x+1'
        'x^3-x'
        'cos(x)')
    end
    object EditA: TEdit
      Left = 281
      Top = 3
      Width = 50
      Height = 21
      TabOrder = 1
      Text = '-1'
      OnKeyPress = EditAKeyPress
    end
    object EditB: TEdit
      Left = 367
      Top = 3
      Width = 50
      Height = 21
      TabOrder = 2
      Text = '1'
      OnKeyPress = EditBKeyPress
    end
    object CheckBox1: TCheckBox
      Left = 427
      Top = 5
      Width = 107
      Height = 15
      Caption = #1053#1077' '#1089#1090#1080#1088#1072#1090#1100
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 3
    end
    object Button1: TButton
      AlignWithMargins = True
      Left = 550
      Top = 3
      Width = 87
      Height = 22
      Align = alRight
      Caption = #1055#1086#1089#1090#1088#1086#1080#1090#1100
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 4
      OnClick = UpdateGraphView
    end
  end
end
