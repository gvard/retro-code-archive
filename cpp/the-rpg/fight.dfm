object frmFight: TfrmFight
  Left = 271
  Top = 150
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'The RPG - fight'
  ClientHeight = 410
  ClientWidth = 406
  Color = clBtnFace
  Constraints.MaxHeight = 760
  Constraints.MaxWidth = 1200
  Constraints.MinHeight = 390
  Constraints.MinWidth = 390
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  OnResize = FormResize
  OnShow = FormShow
  DesignSize = (
    406
    410)
  PixelsPerInch = 96
  TextHeight = 13
  object grEnemy: TStringGrid
    Left = 8
    Top = 8
    Width = 397
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
    Width = 397
    Height = 161
    Anchors = [akLeft, akTop, akRight, akBottom]
    DefaultRowHeight = 17
    FixedCols = 0
    RowCount = 2
    Options = [goFixedVertLine, goFixedHorzLine, goHorzLine, goColSizing]
    TabOrder = 1
    OnKeyDown = grWeaponKeyDown
    OnSelectCell = grWeaponSelectCell
    ExplicitWidth = 401
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
    Width = 406
    Height = 19
    Panels = <
      item
        Text = #1047#1076#1086#1088#1086#1074#1100#1077': '
        Width = 86
      end
      item
        Text = #1052#1072#1085#1072': '
        Width = 79
      end
      item
        Text = #1042#1099#1085#1086#1089#1083#1080#1074#1086#1089#1090#1100': '
        Width = 117
      end
      item
        Text = #1054#1095#1082#1080' '#1076#1077#1081#1089#1090#1074#1080#1103': '
        Width = 102
      end>
    ExplicitWidth = 410
  end
  object btnAttack: TButton
    Left = 174
    Top = 363
    Width = 74
    Height = 25
    Anchors = [akBottom]
    Caption = #1040#1090#1072#1082#1086#1074#1072#1090#1100'!'
    TabOrder = 2
    OnClick = btnAttackClick
    ExplicitLeft = 176
  end
end
