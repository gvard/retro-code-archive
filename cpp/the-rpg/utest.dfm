object frmUTest: TfrmUTest
  Left = 270
  Top = 274
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'The RPG - test'
  ClientHeight = 330
  ClientWidth = 534
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 116
  TextHeight = 16
  object Memo1: TMemo
    Left = 10
    Top = 15
    Width = 513
    Height = 173
    Hint = #1042#1086#1087#1088#1086#1089#1099' '#1090#1077#1089#1090#1072
    TabStop = False
    TabOrder = 2
  end
  object ListBox1: TListBox
    Left = 10
    Top = 197
    Width = 513
    Height = 90
    Hint = #1042#1072#1088#1080#1072#1085#1090#1099' '#1086#1090#1074#1077#1090#1086#1074
    ParentShowHint = False
    ShowHint = False
    TabOrder = 0
    OnDblClick = ListBox1DblClick
    OnKeyDown = ListBox1KeyDown
  end
  object Button1: TButton
    Left = 207
    Top = 295
    Width = 119
    Height = 31
    Caption = 'OK'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    OnClick = Button1Click
  end
end
