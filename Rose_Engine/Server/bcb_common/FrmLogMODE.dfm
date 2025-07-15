object FormLogMODE: TFormLogMODE
  Left = 226
  Top = 123
  ActiveControl = RadioGroupSCR
  BorderStyle = bsDialog
  Caption = 'Set log mode'
  ClientHeight = 148
  ClientWidth = 238
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #44404#47548
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poMainFormCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 12
  object RadioGroupSCR: TRadioGroup
    Left = 4
    Top = 3
    Width = 109
    Height = 112
    Caption = 'Screen log'
    ItemIndex = 3
    Items.Strings = (
      'NONE'
      'Normal'
      'Verbose'
      'Debug')
    TabOrder = 0
  end
  object ButtonOK: TButton
    Left = 24
    Top = 121
    Width = 88
    Height = 25
    Caption = 'Ok'
    ModalResult = 1
    TabOrder = 1
    OnClick = ButtonOKClick
  end
  object ButtonCANCEL: TButton
    Left = 125
    Top = 121
    Width = 88
    Height = 25
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
    OnClick = ButtonCANCELClick
  end
  object RadioGroupFILE: TRadioGroup
    Left = 124
    Top = 3
    Width = 109
    Height = 112
    Caption = 'File log'
    ItemIndex = 1
    Items.Strings = (
      'NONE'
      'Normal'
      'Verbose'
      'Debug')
    TabOrder = 3
  end
end
