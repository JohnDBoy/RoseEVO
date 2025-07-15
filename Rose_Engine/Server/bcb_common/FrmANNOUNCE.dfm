object FormAnnounce: TFormAnnounce
  Left = 1284
  Top = 564
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'Send Announce Chat'
  ClientHeight = 75
  ClientWidth = 647
  Color = clBtnFace
  DefaultMonitor = dmMainForm
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object ButtonOK: TButton
    Left = 120
    Top = 47
    Width = 97
    Height = 25
    Caption = 'Send'
    TabOrder = 0
    OnClick = ButtonOKClick
  end
  object ButtonCANCEL: TButton
    Left = 400
    Top = 48
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 1
    OnClick = ButtonCANCELClick
  end
  object EditChat: TEdit
    Left = 5
    Top = 8
    Width = 636
    Height = 21
    ImeName = '한국어(한글) (MS-IME98)'
    TabOrder = 2
  end
end
