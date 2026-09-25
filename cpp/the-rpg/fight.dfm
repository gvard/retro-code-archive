object frmFight: TfrmFight
  Left = 271
  Top = 150
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'The RPG - fight'
  ClientHeight = 415
  ClientWidth = 410
  Color = clBtnFace
  Constraints.MinHeight = 415
  Constraints.MinWidth = 410
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  DesignSize = (
    410
    415)
  PixelsPerInch = 96
  TextHeight = 13
  object grEnemy: TStringGrid
    Left = 8
    Top = 8
    Width = 401
    Height = 177
    Anchors = [akLeft, akTop, akRight]
    DefaultRowHeight = 17
    FixedCols = 0
    RowCount = 2
    Options = [goFixedVertLine, goFixedHorzLine, goHorzLine]
    TabOrder = 0
    OnKeyDown = grEnemyKeyDown
    OnSelectCell = grEnemySelectCell
    ExplicitWidth = 391
    ColWidths = (
      20
      104
      157
      54
      53)
  end
  object grWeapon: TStringGrid
    Left = 8
    Top = 192
    Width = 401
    Height = 166
    Anchors = [akLeft, akTop, akRight]
    DefaultRowHeight = 17
    FixedCols = 0
    RowCount = 2
    Options = [goFixedVertLine, goFixedHorzLine, goHorzLine]
    TabOrder = 1
    OnKeyDown = grWeaponKeyDown
    OnSelectCell = grWeaponSelectCell
    ExplicitWidth = 391
    ColWidths = (
      20
      181
      64
      69
      69)
  end
  object sbBar: TStatusBar
    Left = 0
    Top = 396
    Width = 410
    Height = 19
    Panels = <
      item
        Text = 'Health: '
        Width = 100
      end
      item
        Text = #1052#1072#1085#1072': '
        Width = 100
      end
      item
        Text = 'Stam: '
        Width = 100
      end
      item
        Text = 'Action points: '
        Width = 50
      end>
  end
  object btnAttack: TButton
    Left = 176
    Top = 368
    Width = 74
    Height = 25
    Caption = #1040#1090#1072#1082#1086#1074#1072#1090#1100'!'
    TabOrder = 2
    OnClick = btnAttackClick
  end
end
