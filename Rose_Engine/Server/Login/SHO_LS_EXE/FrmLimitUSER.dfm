object FormLimitUSER: TFormLimitUSER
  Left = 269
  Top = 456
  BorderStyle = bsToolWindow
  Caption = 'Set limit user count'
  ClientHeight = 47
  ClientWidth = 262
  Color = clBtnFace
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
  object Edit1: TEdit
    Left = 8
    Top = 13
    Width = 153
    Height = 21
    ImeName = #54620#44397#50612'('#54620#44544') (MS-IME98)'
    TabOrder = 0
    Text = '0'
  end
  object Button1: TButton
    Left = 176
    Top = 12
    Width = 75
    Height = 25
    Caption = '&Set'
    TabOrder = 1
    OnClick = Button1Click
  end
end
