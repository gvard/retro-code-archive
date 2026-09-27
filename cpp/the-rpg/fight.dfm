object frmFight: TfrmFight
  Left = 271
  Top = 150
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'The RPG - fight'
  ClientHeight = 410
  ClientWidth = 400
  Color = clBtnFace
  Constraints.MaxHeight = 760
  Constraints.MaxWidth = 1250
  Constraints.MinHeight = 390
  Constraints.MinWidth = 390
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnResize = FormResize
  OnShow = FormShow
  DesignSize = (
    400
    410)
  PixelsPerInch = 96
  TextHeight = 13
  object grEnemy: TStringGrid
    Left = 8
    Top = 8
    Width = 391
    Height = 177
    Anchors = [akLeft, akTop, akRight]
    DefaultRowHeight = 17
    FixedCols = 0
    RowCount = 2
    Options = [goFixedVertLine, goFixedHorzLine, goHorzLine, goColSizing]
    TabOrder = 0
    OnKeyDown = grEnemyKeyDown
    OnSelectCell = grEnemySelectCell
    ExplicitWidth = 401
    ColWidths = (
      20
      108
      164
      50
      40)
  end
  object grWeapon: TStringGrid
    Left = 8
    Top = 191
    Width = 391
    Height = 161
    Anchors = [akLeft, akTop, akRight, akBottom]
    DefaultRowHeight = 17
    FixedCols = 0
    RowCount = 2
    Options = [goFixedVertLine, goFixedHorzLine, goHorzLine, goColSizing]
    TabOrder = 1
    OnKeyDown = grWeaponKeyDown
    OnSelectCell = grWeaponSelectCell
    ExplicitWidth = 395
    ColWidths = (
      20
      191
      60
      61
      50)
  end
  object sbBar: TStatusBar
    Left = 0
    Top = 391
    Width = 400
    Height = 19
    Panels = <
      item
        Text = 'Health: '
        Width = 86
      end
      item
        Text = #1052#1072#1085#1072': '
        Width = 86
      end
      item
        Text = 'Stam: '
        Width = 86
      end
      item
        Text = 'Action points: '
        Width = 96
      end>
    ExplicitTop = 396
    ExplicitWidth = 410
  end
  object btnAttack: TButton
    Left = 171
    Top = 363
    Width = 74
    Height = 25
    Anchors = [akBottom]
    Caption = #1040#1090#1072#1082#1086#1074#1072#1090#1100'!'
    TabOrder = 2
    OnClick = btnAttackClick
    ExplicitLeft = 176
    ExplicitTop = 368
  end
end
