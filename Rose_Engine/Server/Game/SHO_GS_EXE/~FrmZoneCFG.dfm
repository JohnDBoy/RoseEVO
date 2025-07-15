object FormZoneCFG: TFormZoneCFG
  Left = 146
  Top = 520
  Width = 240
  Height = 460
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSizeToolWin
  Caption = 'Zone config'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object CheckListBoxZONE: TCheckListBox
    Left = 0
    Top = 0
    Width = 232
    Height = 364
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ImeName = '한국어(한글) (MS-IME98)'
    ItemHeight = 16
    Items.Strings = (
      'zone 1'
      'zone 2'
      'zone 3'
      'zone 4'
      'zone 5'
      'zone 6'
      'zone 7'
      'zone 8'
      'zone 9'
      'zone 10')
    ParentFont = False
    TabOrder = 0
  end
  object Panel1: TPanel
    Left = 0
    Top = 364
    Width = 232
    Height = 69
    Align = alBottom
    BevelInner = bvLowered
    BevelOuter = bvLowered
    TabOrder = 1
    object ButtonCheck: TButton
      Left = 13
      Top = 6
      Width = 89
      Height = 25
      Caption = '전체 선택'
      TabOrder = 0
      OnClick = ButtonCheckClick
    end
    object ButtonOk: TButton
      Left = 27
      Top = 38
      Width = 75
      Height = 25
      Caption = 'ButtonOk'
      TabOrder = 1
    end
    object ButtonCancel: TButton
      Left = 130
      Top = 37
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'ButtonCancel'
      TabOrder = 2
    end
    object ButtonUncheck: TButton
      Left = 130
      Top = 6
      Width = 89
      Height = 25
      Anchors = [akTop, akRight]
      Caption = '전체 선택 해제'
      TabOrder = 3
    end
  end
end
