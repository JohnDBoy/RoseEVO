object FormABOUT: TFormABOUT
  Left = 248
  Top = 379
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'About'
  ClientHeight = 184
  ClientWidth = 353
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = #44404#47548#52404
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poMainFormCenter
  DesignSize = (
    353
    184)
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 106
    Top = 157
    Width = 143
    Height = 24
    Anchors = [akLeft, akBottom]
    Caption = '&Okay'
    ModalResult = 1
    TabOrder = 0
    OnClick = Button1Click
    ExplicitTop = 135
  end
  object Memo1: TMemo
    Left = 0
    Top = 0
    Width = 353
    Height = 151
    Align = alTop
    Alignment = taCenter
    Color = clGray
    ImeName = #54620#44397#50612'('#54620#44544') (MS-IME98)'
    Lines.Strings = (
      ''
      'Seven Heart Indonesia'
      ''
      'Copyright (c) Serenity Mega Media'
      'All Rights Reserverd'
      ''
      'Server Programmed by RMS Noor HPD'
      'Client Programmed by Fahrezal Effendi'
      'Tools by Achmad Aulia NoorHakim')
    ReadOnly = True
    TabOrder = 1
  end
end
